FROM philcryer/min-wheezy:latest

ADD . .
RUN apt-get update
RUN apt-get install -y gcc make openssl libssl-dev
RUN make

ENTRYPOINT ./restrictions-crack "$hash" "$salt"
