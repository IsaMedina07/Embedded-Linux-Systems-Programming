CC = gcc
CFLAGS = -O2 -Wall -Wextra -std=gnu11

SRC = src/assignment_sensor.c
OUTDIR = build
OUT = $(OUTDIR)/assignment-sensor

all: $(OUT)

$(OUT): $(SRC)
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

install: $(OUT)
	sudo cp $(OUT) /usr/local/bin/assignment-sensor

uninstall:
	sudo rm -f /usr/local/bin/assignment-sensor
	sudo rm -f /etc/systemd/system/assignment-sensor.service
	sudo systemctl daemon-reload

clean:
	rm -rf $(OUTDIR)
