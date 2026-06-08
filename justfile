# Default
default:
	just build

# Setup build directory
setup:
	CC=clang meson setup build --reconfigure

# Build project
build:
  if [ ! -d ./build ]; then
    just setup 
  fi
	meson compile -C build

# Run binary
run:
	./build/note

# Full rebuild (clean setup + build)
rebuild:
	just clean 
	just build

# Clean build directory
clean:
	rm -rf build
