NAME = webserv
CXX = c++
CXXFLAGS  = -Wall -Wextra -Werror -std=c++98 -g
OBJDIR = obj
OBJ = $(SRC:%.cpp=$(OBJDIR)/%.o)
SRC = srcs/main.cpp srcs/Logger.cpp srcs/Request.cpp srcs/Response.cpp \
	srcs/utils.cpp srcs/Config.cpp srcs/Webserv.cpp srcs/ResponseUtils.cpp \
	srcs/Cgi.cpp srcs/Client.cpp srcs/Server.cpp srcs/Cookie.cpp 

all: $(NAME)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)/srcs
	@$(CXX) $(CXXFLAGS) -o $@ -c $< && printf "Compiling: $(notdir $<)\n"

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

run:
	./$(NAME) content/conf/basic.conf

clean:
	@rm -rf $(OBJDIR)

fclean: clean
	@-rm $(NAME)

re: clean all

.PHONY: all clean fclean re
