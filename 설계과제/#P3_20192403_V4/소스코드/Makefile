CC = gcc
TARGET = ssu_monitor
SRCS = ssu_monitor.c
OBJS = ssu_monitor.o

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^

.c.o :
	$(CC) -c -o $@ $<

clean :
	rm -f $(OBJS) $(TARGET)