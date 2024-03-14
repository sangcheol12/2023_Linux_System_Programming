CC = gcc
HFLAG = -lcrypto
TARGET = ssu_backup
ADD = add
HELP = help
RECOVER = recover
REMOVE = remove
SRCS = add.c common.c hash.c help.c main.c recover.c remove.c linkedList.c
OBJS = add.o common.o hash.o help.o main.o recover.o remove.o linkedList.o
MAIN_OBJS = main.o common.o hash.o linkedList.o
ADD_OBJS = add.o common.o hash.o linkedList.o
RECOVER_OBJS = recover.o common.o linkedList.o hash.o
REMOVE_OBJS = remove.o common.o hash.o linkedList.o
HELP_OBJS = help.o

all : $(TARGET) $(ADD) $(HELP) $(RECOVER) $(REMOVE)

$(TARGET) : $(MAIN_OBJS)
	$(CC) -o $@ $^ $(HFLAG)

$(ADD) : $(ADD_OBJS)
	$(CC) -o $@ $^ $(HFLAG)

$(RECOVER) : $(RECOVER_OBJS)
	$(CC) -o $@ $^ $(HFLAG)

$(REMOVE) : $(REMOVE_OBJS)
	$(CC) -o $@ $^ $(HFLAG)

$(HELP) : $(HELP_OBJS)
	$(CC) -o $@ $^

.c.o :
	$(CC) -c -o $@ $<

clean :
	rm -f $(OBJS) $(ADD) $(HELP) $(RECOVER) $(REMOVE) $(TARGET)