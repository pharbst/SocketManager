# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/15 15:39:29 by pharbst           #+#    #+#              #
#    Updated: 2024/03/15 18:31:07 by pharbst          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

include color.mk

PRONAME		 = libsocketManager.a

INC_DIR		:= 	-I./include/

ifeq ($(UNAME), Darwin)
SUDO		:= 
SSLCFLAGS	:= -I$(shell brew --prefix)/opt/openssl@3/include/
SSLLDFLAGS	:= -L$(shell brew --prefix)/opt/openssl@3/lib/ -lssl -lcrypto
CFLAGS		:= -Wall -Wextra -Werror -MMD -MP -g -std=c++98 $(SSLCFLAGS) $(INC_DIR)
LDFLAGS		:= $(SSLLDFLAGS)
else ifeq ($(UNAME), Linux)
SUDO		:= sudo
CFLAGS		:= -Wall -Wextra -Werror -MMD -MP -g -std=c++98 $(INC_DIR)
LDFLAGS		:= -lssl -lcrypto
endif

CC			 = c++

# add source files with header with the same name
SOURCE		 =	socketManager.cpp \
				Interface.cpp

HEADER		 = $(addprefix $(INC_DIR), $(SOURCE:.cpp=.hpp))

# add other header files here
HEADER		+= socketManagerBase.hpp

# add source files without header with the same name and the file with the main function has to be the first in the list
SRCS		 =	socketManagerTools.cpp \
				socketManagerSSL.cpp \
				socketManagerSEPOLL.cpp \
				InterfaceTools.cpp \
				$(SOURCE)

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
	@$(MAKE) -s proname_header 2> /dev/null
	@$(MAKE) -s std_all 2> /dev/null

install_openssl:
ifeq ($(UNAME), Darwin)
	@$(MAKE) -s install_openssl_mac
else
	@printf "%-40s$(RESET)" "$(FBlue)Installing openssl"
ifeq ($(filter $(OS_LIKE),Debian)$(filter $(OS),Debian),Debian)
	@$(SUDO) apt-get -y install openssl > /dev/null 2>&1
else ifeq ($(filter $(OS_LIKE),Alpine)$(filter $(OS),Alpine),Alpine)
	@$(SUDO) apk add openssl > /dev/null 2>&1
else ifeq ($(filter $(OS_LIKE),Arch)$(filter $(OS),Arch),Arch)
	@$(SUDO) pacman -S --noconfirm openssl > /dev/null 2>&1
endif
	@printf "$(FGreen)$(TICKBOX)$(RESET)\n"
endif


install_openssl_mac:
ifeq ($(shell test -d $(HOME)/.brew || echo $$?), 1)
ifeq ($(shell test -d $(HOME)/goinfre/.brew || echo $$?), 1)
	@printf "%-40s$(RESET)" "$(FCyan)installing brew"
	@rm -rf $(HOME)/.brew && rm -rf $(HOME)/goinfre/.brew && git clone --depth=1 https://github.com/Homebrew/brew $(HOME)/goinfre/.brew && echo 'export PATH=$(HOME)/goinfre/.brew/bin:$$PATH' >> $(HOME)/.zshrc && source $(HOME)/.zshrc && brew update > /dev/null 2>&1
	@printf "$(FGreen)$(TICKBOX)$(RESET)\n"
endif
endif
	@printf "%-40s$(RESET)" "$(FBlue)Installing openssl"
	@brew install openssl >/dev/null 2>&1
	@printf "$(FGreen)$(TICKBOX)$(RESET)\n"

std_all:
	@$(MAKE) -s install_openssl
	@printf "%s$(RESET)\n" "$(FPurple)Compiling $(PRONAME)"
	@-include $(OBJS:.o=.d) 2> /dev/null
	@$(MAKE) -s $(PRONAME)
	@printf "$(SETCURUP)$(CLEARLINE)\r$(FPurple)%-33s$(FGreen)$(TICKBOX)$(RESET)\n" "Compiling $(PRONAME)"

$(PRONAME): $(OBJS)
	@ar rcs $(PRONAME) $(OBJS)

$(OBJ_DIR)%.o: %.cpp
ifeq ($(shell test -d $(OBJ_DIR) || echo $$?), 1)
	printf "$(CLEARLINE)\r$(Yellow)creting obj dir$(RESET)"
	@mkdir -p $(OBJ_DIR)
endif
	@printf "$(CLEARLINE)\r%-40s$(RESET)" "$(Yellow)Compiling $< ..."
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@$(MAKE) -s proname_header
	@printf "%-40s$(RESET)" "$(FRed)Cleaning $(PRONAME)"
	@$(MAKE) -s std_clean
	@printf "$(FGreen)$(TICKBOX)$(RESET)\n"

fclean:
	@$(MAKE) -s proname_header
	@$(MAKE) -s cleanator

re:
	@$(MAKE) -s proname_header 2> /dev/null
	@$(MAKE) -s cleanator 2> /dev/null
	@$(MAKE) -s std_all

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
	@printf "%-40s$(RESET)" "$(FRed)FCleaning $(PRONAME)"
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