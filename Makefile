CC=gcc 
CFLAGS=-Wall -lfuse `xmlrpc-c-config client --libs` `xmlrpc-c-config client --cflags` -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26 -g
wpfs: wpfs.o wpapi.o
	$(CC) $(CFLAGS) -o wpfs *.o

clean:
	rm -f wpfs *.o

