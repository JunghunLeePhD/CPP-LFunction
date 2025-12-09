CXX      = g++
CXXFLAGS = -O3 -std=c++26 -I./lib

APP_SRC  = src/main.cpp

# LIB_SRCS = lib/*.cpp \

SRCS = $(APP_SRC) $(LIB_SRCS)

OUT_DIR = output
TARGET  = $(OUT_DIR)/run_app

all: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p $(OUT_DIR)
	@echo "Compiling Project..."
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)
	@echo "Done! Executable: $(TARGET)"

run: all
	@echo "Running..."
	./$(TARGET)

clean:
	rm -rf $(OUT_DIR)

.PHONY: all clean run