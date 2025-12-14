# ==========================================
# Stage 1: The Builder (Compiles the code)
# ==========================================
FROM ubuntu:24.04 AS builder

# Prevent interactive prompts during build
ARG DEBIAN_FRONTEND=noninteractive

# 1. Install Build Dependencies
# REMOVED: libarb-dev (It is now part of libflint-dev in Ubuntu 24.04)
RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    gcc-14 g++-14 \
    libflint-dev \
    libasio-dev \
    libgmp-dev \
    libmpfr-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

# 2. Set GCC-14 as default
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100

WORKDIR /app

# 3. Copy Source Code
COPY . .

# 4. Build the Project
RUN mkdir build && cd build && \
    cmake .. && \
    make

# ==========================================
# Stage 2: The Runner (Runs the code)
# ==========================================
FROM ubuntu:24.04

# 1. Install Runtime Dependencies ONLY
# REMOVED: libarb-dev
RUN apt-get update && apt-get install -y \
    libflint-dev \
    libgmp10 \
    libmpfr6 \
    libgomp1 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# 2. Copy the Executable from the Builder Stage
COPY --from=builder /app/build/LFunctionServer ./LFunctionServer

# 3. Copy the Static Files (HTML/CSS/JS)
# Ensure your CMakeLists.txt copies the "static" folder to "build/static"
COPY --from=builder /app/build/static ./static

# 4. Expose the port
EXPOSE 8080

# 5. Run the application
CMD ["./LFunctionServer"]