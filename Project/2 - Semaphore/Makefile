CC = gcc
CFLAGS  = -g -Wall

NAME = proj_sem
LIBS	= -lpthread -lm

$(NAME): $(NAME).c
	$(CC) $(CFLAGS) -o $(NAME) $(NAME).c $(LIBS)

clean:
	$(RM) $(NAME)

all: $(NAME)
