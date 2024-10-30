# Use an official Debian base image
FROM debian:11

# Set arguments for non-interactive installations
ARG DEBIAN_FRONTEND=noninteractive

# Install required packages (excluding cmake)
RUN apt-get update && apt-get install -y \
    build-essential \
    gdb \
    g++ \
    git \
    vim \
    curl \
    iputils-ping \
    net-tools  \
    libpq-dev \
    libpoco-dev \
    sudo \
    libc6-dev \
    libcurl4-openssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Download and install CMake manually
RUN CMAKE_VERSION=3.22.0 \
    && curl -L https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz -o cmake.tar.gz \
    && tar -zxvf cmake.tar.gz --strip-components=1 -C /usr/local \
    && rm cmake.tar.gz

ARG USER_ID=1000
ARG GROUP_ID=1000

RUN useradd -u $USER_ID -g $GROUP_ID -ms /bin/bash dev && \
    echo 'dev ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# Switch to user dev
USER dev

WORKDIR /home/dev

CMD ["/bin/bash"]



# docker build --build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) -t <IMAGE_NAME> .
# docker run -it --network host -v $(pwd):/home/dev/ORM-C_PLUS_PLUS <IMAGE_NAME>



# Image creation
# docker build -t <IMAGE_NAME> .

# Container spin up from image
# docker run -it --network host -v $(pwd):/home/dev/ORM-C_PLUS_PLUS <IMAGE_NAME>


# Build commands

# cmake . -B build && cmake --build build -j12 && sudo cmake --install build
# orm_generator generate orm_config.json
# cd factory/ostaz && rm -rf build && cmake . -B build && cmake --build build -j12 && sudo cmake --install build
# cd /home/dev/ORM-C_PLUS_PLUS/applications/LOS && rm -rf build && cmake . -B build && cmake --build build -j10
# c++_app_server config_docker.json 