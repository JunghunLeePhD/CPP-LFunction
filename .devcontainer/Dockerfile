# 1. Use the official Ubuntu 24.04 Dev Container base
#    This version NATIVELY supports GCC 14.
FROM mcr.microsoft.com/devcontainers/base:ubuntu-24.04

# 2. Install GCC 14, Make, and your libraries directly
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    cmake \
    build-essential \
    gcc-14 g++-14 \
    libflint-dev \
    libarb-dev \
    libasio-dev \
    && apt-get clean -y && rm -rf /var/lib/apt/lists/*

# 3. Set GCC-14 as the default compiler
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100

# 4. Set the working directory
WORKDIR /workspace