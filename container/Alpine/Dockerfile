FROM alpine:latest

RUN apk update

RUN apk add git make valgrind gcc vim nano g++ openssl

COPY ./entrypoint.sh /

ENTRYPOINT [ "/entrypoint.sh" ]