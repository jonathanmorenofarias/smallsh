FROM gcc:latest

WORKDIR /app

COPY smallsh.c /app/

RUN gcc -o smallsh smallsh.c

CMD ["./smallsh"]