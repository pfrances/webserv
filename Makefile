# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/02/26 11:07:21 by pfrances          #+#    #+#              #
#    Updated: 2023/07/18 16:43:54 by pfrances         ###   ########.fr        #
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

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

debug: CXXFLAGS += -g -fsanitize=address
debug: re

# valgrind: CXXFLAGS += -g
# valgrind: re
valgrind: all
	valgrind --leak-check=full ./$(NAME) confs/config.txt

.PHONY: all clean fclean re debug valgrind

-include $(DEPS)