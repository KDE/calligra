PYTHON=$(PYTHON_BIN)
#PYTHON="python2.3"
SETUPFLAGS=

# What should the default be?
#all: build
all:

# Build in-place.
inplace:
	$(PYTHON) setup.py $(SETUPFLAGS) build_ext -i

# Build and move to ./build/ directory.
build:
	$(PYTHON) setup.py $(SETUPFLAGS) build

# Install in the global package path.
install:
	$(PYTHON) setup.py $(SETUPFLAGS) install

# Removed temp files.
clean:
	find . \( -name '*.o' -o -name '*.so' -o -name '*.py[co]' -o -name '*.dll' \) -exec rm -f {} \;
	rm -rf build

METASOURCES = AUTO
