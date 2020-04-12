
/*
 * 
* example
* 
#!/bin/bash
echo "http://www.clarens.com/Metis/pepe.jpg" >/tmp/test.url
echo "image/jpeg " >/tmp/test.mime
./bin/glcast -c 192.168.67.147:8009 -l /tmp/test-f -p /tmp/glcast.pid -d all=error

./bin/glcast -c 192.168.67.147:8009 -u http://www.clarens.com/Metis/pepe.jpg -m image/jpeg -f -p /tmp/glcast.pid -d all=error
./bin/glcast -c 192.168.67.147:8009 -u http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4 -m video/mp4

http://netdna.webdesignerdepot.com/uploads/2008/11/sample-graphic.jpg

*/
//#define TEST_URL

#include <stdio.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

#include "log_util.h"
#include "util.h"
#include "cast_util.h"
#include "castitf.h"
#include "cast_parse.h"
#include "castcore.h"

#define TRACK_POLL  (1000)
#define MAX_ACTION_ERRORS (5)

enum 	cCstate {  STOPPED, BUFFERING, PLAYING, PAUSED };

typedef	struct dev_param_s {
	struct in_addr		glCCIP;
	u16_t		glCCPort;
    char        URL[_STR_LEN_];
    char        mime[_STR_LEN_];
    char        loadFile[_STR_LEN_];
    bool        isIdle;
    bool        forceLoad;
	enum cCstate 	State;
	u8_t		mac[6];
    char	*PidFile;
} dev_param_t;

static bool					glGracefullShutdown = true;
static bool					glGetStatus = false;
static bool					glPrintJSON = false;
static bool                 gStatusGotten = false;
static dev_param_t   glParms;

static char usage[] =

			VERSION "\n"
		   "Usage: [options]\n"
		   "  -c <server[:port]>\tConnect to specified Chromecast, port defaults to 8009\n"
		   "  -l <file\t\t\tLoad <file>.url + <file<.mime\n"
		   "  -u <uri>\t\t\tPlay uri\n"
		   "  -m <uri>\t\t\tmime type to play\n"
		   "  -s \t\t\tGet status and exit 0 if idle\n"
		   "  -j \t\t\tPrint JSON status and exit\n"
		   "  -k \t\t\tImmediate exit on SIGQUIT and SIGTERM\n"
		   "  -p <pif file>\n"
		   "  -f \t\t\tForce load of url at first run\n"
		   "  -d <log>=<level>\t\t\tSet logging level, logs: all|main|cast, level: error|warn|info|debug|sdebug\n"
		   "\n"
		   "\n\n";


struct device_t {
	bool  Running;
	struct sCastCtx* CastCtx;
	pthread_mutex_t Mutex;
	pthread_t 		Thread;
	unsigned		TrackPoll;
	bool			TimeOut;
	enum cCstate 	State;
	u32_t			StartTime, LocalStartTime;

};

log_level	main_loglevel = lERROR;
log_level	cast_loglevel = lERROR; //lWARN, lINFO, lDEBUG, lSDEBUG
static log_level 			*loglevel = &main_loglevel;

struct device_t glDevice;
struct device_t *Device=&glDevice;
struct sCastCtx *CastCtx;
static void *MRThread(void *args);

static void _SyncNotifyState(const char *State, struct device_t* Device);
static bool AddCastDevice(struct in_addr ip, u16_t port);

/*---------------------------------------------------------------------------*/
static void sighandler(int signum) {

	if (!glGracefullShutdown) {
		LOG_INFO("forced exit", NULL);
		exit(EXIT_SUCCESS);
	}

//	Stop();
	exit(EXIT_SUCCESS);
}


/*---------------------------------------------------------------------------*/
bool ParseArgs(int argc, char **argv) {
	char *optarg = NULL;
	int optind = 1;
	int i;

#define MAXCMDLINE 256
	char cmdline[MAXCMDLINE] = "";
    const char *p = NULL;

	for (i = 0; i < argc && (strlen(argv[i]) + strlen(cmdline) + 2 < MAXCMDLINE); i++) {
		strcat(cmdline, argv[i]);
		strcat(cmdline, " ");
	}
    strcpy(glParms.mime, "image/jpeg"); //default
	while (optind < argc && strlen(argv[optind]) >= 2 && argv[optind][0] == '-') {
		char *opt = argv[optind] + 1;
		if (strstr("cumpdl", opt) && optind < argc - 1) {
			optarg = argv[optind + 1];
			optind += 2;
		} else if (strstr("ksfj", opt)) {
			optarg = NULL;
			optind += 1;
		} else {
			fprintf(stderr,"%s", usage);
			return false;
		}

		switch (opt[0]) {
		case 'c':
            server_addr(optarg, &glParms.glCCIP.s_addr, (u16_t *) &glParms.glCCPort);
			break;
		case 'f':
            glParms.forceLoad=true;
			break;

		case 'k':
			glGracefullShutdown = false;
			break;
		case 'u':
            strcpy(glParms.URL, optarg);
			break;
		case 'm':
			strcpy(glParms.mime, optarg);
			break;
		case 'j':
			glPrintJSON = true;
			break;
		case 's':
			glGetStatus = true;
			break;
		case 'l':
			strcpy(glParms.loadFile,optarg);
			break;
		case 'p':
			glParms.PidFile = optarg;
			break;
		case 'd':
			{
				char *l = strtok(optarg, "=");
				char *v = strtok(NULL, "=");
				log_level new = lERROR;
				if (l && v) {
					if (!strcmp(v, "error"))  new = lERROR;
					if (!strcmp(v, "warn"))   new = lWARN;
					if (!strcmp(v, "info"))   new = lINFO;
					if (!strcmp(v, "debug"))  new = lDEBUG;
					if (!strcmp(v, "sdebug")) new = lSDEBUG;
					if (!strcmp(l, "all") || !strcmp(l, "main"))     	main_loglevel = new;
					if (!strcmp(l, "all") || !strcmp(l, "cast"))    	cast_loglevel = new;
				} else {
					fprintf(stderr,"%s", usage);
					return false;
				}
			}
			break;
		default:
			break;
		}
	}

	return true;
}
static void RemoveCastDevice(struct device_t *Device)
{
	pthread_mutex_lock(&Device->Mutex);
	Device->Running = false;
	pthread_mutex_unlock(&Device->Mutex);
    if(Device->Thread) {
        pthread_join(Device->Thread, NULL);
    }
    DeleteCastDevice(Device->CastCtx);
}

static bool readSingleLine(const char *fileName, char *buffer)
{
    FILE* fp;
    char *p;
    fp = fopen(fileName, "r");

    p=fgets(buffer, _STR_LEN_, (FILE*) fp);
    fclose(fp);
    int len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n')
        buffer[len-1] = 0;
    return (p!=NULL);
}

int main(int argc, char *argv[])
{

    memset(&glParms,0,sizeof(glParms));
    if(!ParseArgs(argc,argv)) {
        exit(-1);
    }

    glParms.isIdle = false;
	Device->Running = false;
	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);
#if defined(SIGQUIT)
	signal(SIGQUIT, sighandler);
#endif
#if defined(SIGHUP)
	signal(SIGHUP, sighandler);
#endif
	if (glParms.PidFile) {
		FILE *pid_file;
		pid_file = fopen(glParms.PidFile, "wb");
		if (pid_file) {
			fprintf(pid_file, "%d", getpid());
			fclose(pid_file);
		} else {
			LOG_ERROR("Cannot open PID file %s", glParms.PidFile);
		}
	}
    InitSSL();  
    if(glPrintJSON) {
        int timeout=0;
        while(Device->Running == false) {
            while(!AddCastDevice(glParms.glCCIP, glParms.glCCPort)) {
                sleep(1); //wait for the CC up to 20 seconds
                timeout++;
                if(timeout > 20) 
                    exit(-1);
            }   
        }
        if(Device->CastCtx->Status == CAST_DISCONNECTED) {
            RemoveCastDevice(Device);
        } else {
            int timeout=0;
            while(!gStatusGotten) {
                CastGetStatus(Device->CastCtx);
                sleep(1);  
                timeout++;
                if(timeout > 10) {
                    exit(-1);
                } 
            }
        }
        exit(0);
    }

    if(!glGetStatus) {
        while(1) {
            while(Device->Running == false) {
                while(!AddCastDevice(glParms.glCCIP, glParms.glCCPort)) {
                    sleep(1); //wait for the CC
                }   
            }
            if(Device->CastCtx->Status == CAST_DISCONNECTED) {
                RemoveCastDevice(Device);
            } else {
                if(glParms.isIdle || glParms.forceLoad ) {
                    if(glParms.URL) {
                        if(CastLoad(Device->CastCtx,glParms.URL,glParms.mime)) {
                            if (strstr(glParms.mime, "mp4") != NULL) {
                                CastPlay(Device->CastCtx);
                            }
                            glParms.isIdle=false;
                            glParms.forceLoad =false;
                        }
                    }
                }
               sleep(1);
            }
        }
    }
    else { //glGetStatus
        int timeout=0;
        while(Device->Running == false) {
            while(!AddCastDevice(glParms.glCCIP, glParms.glCCPort)) {
                sleep(1); //wait for the CC up to 20 seconds
                timeout++;
                if(timeout > 20) 
                    exit(-1);
            }   
        }
        if(Device->CastCtx->Status == CAST_DISCONNECTED) {
            RemoveCastDevice(Device);
        } else {
            while(1) {
               int timeout=0;
               while(!gStatusGotten) {
                    CastGetStatus(Device->CastCtx);
                    sleep(1);  
                    timeout++;
                   if(timeout > 10) {
                        exit(-1);
                    }
                }
                if(glParms.isIdle) {
                   exit(0); //asked for stauts and CC is idle, all done
                }
                gStatusGotten=false;
            }
        }
    }
}


static bool AddCastDevice(struct in_addr ip, u16_t port)
{
	CastCtx=CreateCastDevice(true,ip, port, 0);
  
	Device->Running 		= true;
	Device->State 			= STOPPED;
    Device->CastCtx         =CastCtx;
    pthread_mutex_init(&Device->Mutex, 0);
    if(CastConnect(Device->CastCtx)) {
        pthread_create(&Device->Thread, NULL, &MRThread, Device);
        return true;
    } else {
        return false;
    }
}
/*----------------------------------------------------------------------------*/
static void _SyncNotifyState(const char *State, struct device_t* Device)
{
	
	/*
	DEVICE MUTEX IS LOCKED
	*/

	if (!strcasecmp(State, "CLOSED")) {
		Device->State = STOPPED;
	}

	if (!strcasecmp(State, "BUFFERING") && Device->State != BUFFERING) {
		Device->State = BUFFERING;
	}

	if (!strcasecmp(State, "STOPPED") && Device->State != STOPPED) {
		LOG_INFO("[%p]: Cast stop", Device);
		Device->State = STOPPED;
	}

	if (!strcasecmp(State, "PLAYING") && Device->State != PLAYING) {
		LOG_INFO("[%p]: Cast playing", Device);
		Device->State = PLAYING;
	}

	if (!strcasecmp(State, "PAUSED")) {
		/*
		Cast devices start "paused" so there is a first status received with
		that state, even if the play request has been sent. Make sure that
		this is filtered out and that "pause" state are only taken into account
		when already playing
		*/
		if (Device->State == PLAYING) {
			LOG_INFO("Cast pause");

			Device->State = PAUSED;
		}
	}
}

static void *MRThread(void *args)
{
	int elapsed;
	unsigned last = gettime_ms();
	struct device_t *p = (struct device_t*) args;
	json_t *data;
	while (p->Running) {
        // context is valid until this thread ends, no deletion issue
        data = GetTimedEvent(p->CastCtx, 500);
        elapsed = gettime_ms() - last;
    
        // need to protect against events from CC threads, not from deletion
        pthread_mutex_lock(&p->Mutex);
        LOG_SDEBUG("Cast thread timer %d", elapsed);
        // a message has been received
        if (data) {
            char *ss;
            ss=			json_dumps(data,0);
            if(glPrintJSON) {
                printf("%s\n\r",ss);
            }
            LOG_SDEBUG("JSONDATA=%s",ss);
            free(ss);
            json_t *val = json_object_get(data, "type");
            const char *type = json_string_value(val);
    
    
            json_t *jsStatus = json_object_get(data, "status");
            if(jsStatus) { //got status
                json_t *jsApps = json_object_get(jsStatus, "applications");
                if(jsApps) {
                    size_t size = json_array_size(jsApps);
                    for(int i=0; i < size;i++) {
                        json_t *jsApp = json_array_get(jsApps,0);
                        if(jsApp) {
                            json_t *jsIdle = json_object_get(jsApp, "isIdleScreen");
                            if(jsIdle) {
                                gStatusGotten=true;
                                if(json_typeof(jsIdle)  ==JSON_TRUE) {
                                    glParms.isIdle=true;
                                } else{
                                    glParms.isIdle=false;
                                }
                                break;
                            }
                        }
                    }
                }
                // a mediaSessionId has been acquired
                if (type && !strcasecmp(type, "MEDIA_STATUS")) {
                    const char *url;
                    const char *state = GetMediaItem_S(data, 0, "playerState");
                    // so far, buffering and playing can be merged
                    if (state && (!strcasecmp(state, "PLAYING") || !strcasecmp(state, "BUFFERING"))) {
                        _SyncNotifyState("PLAYING", p);
                    }
    
                    if (state && !strcasecmp(state, "PAUSED")) {
                        _SyncNotifyState("PAUSED", p);
                    }
    
                    if (state && !strcasecmp(state, "IDLE")) {
                        //const char *cause = GetMediaItem_S(data, 0, "idleReason");
                       _SyncNotifyState("STOPPED", p);
                    }
    
                    /*
                    Discard any time info unless we are confirmed playing. Cast
                    devices seems to report time according to seekpoint, so in case
                    difference is too large, it means that we have a LMS repositioning
                    */
                    if (p->State == PLAYING && CastIsMediaSession(p->CastCtx)) {
                        u32_t elapsed = 1000L * GetMediaItem_F(data, 0, "currentTime");
#if 0 //!defined(REPOS_TIME)
                        // LMS reposition time can be a bit BEFORE seek time ...
                        if (gettime_ms() - p->LocalStartTime + 5000 - elapsed > 0x7fffffff) {
                            if (elapsed > p->StartTime)	elapsed -= p->StartTime;
                            else elapsed = 0;
                        }
#endif
                    }
    
                    url = GetMediaInfoItem_S(data, 0, "contentId");
                    LOG_INFO ("Playing URL %s", url);
                }
    
                // Cast devices has closed the connection
                if (type && !strcasecmp(type, "CLOSE")) {
                    // _SyncNotifyState("CLOSED", p);
                }
            }
            json_decref(data);
        }
    
    
        // get track position & CurrentURI
        p->TrackPoll += elapsed;
        if (p->TrackPoll > TRACK_POLL) {
            p->TrackPoll = 0;
            if (p->State != STOPPED)  {
                CastGetCurrentMediaStatus(p->CastCtx);
                CastGetStatus(p->CastCtx);
            }
        }
    
        pthread_mutex_unlock(&p->Mutex);
        last = gettime_ms();
        usleep(1000);
    
    }
	return NULL;
}

