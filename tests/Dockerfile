FROM ubuntu:22.04

RUN apt update && apt upgrade

RUN apt install vim nginx curl systemctl -y

#COPY nginx.conf /etc/nginx/nginx.conf

ENTRYPOINT [ "tail", "-f" ]
