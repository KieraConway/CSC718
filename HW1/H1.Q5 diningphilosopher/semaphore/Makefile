CC	= cc
CFLAGS	= -O
LDFLAGS	=

NAME	= philosopher
HDRS	= msecond.h random_int.h
OBJS	= $(NAME).o msecond.o random_int.o
LIBS	= -lpthread -lm
OUT	= $(NAME)

$(OUT):	$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(NAME).o:	$(NAME).c $(HDRS)
msecond.o:	msecond.c msecond.h
random_int.o:	random_int.c random_int.h

clean:
	rm -f $(OBJS)
	
cleanAll:
	rm -f $(OBJS) $(OUT)
	#make clean
	
#clobber:
#	/bin/rm -f $(OBJS) $(OUT)

#Needed for CLion IDE can be ignored
all:
	$(CC) $(CFLAGS) -c -o philosopher.o philosopher.c
	$(CC) $(CFLAGS) -c -o msecond.o msecond.c
	$(CC) $(CFLAGS) -c -o random_int.o random_int.c
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJS) $(LIBS)