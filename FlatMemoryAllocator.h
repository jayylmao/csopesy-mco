#pragma once
#include "IMemoryAllocator.h"
#include <vector>
#include <unordered_map>
#include <iostream>


struct MemoryBlock {
	size_t start;
	size_t size;
	int pid;
};

/**
 * @class FlatMemoryAllocator
 * @brief Allocates memory with a flat model.
 */
class FlatMemoryAllocator : public IMemoryAllocator {
public:
	// reserve actual memory for vector representation of memory and initialize.
	FlatMemoryAllocator(size_t maxSize)
		: maxSize(maxSize), allocatedSize(0), memory(maxSize), allocationMap(maxSize, false) {
		init();
	}

	// clear memory upon object destruction.
	~FlatMemoryAllocator() {
		memory.clear();
		blocks.clear();
		allocationMap.clear();
	}

	/**
	 * @brief Allocate a block of memory.
	 * @param size Index of block to allocate.
	 */
	void* allocate(size_t size, int pid) override;

	/**
	 * @brief Deallocate a block of memory.
	 * @param ptr Pointer to block to deallocate.
	 */
	void deallocate(void* ptr) override;

	/**
	 * @brief Display a visual representation of the memory.
	 */
	std::string displayMemory() override;
	size_t getExternalFragmentation() override;
	int getProcessCount() override;
	std::vector<MemoryBlock> getBlocks() const;
	size_t getMaxSize() const;

private:


	size_t maxSize; // memory size.
	size_t allocatedSize; // amount of memory allocated.
	std::vector<char> memory; // vector representing memory usage.
	std::vector<bool> allocationMap; // track allocated blocks.


	std::vector<MemoryBlock> blocks;


	/**
	 * @brief Fill the memory with unallocated blocks.
	 */
	void init();

	/**
	 * @brief Check the memory to see if a block can be allocated.
	 */
	bool canAllocateAt(size_t index, size_t size) const;

	/**
	 * @brief Allocate the block at a given index given a certain memory size.
	 */
	void allocateAt(size_t index, size_t size, int pId);

	/**
	 * @brief Deallocate the block at a given index.
	 */
	void deallocateAt(size_t index);


};
