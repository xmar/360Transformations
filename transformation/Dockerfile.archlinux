FROM archimg/base-devel:latest as builder
RUN pacman -q --noconfirm -Syyuu gcc cmake ffmpeg opencv boost hdf5 openmp
WORKDIR /tmp
COPY . /tmp
RUN rm -rf build && mkdir build
WORKDIR /tmp/build
RUN cmake ..
RUN make

FROM archimg/base:latest as worker
RUN pacman -q --noconfirm -Syyuu ffmpeg opencv boost hdf5 openmp
WORKDIR /home/trans360
COPY --from=builder /tmp/build/MainProject/trans /bin
