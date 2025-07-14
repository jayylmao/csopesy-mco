#pragma once

#include <string>

class IMemoryAllocator {
public:
	virtual void* allocate(size_t size, int pid) = 0;

	virtual void deallocate(void* ptr) = 0;
	virtual std::string displayMemory() = 0;

	virtual size_t getExternalFragmentation() = 0;
	virtual int getProcessCount() = 0;

	virtual ~IMemoryAllocator() = default;
};