
.PHONY: all clean

BUILD_COUNTER_FILE := .build_counter

all: increment_counter
	cmake --toolchain=cmake/toolchain.cmake -S . -B build && $(MAKE) -C build

clean:
	rm -r build || true#

increment_counter:
	@if [ ! -f $(BUILD_COUNTER_FILE) ]; then echo 0 > $(BUILD_COUNTER_FILE); fi
	@echo $$(($$(cat $(BUILD_COUNTER_FILE)) + 1)) > $(BUILD_COUNTER_FILE)
	@echo "#pragma once" > include/pe/Util/BuildId.h
	@echo "#define BUILD_ID $$(cat $(BUILD_COUNTER_FILE))" >> include/pe/Util/BuildId.h
	
