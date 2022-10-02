CC = g++
CFLAGS  = -g -Wall

NAME = myhttpd2
LIBS	= -lpthread -lm

$(NAME): $(NAME).cpp
	$(CC) $(CFLAGS) -o $(NAME) $(NAME).cpp $(LIBS)

clean:
	$(RM) $(NAME)

all: $(NAME)