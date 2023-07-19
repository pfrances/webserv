# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/02/26 11:07:21 by pfrances          #+#    #+#              #
#    Updated: 2023/07/19 13:12:12 by pfrances         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -g -std=c++98 -pedantic -MMD -MP
INCLUDE =	-I includes							\
			-I includes/cgi 					\
			-I includes/servers					\
			-I includes/httpMessages			\
			-I includes/utils

SRCS_DIR = ./srcs
OBJS_DIR = ./objs

SERVERS_DIR = $(SRCS_DIR)/servers
CGI_DIR = $(SRCS_DIR)/cgi

HTTP_DIR = $(SRCS_DIR)/httpMessages

UTILS_DIR = $(SRCS_DIR)/utils

SRCS =	$(SRCS_DIR)/main.cpp				\
		$(SERVERS_DIR)/ServerMonitor.cpp	\
		$(SERVERS_DIR)/Location.cpp			\
		$(SERVERS_DIR)/Server.cpp			\
		$(SERVERS_DIR)/MimeTypes.cpp		\
		$(CGI_DIR)/CgiHandler.cpp			\
		$(HTTP_DIR)/HttpMessage.cpp			\
		$(HTTP_DIR)/Request.cpp				\
		$(HTTP_DIR)/Response.cpp			\
		$(UTILS_DIR)/ParseTools.cpp			\
		$(UTILS_DIR)/File.cpp				\
		$(UTILS_DIR)/Path.cpp				\
		$(UTILS_DIR)/Timer.cpp

OBJS = $(subst $(SRCS_DIR), $(OBJS_DIR), $(SRCS:.cpp=.o))

CGI-BIN_DIR = webservPages/cgi-bin
CGI_SRCS_DIR = cgi_srcs
CPP_CGI = $(CGI-BIN_DIR)/cpp_cgi
CPP_CGI_SRC = $(CGI_SRCS_DIR)/cpp_cgi.cpp
TIMEOUT_CPP_CGI = $(CGI-BIN_DIR)/timeout_cpp_cgi
TIMEOUT_CPP_CGI_SRC = $(CGI_SRCS_DIR)/timeout_cpp_cgi.cpp
CGIFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic

all: $(NAME) cgi

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(CPP_CGI): $(CPP_CGI_SRC)
	@mkdir -p $(@D)
	$(CXX) $(CGIFLAGS) $^ -o $@

$(TIMEOUT_CPP_CGI): $(TIMEOUT_CPP_CGI_SRC)
	@mkdir -p $(@D)
	$(CXX) $(CGIFLAGS) $^ -o $@

cgi: $(CPP_CGI) $(TIMEOUT_CPP_CGI)

fclean-cgi:
	rm -f $(CPP_CGI) $(TIMEOUT_CPP_CGI)

clean:
	rm -rf $(OBJS_DIR)

fclean: fclean-cgi clean
	rm -f $(NAME)

re: fclean all

debug: CXXFLAGS += -g -fsanitize=address
debug: re

# valgrind: CXXFLAGS += -g
# valgrind: re
valgrind: all
	valgrind --leak-check=full ./$(NAME) confs/config.txt

.PHONY: all clean fclean cgi fclean-cgi re debug valgrind

-include $(DEPS)