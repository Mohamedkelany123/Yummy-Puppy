# Use an official Ubuntu base image
FROM ubuntu:22.04

# Set arguments for non-interactive installations
ARG DEBIAN_FRONTEND=noninteractive

# Install any needed packages (like compilers, debuggers, etc.)
RUN apt-get update && apt-get install -y \
    build-essential \
    gdb \
    cmake \
    g++ \
    git \
    vim \
    curl \
    iputils-ping \
    net-tools  \
    libpq-dev \
    libpoco-dev

# Set the working directory inside the container
WORKDIR /app

# Copy your source files into the container at /app
COPY . .

RUN cmake . -B build && cmake --build build -j12 && cmake --install build
RUN orm_generator generate orm_config.json
RUN cd factory/ostaz && rm -rf build && cmake . -B build && cmake --build build -j12 && cmake --install build
# RUN cd /app/applications/LOS && rm -rf build && cmake . -B build && cmake --build build -j10