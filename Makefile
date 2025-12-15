NAME = webserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g
SRC = Server/Server.cpp Server/Client.cpp Configuration/config.cpp main.cpp Request/Request.cpp Request/RequestLine.cpp Request/RequestHeader.cpp \
		Response/Response.cpp

OBJ = $(SRC:.cpp=.o)

HEADER = Request/Request.hpp Request/RequestLine.hpp Request/RequestHeader.hpp \
		 Response/Response.hpp Server/Server.hpp Server/Client.hpp Configuration/config.hpp

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

%.o: %.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
