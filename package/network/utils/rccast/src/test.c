//  gcc test.c -ljansson



#include <jansson.h>

int main() {
    json_error_t error;
    json_t *root, *obj;
    int i;
    char *ss;
 const char *type;
//    const char *json_text = "{\"a\":\"people\",\"b\":[\"human\",\"man\"]}";
//  const char *json_text = "{\"type\": \"MEDIA_STATUS\", \"status\": [{\"mediaSessionId\": 1, \"playbackRate\": 1, \"playerState\": \"IDLE\", \"currentTime\": 0, \"supportedMediaCommands\": 15, \"volume\": {\"level\": 1, \"muted\": false}, \"media\": {\"contentId\": \"http://www.clarens.com/Metis/pepe.jpg\", \"streamType\": \"NONE\", \"contentType\": \"image/jpeg\"}, \"currentItemId\": 1, \"extendedStatus\": {\"playerState\": \"LOADING\", \"media\": {\"contentId\": \"http://www.clarens.com/Metis/pepe.jpg\", \"streamType\": \"NONE\", \"contentType\": \"image/jpeg\"}}, \"repeatMode\": \"REPEAT_OFF\"}], \"requestId\": 0}";
    const char *json_text = 
"{\"requestId\": 122, \"status\": {\"applications\": [{\"appId\": \"CC1AD845\", \"displayName\": \"Default Media Receiver\", \"isIdleScreen\": true, \"launchedFromCloud\": false, \"namespaces\": [{\"name\": \"urn:x-cast:com.google.cast.debugoverlay\"}, {\"name\": \"urn:x-cast:com.google.cast.cac\"}, {\"name\": \"urn:x-cast:com.google.cast.broadcast\"}, {\"name\": \"urn:x-cast:com.google.cast.media\"}], \"sessionId\": \"f2e6ba73-8662-4106-9d73-0ece4ad14ee3\", \"statusText\": \"Default Media Receiver\", \"transportId\": \"f2e6ba73-8662-4106-9d73-0ece4ad14ee3\"}], \"volume\": {\"controlType\": \"attenuation\", \"level\": 1.0, \"muted\": false, \"stepInterval\": 0.05000000074505806}}, \"type\": \"RECEIVER_STATUS\"}";

    root=json_loads(json_text, 0, &error);

    json_t *val = json_object_get(root, "status");

    val = json_object_get(val, "applications");
    printf("applications=[%d]\n\n\n",json_array_size(val));


    val = json_array_get(val,0);
    ss=			json_dumps(val,0);
    printf("%d=%s\n\n\n",__LINE__,ss);
    free(ss);
    val = json_object_get(val, "isIdleScreen");
    int isIdleScreen = json_boolean_value(val);
    val = json_object_get(val, "displayName");
    type = json_string_value(val);


       printf("**************************************************************isIdleScreen=%d %s %d\n",isIdleScreen,type,val);

    free(ss);

    val = json_object_get(root, "displayName");

    type = json_string_value(val);
    val = json_object_get(root, "isIdleScreen");
    isIdleScreen = json_boolean_value(val);
 
       printf("**************************************************************isIdleScreen=%d %s\n",isIdleScreen, type);
    obj = json_object_get(root, "applications");
    ss=			json_dumps(obj,0);
    printf("applications=%s[%d]\n\n\n",ss,json_array_size(obj));
    free(ss);
   for(int i = 0; i< json_array_size(obj); i++){
        const char *strText;
        json_t *obj_txt;

        obj_txt = json_array_get(obj, i);
    ss=			json_dumps(obj_txt,0);
    printf("obj_txt=%s\n\n\n",ss);
    free(ss);
       if( NULL==obj_txt || !json_is_string(obj_txt) ) 
                continue;
        strText = json_string_value(obj_txt);
        printf("found %s\n", strText);
    }
            json_decref(root);


   return 0;
}
