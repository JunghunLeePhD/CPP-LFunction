# **CPP-Template**

**CPP-Template** is a GitHub template for C++26 project mainly designed to numerically compute and visualize mathematical objects.

## 📂 Project Structure

```
CPP-Zeta/
├── .devcontainer/         # VS Code DevContainer configuration
│   ├── devcontainer.json
│   └── Dockerfile
├── lib/
├── src/
│   └── main.cpp           # Main application logic
├── output/                # Generated program
├── Makefile               # Build instructions
└── README.md              # Project documentation
```

## **🚀 Getting Started**

You can set up this project locally or use the provided VS Code DevContainer for an instant, pre-configured environment.

### **Option A: VS Code DevContainer (Recommended)**

This project is configured with a Development Container. If you use Visual Studio Code and Docker, you can open this repository in a container that comes pre-installed with **GCC**, **Make**, and **FFmpeg**.

1. Open the project in VS Code.

2. Click **"Reopen in Container"** when the popup appears (or run the command from the palette).

3. The environment will automatically install all necessary tools defined in `.devcontainer/Dockerfile`.

### **Option B: Local Installation**

If you prefer to run directly on your host machine, ensure you have the following prerequisites installed:

- **C++ Compiler:** GCC or Clang supporting C++26.

- **Make:** For building the project.

- **FFmpeg:** Required to convert the generated image frames into video files.


### **Installation**

1. **Clone the repository:**

    ```bash
    git clone https://github.com/JunghunLeePhD/CPP-Template.git
    cd CPP-Template
    ```

## **💻 Usage**

The usage consists of two steps: 

### **1. Run the Simulation**

Execute the following command to compile the project and run the simulation. 

```bash
make run
```

## **🧹 Cleanup**

To remove the generated frames and executable to save space:

```bash
make clean
```

## **🤝 Contributing**

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the Project

2. Create your Feature Branch (`git checkout -b feature/NewAlgorithm`)

3. Commit your Changes (`git commit -m 'Add Odlyzko-Schonhagen method'`)

4. Push to the Branch (`git push origin feature/NewAlgorithm`)

5. Open a Pull Request


## **📄 License**

Distributed under the MIT License. See `LICENSE` for more information.

Created by [*Junghun Lee, PhD*](https://www.google.com/search?q=https://github.com/JunghunLeePhD)
