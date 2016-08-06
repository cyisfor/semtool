CFLAGS+=-ggdb 
LDFLAGS+=-lpthread
semtool: main.c
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS)
