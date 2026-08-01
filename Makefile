CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
TARGET := void
BINDIR := .

# Include libutils if present
LIB_UTILS_DIR := libutils

# Detect sources: root .cpp files + libutils/*.cpp (if any)
ROOT_SRCS := $(wildcard *.cpp)
LIB_SRCS := $(wildcard $(LIB_UTILS_DIR)/*.cpp)
SRCS := $(ROOT_SRCS) $(LIB_SRCS)

OBJS := $(patsubst %.cpp,%.o,$(SRCS))

# If libutils exists, add its include dir
ifeq ($(wildcard $(LIB_UTILS_DIR)),)
  INCLUDES :=
else
  INCLUDES := -I$(LIB_UTILS_DIR)
endif

.PHONY: all debug clean run

all: $(BINDIR)/$(TARGET)

# debug target with symbols and no optimizations
debug: CXXFLAGS += -g -O0 -DDEBUG
debug: clean all

# Link step
$(BINDIR)/$(TARGET): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(OBJS)
	@echo "Built -> $@"

# Generic compile rule
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: all
	@./$(BINDIR)/$(TARGET)

clean:
	-@rm -f $(OBJS)
	-@rm -f $(BINDIR)/$(TARGET)
	@echo "Cleaned up the ashes."

# Prevent make from trying to build files named 'all', etc.
.SUFFIXES:
