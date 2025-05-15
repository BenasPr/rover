FROM ubuntu:22.04

# Set environment variables to prevent interactive prompts during package installation
ARG DEBIAN_FRONTEND=noninteractive

# Update package list and install necessary tools and dependencies
RUN apt-get update && \
    apt-get install -y \
    sudo \
    nano \
    git \
    curl \
    gnupg \
    make \
    software-properties-common \
    build-essential \
    pkg-config \
    libzmq3-dev \
    protobuf-c-compiler \
    libprotobuf-c-dev \
    libcjson-dev \
    cmake


RUN echo 'root:dev' | chpasswd
RUN useradd -ms /bin/bash dev && echo 'dev:dev' | chpasswd && adduser dev sudo
RUN echo 'dev ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

COPY --chown=dev:dev .devcontainer/scripts/gu /usr/local/bin/gu
COPY --chown=dev:dev .devcontainer/scripts/.bashrc /home/dev/.bashrc

# Install two external dependencies: c-hashtable and c-list
WORKDIR /opt/
RUN git clone https://github.com/joelguittet/c-list.git
RUN cd /opt/c-list && mkdir build && cd build && cmake .. && make install

RUN git clone https://github.com/joelguittet/c-hashtable.git
RUN cd /opt/c-hashtable && mkdir build && cd build && cmake .. && make install

USER dev
