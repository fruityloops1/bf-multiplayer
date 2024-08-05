FROM rikorose/gcc-cmake:latest AS builder

WORKDIR /app

COPY . /app

RUN cd server; \
    cmake .; \
    make

CMD ./server/peepa-server
