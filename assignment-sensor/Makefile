CC=gcc
CFLAGS=-Wall -O2
TARGET=assignment-sensor

all: $(TARGET)

$(TARGET): src/main.c
	$(CC) $(CFLAGS) -o $(TARGET) src/main.c

clean:
	rm -f $(TARGET)
