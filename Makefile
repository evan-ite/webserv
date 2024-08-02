NAME = webserv
CXX = c++
CXXFLAGS  = -Wall -Wextra -Werror -std=c++98 -g
OBJDIR = obj
OBJ = $(SRC:%.cpp=$(OBJDIR)/%.o)
SRC = srcs/main.cpp srcs/Logger.cpp srcs/Request.cpp srcs/Response.cpp \
	srcs/utils.cpp srcs/Config.cpp srcs/Webserv.cpp srcs/ResponseUtils.cpp \
	srcs/Cgi.cpp srcs/Client.cpp srcs/Server.cpp srcs/Cookie.cpp srcs/ConfigUtils.cpp

all: $(NAME) key

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)/srcs
	@$(CXX) $(CXXFLAGS) -o $@ -c $< && printf "Compiling: $(notdir $<)\n"

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

run: all
	./$(NAME) content/conf/basic.conf

clean:
	@rm -rf $(OBJDIR) user_sessions.db

fclean: clean
	@-rm $(NAME) user_sessions.db

re: clean all

key:
	@python3 utils/generate_key.py

test:
	python3 utils/tests/stresstest.py http://localhost:8080 --num-requests 50 --sleep-time 0.1
	python3 utils/tests/connect_test.py localhost:8080


.PHONY: all clean fclean re test key run
