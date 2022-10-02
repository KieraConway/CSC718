CC = g++
CFLAGS  = -g -Wall

NAME = myhttpd3
LIBS	= -lpthread -lm

$(NAME): $(NAME).cpp
	$(CC) $(CFLAGS) -o $(NAME) $(NAME).cpp $(LIBS)

clean:
	$(RM) $(NAME)

all: $(NAME)