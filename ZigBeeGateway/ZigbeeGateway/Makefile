CFLAGS+=-pthread
LDFLAGS+=-pthread
CC=gcc
RM=rm
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
TARGET=ZigbeeGateway

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
$(OBJS):$(SRCS)
	$(CC) -c $^ -I ./
clean:
	@$(RM) -f $(TARGET) $(OBJS)
