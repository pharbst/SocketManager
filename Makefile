# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/15 15:39:29 by pharbst           #+#    #+#              #
#    Updated: 2024/03/25 14:42:56 by pharbst          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

include color.mk

PRONAME		 = libsocketManager.a

INC_DIR		:= 	-I./include/

ifeq ($(UNAME), Darwin)
SUDO		:= 
CFLAGS		:= -Wall -Wextra -Werror -MMD -MP -g -std=c++98 $(INC_DIR)
else ifeq ($(UNAME), Linux)
SUDO		:= sudo
CFLAGS		:= -Wall -Wextra -Werror -MMD -MP -g -std=c++98 $(INC_DIR)
endif

CC			 = c++

# add source files with header with the same name
# SOURCE		 =	socketManager.cpp \
# 				Interface.cpp

# HEADER		 = $(addprefix $(INC_DIR), $(SOURCE:.cpp=.hpp))

# add other header files here
HEADER		 =	socketManagerBase.hpp \
				socketManager.hpp

# add source files without header with the same name and the file with the main function has to be the first in the list
SRCS		 =	socketManager.cpp \
				socketManagerTools.cpp \
				socketManagerSSL.cpp \
				socketManagerSEPOLL.cpp \

OBJ_DIR		 = ./obj/
ifeq ($(UNAME), Darwin)
OBJ_DIR 	 = ./obj/mac/
else ifeq ($(UNAME), Linux)
OBJ_DIR 	 = ./obj/linux/
endif

OBJS		 = $(addprefix $(OBJ_DIR), $(SRCS:.cpp=.o))


# in case of subdirectories in the src folder add them here
VPATH		:= src

all:
	@$(MAKE) -s proname_header
	@$(MAKE) -j6 -s std_all

std_all:
ifeq ($(shell test -f libsocketManager.a && echo $$?), 0)
ifeq ($(shell nm -C libsocketManager.a 2>/dev/null | grep epoll >/dev/null 2>&1 && echo $$? ), 0)
ifeq ($(UNAME), Darwin)
	@$(PRINT) "$(Yellow)lib compiled for linux$(RESET)\n"
	@rm libsocketManager.a
endif
else
ifeq ($(UNAME), Linux)
	@$(PRINT) "$(Yellow)lib compiled for macos$(RESET)\n"
	@rm libsocketManager.a
endif
endif
endif
	@$(MAKE) -s install_openssl
	@$(PRINT) "$(FPurple)%-33s\n$(RESET)" "Compiling $(PRONAME)"
	@-include $(OBJS:.o=.d) 2> /dev/null
	@$(MAKE) -s $(PRONAME)
	@$(PRINT) "\n$(SETCURUP)$(SETCURUP)$(CLEARLINE)$(FPurple)%-33s$(FGreen)$(TICKBOX)\n$(RESET)$(CLEARLINE)" "Compiling $(PRONAME)"

install_openssl:
ifeq ($(UNAME), Darwin)
	@$(MAKE) -s install_openssl_mac
else
	@$(PRINT) "%-40s$(RESET)" "$(FBlue)Installing openssl"
ifeq ($(call GET_OS,Debian), Debian)
ifeq ($(shell dpkg -l | grep -c openssl), 0)
	@$(SUDO) apt-get -y install openssl >/dev/null 2>&1
endif
else ifeq ($(filter $(OS_LIKE),Alpine)$(filter $(OS),Alpine),Alpine)
ifeq ($(shell apk list | grep -c openssl), 0)
	@$(SUDO) apk add openssl >/dev/null 2>&1
endif
else ifeq ($(filter $(OS_LIKE),Arch)$(filter $(OS),Arch),Arch)
ifeq ($(shell pacman -Q | grep -c openssl), 0)
	@$(SUDO) pacman -S --noconfirm openssl >/dev/null 2>&1
endif
endif
	@$(PRINT) "$(FGreen)$(TICKBOX)$(RESET)\n"
endif

install_openssl_mac:
	@$(PRINT) "%-40s$(RESET)" "$(FCyan)Installing brew"
ifeq ($(shell test -d $(HOME)/.brew || echo $$?), 1)
ifeq ($(shell test -d $(HOME)/goinfre/.brew || echo $$?), 1)
	@rm -rf $(HOME)/.brew  > /dev/null 2>&1 && rm -rf $(HOME)/goinfre/.brew  > /dev/null 2>&1 && git clone --depth=1 https://github.com/Homebrew/brew $(HOME)/goinfre/.brew  > /dev/null 2>&1 && echo 'export PATH=$(HOME)/goinfre/.brew/bin:$$PATH' >> $(HOME)/.zshrc  > /dev/null 2>&1 && source $(HOME)/.zshrc  > /dev/null 2>&1 && brew update > /dev/null 2>&1
endif
	@$(PRINT) "$(FGreen)$(TICKBOX)$(RESET)\n"
endif
	@$(PRINT) "%-40s$(RESET)" "$(FBlue)Installing openssl"
ifeq ($(shell brew list 2>/dev/null | grep -c openssl), 0)
	@brew install openssl >/dev/null 2>&1
endif
	@$(PRINT) "$(FGreen)$(TICKBOX)$(RESET)\n"

$(PRONAME): $(OBJS)
	@ar rcs $(PRONAME) $(OBJS)

$(OBJ_DIR)%.o: %.cpp
ifeq ($(shell test -d $(OBJ_DIR) || echo $$?), 1)
	$(PRINT) "$(CLEARLINE)\r$(Yellow)creting obj dir$(RESET)"
	@mkdir -p $(OBJ_DIR)
endif
	@$(PRINT) "$(CLEARLINE)\r%-40s$(RESET)" "$(Yellow)Compiling $< ..."
ifeq ($(UNAME), Darwin)
	@$(CC) $(CFLAGS) -I$(shell brew --prefix)/opt/openssl@3/include/ -c $< -o $@
else
	@$(CC) $(CFLAGS) -c $< -o $@
endif

clean:
	@$(MAKE) -s proname_header
	@$(PRINT) "%-40s$(RESET)" "$(FRed)Cleaning $(PRONAME)"
	@$(MAKE) -s std_clean
	@$(PRINT) "$(FGreen)$(TICKBOX)$(RESET)\n"

fclean:
	@$(MAKE) -s proname_header
	@$(MAKE) -s cleanator

re:
	@$(MAKE) -s proname_header
	@$(MAKE) -s cleanator
	@$(MAKE) -j6 -s std_all

run: re
	./$(PRONAME)

arch:
	-$(SUDO) docker rm -f webserv
	$(SUDO) docker-compose -f container/Arch/docker-compose.yml build
	$(SUDO) docker-compose -f container/Arch/docker-compose.yml up

ubuntu:
	-$(SUDO) docker rm -f webserv
	$(SUDO) docker-compose -f container/Ubuntu/docker-compose.yml build
	$(SUDO) docker-compose -f container/Ubuntu/docker-compose.yml up

debian:
	-$(SUDO) docker rm -f webserv
	$(SUDO) docker-compose -f container/Debian/docker-compose.yml build
	$(SUDO) docker-compose -f container/Debian/docker-compose.yml up

alpine:
	-$(SUDO) docker rm -f webserv
	$(SUDO) docker-compose -f ./container/Alpine/docker-compose.yml build
	$(SUDO) docker-compose -f ./container/Alpine/docker-compose.yml up

logs:
	$(SUDO) docker logs webserv

restart_docker:
	$(SUDO) docker restart webserv

std_clean:
	@rm -rf $(OBJ_DIR)

cleanator:
	@$(PRINT) "%-40s$(RESET)" "$(FRed)FCleaning $(PRONAME)"
	@rm -rf $(OBJ_DIR)
	@rm -f $(PRONAME)
	@$(PRINT) "$(FGreen)$(TICKBOX)$(RESET)\n"

proname_header:
	@$(PRINT) "$(FYellow)╔══════════════════════╗\n\
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