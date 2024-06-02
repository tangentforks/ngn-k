FROM gcc:latest

COPY . /usr/src/k
WORKDIR /usr/src/k

RUN echo -std=gnu99 >> /usr/src/k/opts && make c && make k libk.so

CMD ["./k"]