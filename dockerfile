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
    libboost-all-dev \
    libpq-dev 

# Set the working directory inside the container
WORKDIR /app

# Copy your source files into the container at /app
COPY . .

# Keep the container running to allow terminal access
CMD ["tail", "-f", "/dev/null"]
