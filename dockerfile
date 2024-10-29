# Use an official Ubuntu base image
FROM debian:11

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
    libpoco-dev \
    sudo \
    libcurl4-openssl-dev \
    # clean up to reduce image size
    && rm -rf /var/lib/apt/lists/*

ARG DEBIAN_FRONTEND=dialog

RUN useradd -ms /bin/bash dev && \
    echo 'dev ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# go to user dev
USER dev

WORKDIR /home/dev

# COPY . /ORM-C_PLUS_PLUS

CMD ["/bin/bash"]



#TO CREATE THE IMAGE
#docker build -t <IMAGE_NAME> .

# docker run -it --network host -v $(pwd):/home/dev/ORM-C_PLUS_PLUS <IMAGE_NAME>


# RUN cmake . -B build && cmake --build build -j12 && cmake --install build
# RUN orm_generator generate orm_config.json
# RUN cd factory/ostaz && rm -rf build && cmake . -B build && cmake --build build -j12 && cmake --install build
# # RUN cd /app/applications/LOS && rm -rf build && cmake . -B build && cmake --build build -j10