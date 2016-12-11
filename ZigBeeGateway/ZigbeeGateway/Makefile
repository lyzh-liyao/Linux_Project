DIR_INC = ./
DIR_SRC = ./
DIR_OBJ = ./obj
DIR_BIN = ./

SRC += $(wildcard ${DIR_SRC}/*.c) 
SRC += $(wildcard Socket/*.c)
SRC += $(wildcard UartDriver/*.c) 
OBJ = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRC}))
TARGET = ZigbeeGateway

BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = gcc
LDFLAGS= -pthread
CFLAGS = -g -Wall -I${DIR_INC}
${BIN_TARGET}:${OBJ}
	$(CC) $(OBJ)  -o $@ ${LDFLAGS}

${DIR_OBJ}/%.o:${DIR_SRC}/%.c
	$(CC) -c  $^ -o $@  $(CFLAGS)
${DIR_OBJ}/%.o:Socket/%.c
	$(CC) -c  $^ -o $@  $(CFLAGS)
${DIR_OBJ}/%.o:UartDriver/%.c
	$(CC) -c  $^ -o $@  $(CFLAGS)
.PHONY:clean
clean:
	rm -f $(DIR_OBJ)/*