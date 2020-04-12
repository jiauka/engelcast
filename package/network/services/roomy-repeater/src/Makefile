# Makefile for roomy-repeater


HGVERSION="1.9"

CFLAGS=-Wall

ifdef DEBUG
CFLAGS+= -g
else
CFLAGS+= -Os
LDFLAGS+= -s
endif

CFLAGS+= -DHGVERSION="\"${HGVERSION}\""

.PHONY: all clean

all: roomy-repeater

roomy-repeater.o: _hgversion

roomy-repeater: roomy-repeater.o


# version checking rules
.PHONY: dummy
_hgversion: dummy
	@echo $(HGVERSION) | cmp -s $@ - || echo $(HGVERSION) > $@

clean:
	-$(RM) *.o
	-$(RM) _hgversion
	-$(RM) roomy-repeater
	-$(RM) roomy-repeater-*.zip

