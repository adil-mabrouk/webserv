NAME = webserv
SRC = main.cpp Request/Request.cpp Request/RequestLine.cpp Request/RequestHeader.cpp \
	  Response/Response.cpp
OBJ = $(SRC:.cpp=.o)
HEADER = Request/Request.hpp Request/RequestLine.hpp Request/RequestHeader.hpp \
		 Response/Response.hpp
CMP = c++
FLAGS = -Wall -Wextra -Werror -fsanitize=address -g
RM = rm -rf

all: $(NAME)

$(NAME): $(OBJ)
	$(CMP) $(FLAGS)	$(OBJ) -o $(NAME)

%.o : %.cpp $(HEADER)
	$(CMP) $(FLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: clean
