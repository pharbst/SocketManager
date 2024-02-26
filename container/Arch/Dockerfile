FROM archlinux:latest

RUN pacman -Syu --noconfirm

RUN pacman -S --noconfirm git make valgrind gcc vim nano openssl

COPY ./entrypoint.sh /

ENTRYPOINT [ "/Webserv/container/Arch/entrypoint.sh" ]