# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/20 21:11:03 by pharbst           #+#    #+#              #
#    Updated: 2024/02/27 18:15:22 by pharbst          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

include color.mk

ifeq ($(UNAME), Darwin)
PRONAME = socketManager
SSLCFLAGS	:= -D__SSL__ -I$(shell brew --prefix)/opt/openssl@3/include
SSLLDFLAGS	:= -L$(shell brew --prefix)/opt/openssl@3/lib -lssl -lcrypto
else ifeq ($(UNAME), Linux)
PRONAME = socketManager_linux
endif

CC		= c++

CFLAGS	= -Wall -Wextra -Werror -MMD -MP -g -std=c++98 -lssl -lcrypto $(INC_DIR)
# -MMD and -MP are used to create dependecy files

INC_DIR	= 	-I./include/

# add source files with header with the same name
SOURCE	=	socketManager.cpp

HEADER	= $(addprefix $(INC_DIR), $(SOURCE:.cpp=.hpp))

# add other header files here
HEADER	+= 

# add source files without header with the same name and the file with the main function has to be the first in the list
SRCS	=	test.cpp \
			socketManagerInit.cpp \
			socketManagerSSL.cpp \
			socketManagerSEPOLL.cpp \
			$(SOURCE)

OBJ_DIR	= ./obj/
ifeq ($(UNAME), Darwin)
OBJ_DIR = ./obj/mac/
else ifeq ($(UNAME), Linux)
OBJ_DIR = ./obj/linux/
endif

OBJS = $(addprefix $(OBJ_DIR), $(SRCS:.cpp=.o))


# in case of subdirectories in the src folder add them here
VPATH := src src/socketManager src/Interface src/config src/error src/httpTransfer

all:
	@$(MAKE) -s proname_header
	@$(MAKE) -s std_all

test: $(PRONAME)
	@$(MAKE) -s proname_header
	@$(MAKE) -s std_all
	@c++ $(CFLAGS) -o test test.cpp $(PRONAME)
	@./test

# ssl:
# 	@$(MAKE) -s proname_header
# ifeq ($(UNAME), Darwin)
# 	@$(MAKE) -s std_all SSLCFLAGS="-D__SSL__ -I$(shell brew --prefix)/opt/openssl@3/include" SSLLDFLAGS="-L$(shell brew --prefix)/opt/openssl@3/lib -lssl -lcrypto"
# else ifeq ($(UNAME), Linux)
# 	@$(MAKE) -s std_all -D__SSL__ -lssl -lcrypto
# endif

std_all:
	@printf "%s$(RESET)\n" "$(FPurple)Compiling $(PRONAME)"
	@-include $(OBJS:.o=.d)
	@printf "$(SETCURUP)$(CLEARLINE)"
	@$(MAKE) -s $(PRONAME)
	@printf "$(SETCURUP)$(CLEARLINE)\r$(FPurple)%-21s$(FGreen)$(TICKBOX)$(RESET)\n" "Compiling $(PRONAME)"

$(PRONAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(PRONAME)

$(OBJ_DIR)%.o: %.cpp
ifeq ($(shell test -d $(OBJ_DIR) || echo $$?), 1)
	printf "$(CLEARLINE)\r$(Yellow)creting obj dir$(RESET)"
	@mkdir -p $(OBJ_DIR)
endif
	@printf "$(CLEARLINE)\r%-28s$(RESET)" "$(Yellow)Compiling $< ..."
	@$(CC) $(CFLAGS) $(SSLCFLAGS) -c $< -o $@

clean:
	@$(MAKE) -s proname_header
	@printf "%-28s$(RESET)" "$(FRed)Cleaning $(PRONAME)"
	@$(MAKE) -s std_clean
	@printf "$(FGreen)$(TICKBOX)$(RESET)\n"

fclean:
	@$(MAKE) -s proname_header
	@$(MAKE) -s cleanator

re:
	@$(MAKE) -s proname_header
	@$(MAKE) -s cleanator
	@$(MAKE) -s std_all

run: re
	./$(PRONAME)

arch:
	-sudo docker rm -f webserv
	sudo docker-compose -f container/Arch/docker-compose.yml build
	sudo docker-compose -f container/Arch/docker-compose.yml up

ubuntu:
	-sudo docker rm -f webserv
	sudo docker-compose -f container/Ubuntu/docker-compose.yml build
	sudo docker-compose -f container/Ubuntu/docker-compose.yml up

debian:
	-sudo docker rm -f webserv
	sudo docker-compose -f container/Debian/docker-compose.yml build
	sudo docker-compose -f container/Debian/docker-compose.yml up

alpine:
	-sudo docker rm -f webserv
	sudo docker-compose -f ./container/Alpine/docker-compose.yml build
	sudo docker-compose -f ./container/Alpine/docker-compose.yml up

logs:
	sudo docker logs webserv

restart_docker:
	sudo docker restart webserv

install_brew:
	@git clone --depth=1 https://github.com/Homebrew/brew $HOME/goinfre/.brew && echo 'export PATH=$HOME/goinfre/.brew/bin:$PATH' >> $HOME/.zshrc && source $HOME/.zshrc && brew update

install_openssl:
ifeq ($(UNAME), Darwin)
ifeq ($(which brew), $(shell echo "brew not found"))
	$(MAKE) -s install_brew
endif
ifeq ($(which openssl), $(shell echo "openssl not found"))
	brew install openssl
endif
else ifeq ($(UNAME), Linux)
ifeq ($(OS_LIKE), Debian)
	sudo apt-get install openssl
else ifeq ($(OS_LIKE), Alpine)
	sudo apk add openssl
else ifeq ($(OS_LIKE), Arch)
	sudo pacman -S openssl
endif
endif

std_clean:
	@rm -rf $(OBJ_DIR)

cleanator:
	@printf "%-28s$(RESET)" "$(FRed)FCleaning $(PRONAME)"
	@rm -rf $(OBJ_DIR)
	@rm -f $(PRONAME)
	@printf "$(FGreen)$(TICKBOX)$(RESET)\n"

proname_header:
	@printf "$(FYellow)╔══════════════════════╗\n\
$(FYellow)║$(FRed)          (    (      $(FYellow)║$(RESET)\n\
$(FYellow)║$(FRed)     (    )\\ ) )\\ )   $(FYellow)║$(RESET)\n\
$(FYellow)║$(FRed)     )\\  (()/((()/(   $(FYellow)║$(RESET)\n\
$(FYellow)║$(FRed)   (((_)  /(_))/(_))  $(FYellow)║$(RESET)\n\
$(FYellow)║$(FRed)   )\\$(FBlue)___ $(FRed)($(FBlue)_$(FRed))) ($(FBlue)_$(FRed)))    $(FYellow)║$(RESET)\n\
$(FYellow)║$(FRed)  (($(FBlue)/ __|| _ \\| _ \\   $(FYellow)║$(RESET)\n\
$(FYellow)║$(FBlue)   | ($(FBlue)__ |  _/|  _/   $(FYellow)║$(RESET)\n\
$(FYellow)║$(FBlue)    \\___||_|  |_|     $(FYellow)║$(RESET)\n\
$(FYellow)╚══════════════════════╝\n$(RESET)"

.PHONY: all clean fclean re pro_header std_all std_clean cleanator proname_header