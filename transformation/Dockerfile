FROM debian:buster as builder
RUN apt-get update && apt-get upgrade -y && apt-get install -y ffmpeg libopencv-dev libboost-all-dev && \
 apt-get install -y build-essential && \
 apt-get install -y cmake libgomp1
WORKDIR /tmp
COPY . /tmp
RUN rm -rf build && mkdir build
WORKDIR /tmp/build
RUN cmake ..
RUN make

FROM debian:buster as worker
RUN apt-get update && apt-get upgrade -y && apt-get install -y ffmpeg libopencv-dev libboost-all-dev
WORKDIR /home/trans360
COPY --from=builder /tmp/build/MainProject/trans /bin
