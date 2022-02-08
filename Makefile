all: rebuild

bsh: build

.PHONY: build

check: build
	@echo "Checking..."
	@python3 tests/moulinette.py --binary bsh --tests tests/*.yml 
	@echo "Done."

rebuild:
	@echo "Rebuilding project..."
	@rm -f bsh
	@ninja -C builddir
	@cp builddir/bsh .
	@echo "Done."

build: clean
	@echo "Building project..."
	@rm -f bsh
	@meson setup builddir
	@ninja -C builddir
	@cp builddir/bsh .
	@echo "Done."

doc:
	@echo "Generating documentation..."
	@rm -rf doc
	@doxygen
	@echo "Done."

clean:
	@echo "Cleaning up..."
	@rm -rf builddir bsh doc vgcore.*
	@echo "Done."
