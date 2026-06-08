# Default
default:
	just build

# Setup build directory
setup:
	CC=clang meson setup build --reconfigure

# Build project
build:
  [ -d ./build ] || just setup 
  meson compile -C build

# Full rebuild (clean setup + build)
rebuild:
  just clean 
  just build

# Clean build directory
clean:
  rm -rf build
