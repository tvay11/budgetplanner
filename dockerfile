FROM ubuntu:latest

WORKDIR /app

RUN apt-get update -y && \
    apt-get install -y g++ && \
    apt-get install -y sqlite3 libsqlite3-dev

COPY . /app

RUN g++ -o my_program main.cpp -lsqlite3

CMD ["./my_program"]
