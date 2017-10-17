FROM alpine

ADD . .
RUN apk update
RUN apk add gcc make openssl libressl-dev musl-dev
RUN make

ENTRYPOINT ./restrictions-crack "$hash" "$salt"
