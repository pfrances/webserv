# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/02/26 11:07:21 by pfrances          #+#    #+#              #
#    Updated: 2023/06/24 13:34:07 by pfrances         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic -MMD -MP
INCLUDE =	-I includes							\
			-I includes/cgi 					\
			-I includes/config					\
			-I includes/httpMessages			\
			-I includes/httpMessages/requests	\
			-I includes/httpMessages/response	\
			-I includes/utils

SRCS_DIR = ./srcs
OBJS_DIR = ./objs

CONFIG_DIR = $(SRCS_DIR)/config
CGI_DIR = $(SRCS_DIR)/cgi

HTTP_DIR = $(SRCS_DIR)/httpMessages
REQUEST_DIR = $(HTTP_DIR)/requests
RESPONSE_DIR = $(HTTP_DIR)/response

UTILS_DIR = $(SRCS_DIR)/utils

SRCS =	$(SRCS_DIR)/main.cpp			\
		$(SRCS_DIR)/ServerMonitor.cpp	\
		$(SRCS_DIR)/Client.cpp			\
		$(CONFIG_DIR)/Location.cpp		\
		$(CONFIG_DIR)/Server.cpp		\
		$(CGI_DIR)/CgiHandler.cpp		\
		$(HTTP_DIR)/HttpMessage.cpp		\
		$(REQUEST_DIR)/Request.cpp		\
		$(REQUEST_DIR)/DeleteReq.cpp	\
		$(REQUEST_DIR)/GetReq.cpp		\
		$(REQUEST_DIR)/PostReq.cpp		\
		$(RESPONSE_DIR)/Response.cpp	\
		$(UTILS_DIR)/ParseTools.cpp		\
		$(UTILS_DIR)/Path.cpp			\
		$(UTILS_DIR)/File.cpp

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

valgrind: CXXFLAGS += -g
valgrind: re
	valgrind ./$(NAME) confs/config.txt

.PHONY: all clean fclean re debug valgrind

-include $(DEPS)