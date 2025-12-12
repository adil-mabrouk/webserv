NAME = webserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
SRC = main.cpp Server.cpp Client.cpp ../cgi/cgi.cpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

%.o: %.cpp Server.hpp Client.hpp ../cgi/cgi.hpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all