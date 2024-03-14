CC = gcc
TARGET = ssu_score
SRC_DIR = source/
SRCS = $(SRC_DIR)main.c $(SRC_DIR)ssu_score.c $(SRC_DIR)blank.c
OBJS = $(SRC_DIR)main.o $(SRC_DIR)ssu_score.o $(SRC_DIR)blank.o

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^

.c.o :
	$(CC) -c -o $@ $<

clean :
	rm -f $(OBJS) $(TARGET)