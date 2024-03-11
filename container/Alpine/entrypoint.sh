#!/bin/sh
echo "Starting Webserv"
cd /Webserv
find /usr -name "ssl.h" -type f
find /usr -name "err.h" -type f
make re
./Webserv_linux