FROM alpine

ADD . .
RUN apk update && apk add gcc make openssl libressl-dev musl-dev && make

ENTRYPOINT ./restrictions-crack "$hash" "$salt"
