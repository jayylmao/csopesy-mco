#include "FlatMemoryAllocator.h"

void* FlatMemoryAllocator::allocate(size_t size)
{
	for (size_t i = 0; i < maxSize - size + 1; ++i) {
		if (!allocationMap[i] && canAllocateAt(i, size)) {
			allocateAt(i, size);
			return &memory[i];
		}
	}

	return nullptr;
}

void FlatMemoryAllocator::deallocate(void* ptr)
{
	size_t index = static_cast<char*>(ptr) - &memory[0];
	if (allocationMap[index]) {
		deallocateAt(index);
	}
}

std::string FlatMemoryAllocator::displayMemory()
{
	return std::string(memory.begin(), memory.end());
}

void FlatMemoryAllocator::init()
{
	std::fill(memory.begin(), memory.end(), '.'); // represent unallocated memory with periods.
	
	for (auto& kv : allocationMap) {
		kv.second = false;
	}
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) const
{
	return (index + size <= maxSize);
}

void FlatMemoryAllocator::allocateAt(size_t index, size_t size)
{
	for (size_t i = index; i < index + size; ++i) {
		allocationMap[i] = true;
	}
	
	allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(size_t index)
{
	allocationMap[index] = false;
}