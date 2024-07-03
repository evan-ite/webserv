NAME = webserv
CXX = c++
CXXFLAGS  = -Wall -Wextra -Werror -std=c++98 -g #-Wshadow -Wno-shadow
OBJDIR = obj
OBJ = $(SRC:%.cpp=$(OBJDIR)/%.o)
SRC =

all: $(NAME)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)
	@$(CXX) $(CXXFLAGS) -o $@ -c $< && printf "Compiling: $(notdir $<)\n"

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
	@rm -rf $(OBJDIR)

fclean: clean
	@-rm $(NAME)

re: clean all

.PHONY: all clean fclean re
