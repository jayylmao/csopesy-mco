#include "FlatMemoryAllocator.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <unordered_set>

void* FlatMemoryAllocator::allocate(size_t size, int pid)
{
	for (size_t i = 0; i < maxSize - size + 1; ++i) {
		if (canAllocateAt(i, size)) {
			bool free = true;
			for (size_t j = i; j < i + size; ++j) {
				if (allocationMap[j]) {
					free = false;
					break;
				}
			}
			if (free) {
				allocateAt(i, size, pid);
				return &memory[i];
			}
		}
	}
	return nullptr;
}

void FlatMemoryAllocator::deallocate(int pid)
{
	auto it = std::find_if(blocks.begin(), blocks.end(), [pid](const MemoryBlock& block) {
		return block.pid == pid;
	});

	if (it != blocks.end()) {
		for (size_t i = it->start; i < it->start + it->size; ++i) {
			allocationMap[i] = false;
		}
		allocatedSize -= it->size;
		blocks.erase(it);
	}
}

std::string FlatMemoryAllocator::displayMemory()
{
	std::ostringstream oss;
	for (const auto& block : blocks) {
		oss << "[0x"
			<< std::uppercase << std::hex << std::setw(4) << std::setfill('0') << block.start
			<< " - 0x"
			<< std::setw(4) << std::setfill('0') << (block.start + block.size - 1)
			<< "] -> PID " << std::dec << block.pid << "\n";
	}
	return oss.str();
}

void FlatMemoryAllocator::init()
{
	std::fill(memory.begin(), memory.end(), '.'); // represent unallocated memory with periods.
	std::fill(allocationMap.begin(), allocationMap.end(), false);
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) const
{
	return (index + size <= maxSize);
}

void FlatMemoryAllocator::allocateAt(size_t index, size_t size, int pid)
{
	for (size_t i = index; i < index + size; ++i) {
		allocationMap[i] = true;
	}
	blocks.push_back({ index, size, pid });
	allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(size_t index)
{
	for (auto it = blocks.begin(); it != blocks.end(); ++it) {
		if (it->start == index) {
			for (size_t i = index; i < index + it->size; ++i) {
				allocationMap[i] = false;
			}
			allocatedSize -= it->size;
			blocks.erase(it);
			break;
		}
	}
}

size_t FlatMemoryAllocator::getExternalFragmentation() {
	size_t fragmentation = 0;
	size_t i = 0;

	while (i < maxSize) {
		if (!allocationMap[i]) {
			size_t start = i;
			while (i < maxSize && !allocationMap[i]) ++i;
			size_t freeBlockSize = i - start;

			if (freeBlockSize < 4096) {
				fragmentation += freeBlockSize;
			}
		}
		else {
			++i;
		}
	}

	return fragmentation;
}

int FlatMemoryAllocator::getProcessCount() {
	std::unordered_set<int> uniquePIDs;
	for (const auto& block : blocks) {
		uniquePIDs.insert(block.pid);
	}
	return static_cast<int>(uniquePIDs.size());
}

std::vector<MemoryBlock> FlatMemoryAllocator::getBlocks() const {
	return blocks;
}

size_t FlatMemoryAllocator::getMaxSize() const {
	return maxSize;
}