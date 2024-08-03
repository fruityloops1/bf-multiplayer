#pragma once

#include <cstddef>

void* buddyMalloc(size_t size);
void buddyFree(void* ptr);
