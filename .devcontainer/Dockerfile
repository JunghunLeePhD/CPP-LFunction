FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04

# 1. Install Dependencies, GCC-14, Clang-Format, and NPM
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    cmake \
    build-essential \
    gcc-14 g++-14 \
    clang-format \
    npm \
    libflint-dev \
    libarb-dev \
    libasio-dev \
    && apt-get clean -y && rm -rf /var/lib/apt/lists/*

# 2. Set GCC-14 as default (Keep this part)
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100

WORKDIR /workspace