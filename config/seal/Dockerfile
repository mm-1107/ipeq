FROM ubuntu:20.04

# prevents update and install asking for tz
ENV DEBIAN_FRONTEND=noninteractive

# install dependencies
RUN apt-get update && \
    apt-get install -y git build-essential cmake python3 python3-dev python3-pip
#RUN apt update && \
#    apt install -y git build-essential cmake python3 python3-dev python3-pip

# copy into container requirements and install them before rest of code
RUN pip3 install numpy pybind11
RUN pip3 install pandas==1.5.2
RUN pip3 install scikit-learn==0.20.4

WORKDIR /root/
RUN apt install wget checkinstall zlib1g-dev libssl-dev -y
RUN wget https://github.com/Kitware/CMake/releases/download/v3.23.3/cmake-3.23.3.tar.gz
RUN tar -zxvf cmake-3.23.3.tar.gz
RUN rm -rf cmake-3.23.3.tar.gz
WORKDIR /root/cmake-3.23.3
RUN ./bootstrap
RUN make
RUN make install
RUN hash -r

# For gcc/g++ 9
RUN apt-get update && apt-get install software-properties-common -y
RUN add-apt-repository ppa:ubuntu-toolchain-r/test -y
RUN apt-get update && apt-get install g++-9-multilib -y
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 30
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 30

WORKDIR /root/
RUN git clone https://github.com/Huelse/SEAL-Python.git
WORKDIR /root/SEAL-Python
RUN git submodule update --init --recursive
WORKDIR /root/SEAL-Python/SEAL
RUN cmake -S . -B build -DSEAL_USE_MSGSL=OFF -DSEAL_USE_ZLIB=OFF -DSEAL_USE_ZSTD=OFF
RUN cmake --build build
RUN cmake --install build
WORKDIR /root/SEAL-Python/
RUN python3 setup.py build_ext -i
RUN cp seal.*.so examples
