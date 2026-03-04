# Makefile

all: compile

# Prepares the build directory
build:
	mkdir build
	cd build; \
		cmake ..

# Builds the targets
compile: build
	cd build; \
		make

# And install
install: compile
	cd build; \
		sudo make install

clean:
	rm -rf build