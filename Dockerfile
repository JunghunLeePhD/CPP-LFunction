# ==========================================
# Stage 1: The Builder (Compiles the code)
# ==========================================
FROM ubuntu:24.04 AS builder

# Prevent interactive prompts during build
ARG DEBIAN_FRONTEND=noninteractive

# 1. Install Build Dependencies (Matching your Dev Container)
# We need cmake, gcc-14, and the math libraries headers
RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    gcc-14 g++-14 \
    libflint-dev \
    libarb-dev \
    libasio-dev \
    libgmp-dev \
    libmpfr-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

# 2. Set GCC-14 as default (Crucial for C++23 support)
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100

WORKDIR /app

# 3. Copy Source Code
# We copy CMakeLists first to cache dependencies if you had them separated, 
# but copying everything is fine for this size.
COPY . .

# 4. Build the Project
# We run CMake configuration and then Make
RUN mkdir build && cd build && \
    cmake .. && \
    make

# ==========================================
# Stage 2: The Runner (Runs the code)
# ==========================================
FROM ubuntu:24.04

# 1. Install Runtime Dependencies ONLY
# We don't need gcc, cmake, or git here. Just the libraries the app links to.
RUN apt-get update && apt-get install -y \
    libflint-dev \
    libarb-dev \
    libgmp10 \
    libmpfr6 \
    libgomp1 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# 2. Copy the Executable from the Builder Stage
COPY --from=builder /app/build/LFunctionServer ./LFunctionServer

# 3. Copy the Static Files (HTML/CSS/JS)
# Your CMake copied them to the build folder, so we grab them from there.
# Crow expects the 'static' folder to be relative to the executable.
COPY --from=builder /app/build/static ./static

# 4. Expose the port (Crow usually defaults to 18080 or 8080)
EXPOSE 8080

# 5. Run the application
CMD ["./LFunctionServer"]