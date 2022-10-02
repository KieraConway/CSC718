CC	= cc
CFLAGS	= -O
LDFLAGS	=

NAME	= diningphilos
HDRS	= diningphilos.h
OBJS	= $(NAME).o monitor.o 
LIBS	= -lpthread -lm
OUT	= $(NAME)

$(OUT):	$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(NAME).o:	$(NAME).c $(HDRS)
monitor.o:	monitor.c diningphilos.h
dingphilos.o:	diningphilos.c dingphilos.h

clean:
	rm -f $(OBJS)

cleanAll:
	rm -f $(OBJS) $(OUT)
	#make clean

#clobber:
#	rm -f $(OBJS) $(OUT)

#Needed for CLion IDE can be ignored
all:
	$(CC) $(CFLAGS) -c -o diningphilos.o diningphilos.c
	$(CC) $(CFLAGS) -c -o monitor.o monitor.c
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJS) $(LIBS)