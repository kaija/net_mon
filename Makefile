CFLAGS+=-Wall
all: net_mon.o
	$(CC) -o netmon net_mon.o $(CFLAGS) $(LDFLAGS)
clean:
	rm *.o netmon
