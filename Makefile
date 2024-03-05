# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jinhchoi <jinhchoi@student.42seoul.kr>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/11/16 16:53:40 by gyoon             #+#    #+#              #
#    Updated: 2024/02/14 17:46:49 by jinhchoi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= webserv

# **************************************************************************** #
#                                                                              #
#    Options                                                                   #
#                                                                              #
# **************************************************************************** #

CC			= c++
# CXXFLAGS	=-fsanitize=address -g# -Wall -Wextra -Werror -std=c++98
RM			= rm -rf

.PHONY : all bonus clean fclean re

# **************************************************************************** #
#                                                                              #
#    Source Files                                                              #
#                                                                              #
# **************************************************************************** #

SRCS		= 	src/main.cpp \
				$(wildcard src/Server/*.cpp) \
				$(wildcard src/utils/*.cpp) \
				$(wildcard src/Config/*.cpp) \
				$(wildcard src/File/*.cpp)
OBJS		= 	${SRCS:.cpp=.o}
INC			= 	./include

# **************************************************************************** #
#                                                                              #
#    Progress                                                                  #
#                                                                              #
# **************************************************************************** #

num_file	= $(shell echo $(SRCS) | tr ' ' '\n' | wc -w)
cnt			:= 0
bar_done	= '🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩'
bar_ready	= '⬜️⬜️⬜️⬜️⬜️⬜️⬜️⬜️⬜️⬜️⬜️⬜️⬜️⬜️⬜️⬜️'

#			COLOR
red			= \e[1;31m
grn			= \e[1;32m
yel			= \e[1;33m
blu			= \e[1;34m
mag			= \e[1;35m
cyn			= \e[1;36m
bold		= \e[1;37m
end			= \e[0m

version = "mandatory version"

# **************************************************************************** #
#                                                                              #
#    Executable Section                                                        #
#                                                                              #
# **************************************************************************** #

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o $@ -I $(INC)
#	@printf "\rCompiling source files... "
#	@printf "%.$(shell printf "%d" 64)s" $(bar_done)
#	@printf "%7.2f%%\n" 100
#	@printf "💥 Now you can execute ${bold}$(NAME).${end}"
#	@printf " (%s)\n" $(version)

%.o : %.cpp
#	@$(eval cnt=$(shell printf $$(($(cnt)+1))))
#	@$(eval rate_hex=$(shell echo "scale=2; ($(cnt) / $(num_file)) * 16" | bc))
#	@$(eval len=$(shell printf "%.0f\n" $(rate_hex)))
#	@$(eval len_done=$(shell echo "scale=0; $(len) * 4" | bc))
#	@$(eval len_ready=$(shell echo "scale=0; 96 - ($(len_done) / 4) * 6" | bc))
#	@$(eval percent=$(shell echo "scale=4; ($(cnt) / $(num_file)) * 100" | bc))
#	@printf "\rCompiling source files... "
#	@printf "%.$(shell printf "%d" $(len_done))s" $(bar_done)
#	@printf "%.$(shell printf "%d" $(len_ready))s" $(bar_ready)
#	@printf "%7.2f%%" $(percent)
	$(CC) $(CXXFLAGS) -c $< -o $@ -I $(INC)

clean :
	@$(RM) $(OBJS)
	@printf "make clean ${bold}$(NAME).${end}\n"

fclean :
	@make clean
	@$(RM) $(NAME)
	@printf "make fclean ${bold}$(NAME).${end}\n"

re :
	@make fclean
	@printf "\n"
	@make all

# **************************************************************************** #
#                                                                              #
#    Test Section                                                              #
#                                                                              #
# **************************************************************************** #

ALL_SRCS	=	$(wildcard *.cpp) \
				$(wildcard src/*/*.cpp) \
				$(wildcard test/*.cpp)
ALL_OBJS	=	${ALL_SRCS:.cpp=.o}
ALL_TESTS	=	$(wildcard *.test)

clean_all :
	@$(RM) $(ALL_OBJS)
	@printf "make clean_all ${bold}$(NAME).${end}\n"

fclean_all :
	@make clean_all
	@$(RM) $(ALL_TESTS)
	@printf "make fclean_all ${bold}$(NAME).${end}\n"

# **************************************************************************** #

FILE_SRCS	= 	test/test_file.cpp \
				$(wildcard src/File/*.cpp)
FILE_OBJS	= 	${FILE_SRCS:.cpp=.o}

file.test: $(FILE_OBJS)
	$(CC) $(CXXFLAGS) $(FILE_OBJS) -o $@ -I $(INC)

# **************************************************************************** #

CONFIG_SRCS	=	test/test_webserv_config.cpp \
				$(wildcard src/Config/*.cpp) \
				$(wildcard src/File/*.cpp) \
				$(wildcard src/utils/*.cpp)
CONFIG_OBJS	= 	${CONFIG_SRCS:.cpp=.o}


config.test: $(CONFIG_OBJS)
	$(CC) $(CXXFLAGS) $(CONFIG_OBJS) -o $@ -I $(INC)


# **************************************************************************** #

DLISTING_SRCS	= 	test/test_directory_listing.cpp \
					$(wildcard src/File/*.cpp) \
					$(wildcard src/utils/*.cpp)

DLISTING_OBJS	= 	${DLISTING_SRCS:.cpp=.o}

dlist.test: $(DLISTING_OBJS)
	$(CC) $(CXXFLAGS) $(DLISTING_OBJS) -o $@ -I $(INC)
