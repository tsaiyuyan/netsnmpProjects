TARGET=snmp.out

CC=gcc
SRCS=$(wildcard *.c)
HEADS=$(shell find ./include -type f -name *.h)
OBJS=$(SRCS:%.c=%.o)

INCLUDES=/usr/local/net-snmp/include/
CFLAGS=-g -O2 -Wall -I$(INCLUDES)
#-std=c++11 
LDFLAGS=-lnetsnmpmibs -lnetsnmpagent -lnetsnmp -lnetsnmpmibs -ldl  -lnetsnmpagent -lnetsnmp -lm
LIBS=-L/usr/local/net-snmp/lib

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS) $(LDFLAGS)
%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean
clean: 
	rm -rf *.o *.out
run:
	./$(TARGET)
test:
	pwd
tags:
	ctags -R -f .tags --c++-kinds=+p --fields=+iaS --extra=+q $(INCLUDES)
