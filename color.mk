# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    color.mk                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/03 18:16:49 by pharbst           #+#    #+#              #
#    Updated: 2024/03/23 19:54:59 by pharbst          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
# How to use this snippet:                                                     #
# **************************************************************************** #

#		to check the operating system use UNAME with that you can check if you are on a Darwin (MacOs) system or Linux system.
#		OS contains the distribution name (for example Manjaro Linux) so OS contains "Manjaro" Linux is trimmed away
#		OS_LIKE contains the distribution name if it is based on another distribution (for example Manajaro is based on arch linux) so OS_LIKE contains "arch"
#		for easier use in ifeq statements GET_OS is used to check OS and OS_LIKE at the same time so when you are looking for all arch based systems you call $(call GET_OS, arch) in an if statement like this:
#		ifeq ($(call GET_OS,arch), arch)
#		so the if statement is true if the system is arch or an arch based system like Manjaro
#		for Debian based systems:
#		ifeq ($(call GET_OS,Debian), Debian)
#		so the if statement is true if the system is Debian or an Debian based system like Ubuntu


# **************************************************************************** #
# Operating System tracking                                                    #
# **************************************************************************** #
UNAME		=	$(shell uname)

# /etc/os-release is a file that contains information about the operating system but only exists on Linux systems so the variables only exist on Linux systems on Mac they are just empty and when checking them against arch Denian etc its always false
ifeq ($(UNAME), Linux)
OS			=	$(shell cat /etc/os-release | grep '^NAME=' | cut -d'=' -f2- | tr -d '"' | sed 's/ Linux//')
OS_LIKE		=	$(shell cat /etc/os-release | grep ID_LIKE | cut -d= -f2)
GET_OS		=	$(filter $1, $(OS) $(OS_LIKE))
endif


# ****************************************************************************  #
# Colors and Printing stuff 												    #
# ****************************************************************************  #
ifeq ($(UNAME), Darwin)
PRINT	= printf
Black			=	$(shell echo "\033[0;30m")
FBlack			=	$(shell echo "\033[1;30m")
Red				=	$(shell echo "\033[0;31m")
FRed			=	$(shell echo "\033[1;31m")
Green			=	$(shell echo "\033[0;32m")
FGreen			=	$(shell echo "\033[1;32m")
FYellow			=	$(shell echo "\033[1;33m")
Yellow			=	$(shell echo "\033[0;33m")
Blue			=	$(shell echo "\033[0;34m")
FBlue			=	$(shell echo "\033[1;34m")
Purple			=	$(shell echo "\033[0;35m")
FPurple			=	$(shell echo "\033[1;35m")
Cyan			=	$(shell echo "\033[0;36m")
FCyan			=	$(shell echo "\033[1;36m")
FWhite			=	$(shell echo "\033[1;37m")
White			=	$(shell echo "\033[0;37m")
RESET			=	$(shell echo "\033[0m")

TICK			=	$(shell echo "\xE2\x9C\x94")
TICKBOX			=	$(shell echo "[$(TICK)]")
SCHMILI			=	$(shell echo "☺")

SETCURSTART		=	$(shell echo "\r")
SETCURUP		=	$(shell echo "\033[1A")
SETCURDOWN		=	$(shell echo "\033[1B")
CLEARLINE		=	$(shell echo "\033[2K")
else
	ifeq ($(filter Debian, $(OS), $(OS_LIKE)), Debian)
PRINT = echo
Black			=	$(shell echo "\033[0;30m")
FBlack			=	$(shell echo "\033[1;30m")
Red				=	$(shell echo "\033[0;31m")
FRed			=	$(shell echo "\033[1;31m")
Green			=	$(shell echo "\033[0;32m")
FGreen			=	$(shell echo "\033[1;32m")
FYellow			=	$(shell echo "\033[1;33m")
Yellow			=	$(shell echo "\033[0;33m")
Blue			=	$(shell echo "\033[0;34m")
FBlue			=	$(shell echo "\033[1;34m")
Purple			=	$(shell echo "\033[0;35m")
FPurple			=	$(shell echo "\033[1;35m")
Cyan			=	$(shell echo "\033[0;36m")
FCyan			=	$(shell echo "\033[1;36m")
FWhite			=	$(shell echo "\033[1;37m")
White			=	$(shell echo "\033[0;37m")
RESET			=	$(shell echo "\033[0m")

TICK			=	$(shell echo "\xE2\x9C\x94")
TICKBOX			=	$(shell echo "[$(TICK)]")
SCHMILI			=	$(shell echo "☺")

SETCURSTART		=	$(shell echo "\r")
SETCURUP		=	$(shell echo "\033[1A")
SETCURDOWN		=	$(shell echo "\033[1B")
CLEARLINE		=	$(shell echo "\033[2K")
	else
PRINT = printf
Black			=	$(shell echo -e "\033[0;30m")
FBlack			=	$(shell echo -e "\033[1;30m")
Red				=	$(shell echo -e "\033[0;31m")
FRed			=	$(shell echo -e "\033[1;31m")
Green			=	$(shell echo -e "\033[0;32m")
FGreen			=	$(shell echo -e "\033[1;32m")
FYellow			=	$(shell echo -e "\033[1;33m")
Yellow			=	$(shell echo -e "\033[0;33m")
Blue			=	$(shell echo -e "\033[0;34m")
FBlue			=	$(shell echo -e "\033[1;34m")
Purple			=	$(shell echo -e "\033[0;35m")
FPurple			=	$(shell echo -e "\033[1;35m")
Cyan			=	$(shell echo -e "\033[0;36m")
FCyan			=	$(shell echo -e "\033[1;36m")
FWhite			=	$(shell echo -e "\033[1;37m")
White			=	$(shell echo -e "\033[0;37m")
RESET			=	$(shell echo -e "\033[0m")

TICK			=	$(shell echo -e "\xE2\x9C\x94")
TICKBOX			=	$(shell echo -e "[\xE2\x9C\x94]")
SCHMILI			=	$(shell echo -e "☺")

SETCURSTART		=	$(shell echo -e "\r")
SETCURUP		=	$(shell echo -e "\033[1A")
SETCURDOWN		=	$(shell echo -e "\033[1B")
CLEARLINE		=	$(shell echo -e "\033[2K")
	endif
endif