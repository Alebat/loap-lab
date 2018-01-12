.PHONY : all clean

CFLAGS := -Wall -std=gnu99 -D_GNU_SOURCE -L/usr/lib -lbluetooth -lm
DIRS := $(shell find . -type d)
COMM_FILES_DIR := ../spam/common
DIRS += $(shell find $(COMM_FILES_DIR) -type d)
FILES := $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))
OBJS := $(addsuffix .o, $(basename $(FILES)))
APP := BrickClient

all: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LDFLAGS) -o $(APP)

clean:
	rm -f $(OBJS) $(APP) $(OBJS:.o=.d)

%.d: %.c
	$(CC) $(CFLAGS) -MM -MF $@ $<

-include $(OBJS:.o=.d)
