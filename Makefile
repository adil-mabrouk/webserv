NAME = webserv
<<<<<<< HEAD
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
SRC = Server/Server.cpp Server/Client.cpp Configuration/config.cpp main.cpp

OBJ = $(SRC:.cpp=.o)
=======
SRC = main.cpp Request/Request.cpp Request/RequestLine.cpp Request/RequestHeader.cpp \
	  Response/Response.cpp
OBJ = $(SRC:.cpp=.o)
HEADER = Request/Request.hpp Request/RequestLine.hpp Request/RequestHeader.hpp \
		 Response/Response.hpp
CMP = c++
FLAGS = -Wall -Wextra -Werror -fsanitize=address -g
RM = rm -rf
>>>>>>> achallah

all: $(NAME)

$(NAME): $(OBJ)
<<<<<<< HEAD
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

%.o: %.cpp Server/Server.hpp Server/Client.hpp Configuration/config.hpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
=======
	$(CMP) $(FLAGS)	$(OBJ) -o $(NAME)

%.o : %.cpp $(HEADER)
	$(CMP) $(FLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: clean
>>>>>>> achallah
