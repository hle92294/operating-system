CC			= gcc 
CFLAGS		= -Wall
LDFLAGS		= -l pthread
SINGLE 		= single_thread.c 
MULTI 		= multi_thread.c
OUT_1 		= singl_thread
OUT_2 		= mult_thread

build: $(SINGLE) $(MULTI)
		$(CC) -o $(OUT_1) $(SINGLE) $(LDFLAGS)
		$(CC) -o $(OUT_2) $(MULTI) $(LDFLAGS)
clean:
	rm -f *.o core

rebuild: clean build