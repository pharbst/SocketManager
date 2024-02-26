FROM debian:latest

RUN apt-get -y update

RUN apt install -y git make valgrind gcc vim nano g++ openssl

COPY ./entrypoint.sh /

ENTRYPOINT [ "/entrypoint.sh" ]