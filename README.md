# **L-Function Calculator (CPP-LFunction)**

A high-performance C++ application for computing and visualizing **L-Functions** using the [FLINT](https://flintlib.org/) (Fast Library for Number Theory) and [Arb](https://arblib.org/) libraries.

This project combines a modern **C++23** backend with a lightweight **HTML/JS frontend**, allowing users to perform complex number-theoretic computations directly from their web browser.

## **✨ Features**

- **Dirichlet L-Functions:** Compute and visualize L(s,χ) on the critical line.


- **Selberg CLT Verification:** Verify the Central Limit Theorem for the distribution of log∣L(1/2+it)∣ across different heights and characters.


- **Fast Computation:** Uses FFT-based algorithms for vectorized character evaluation.


- **Interactive Visualization:** Dynamic Chart.js graphs for real-time data exploration.


## **🚀 Quick Start (Easiest Method)**

*Use this method to run the application instantly without downloading the code or compiling anything.*

### **1. Prerequisites**

- **Docker Desktop**: Download and install it from [docker.com](https://www.docker.com/products/docker-desktop/).

- *Note: Ensure Docker is running (look for the whale icon in your taskbar).*





### **2. Run Command**

Open your terminal (Command Prompt, PowerShell, or Terminal) and run this single command:


```bash
docker run -p 8080:8080 ghcr.io/junghunleephd/cpp-lfunction:latest
```




### **3. Open App**

Once the command is running, open your browser and go to: 👉 **http://localhost:8080**

*(To stop the application, press `*Ctrl+C*` in the terminal)*

## **🏗️ Build from Source (Docker Compose)**

*Use this method if you want to download the repository and build the Docker image on your own machine.*

1. **Download** this repository (Click the green **Code** button -> **Download ZIP**) and unzip it.


1. Open a terminal and navigate to the project folder.


1. Run the build command:


```bash
docker compose up
```





1. Wait for the build to finish (this compiles the math libraries from scratch, which may take a few minutes).


1. Go to **http://localhost:8080**.



## **🛠️ Development (For Developers)**

*Use this method if you want to modify the C++ code or frontend.*

### **Option A: VS Code Dev Container (Recommended)**

This project is configured with a **Dev Container**. This ensures you have the exact C++ compiler (GCC-14), CMake, and math libraries pre-installed without polluting your local machine.

1. Install **VS Code** and the **Dev Containers** extension.


1. Open this folder in VS Code.


1. Click **"Reopen in Container"** when prompted (or run `Dev Containers: Reopen in Container` from the command palette).


1. Once inside, you can build and run using the terminal:


```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
./LFunctionServer
```






### **Option B: Local Manual Build**

If you prefer not to use Docker, you must have the following installed on your host machine:

- **C++ Compiler**: GCC 14+ or Clang 18+ (Must support C++23).


- **CMake**: Version 3.16+.


- **Libraries**: `libflint` (v3+), `libgmp`, `libmpfr`.



```bash
# Ubuntu 24.04 example
sudo apt install gcc-14 g++-14 cmake libflint-dev libgmp-dev libmpfr-dev
```




## **📂 File Structure**


```
CPP-LFunction/
├── .devcontainer/       # Configuration for VS Code Dev Containers
├── include/             # C++ Header files (.hpp)
├── src/                 # C++ Source files (.cpp)
│   ├── main.cpp         # Entry point & Web Server (Crow) configuration
│   ├── l_functions...   # Mathematical implementation logic
│   └── selberg...       # Logic for Selberg CLT sampling
├── static/              # Frontend Assets (HTML, CSS, JS)
│   ├── index.html       # Main visualization interface
│   ├── selberg.html     # Statistical distribution interface
│   └── css/             # Stylesheets
├── Dockerfile           # Multi-stage build for production (Small size)
├── compose.yaml         # Docker Compose file for easy launching
├── CMakeLists.txt       # Build system configuration
└── README.md            # Project documentation
```






## **📄 License**

This project is open-source.

- **FLINT/Arb** libraries are subject to their respective licenses (LGPL/GPL).
