FROM ubuntu

ADD . .
RUN apt update
RUN apt install -y gcc make openssl libssl-dev
RUN make
ENTRYPOINT ./restrictions-crack "$hash" "$salt"