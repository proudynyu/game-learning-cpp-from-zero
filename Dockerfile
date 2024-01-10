FROM gcc:4.9

COPY . /usr/src/gamelearning
WORKDIR /usr/src/gamelearning

RUN ./build.sh
CMD ["./gamelearning"]