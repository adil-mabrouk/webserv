NAME = server
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
SRC = Server/Server.cpp Server/Client.cpp Server/main.cpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

%.o: %.cpp Server/Server.hpp Server/Client.hpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all