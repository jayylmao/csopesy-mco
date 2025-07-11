#include "IMemoryAllocator.h"
#include <vector>
#include <unordered_map>

/**
 * @class FlatMemoryAllocator
 * @brief Allocates memory with a flat model.
 */
class FlatMemoryAllocator : public IMemoryAllocator {
public:
	// reserve actual memory for vector representation of memory and initialize.
	FlatMemoryAllocator(size_t maxSize) : maxSize(maxSize), allocatedSize(0) {
		memory.reserve(maxSize);
		init();
	}

	// clear memory upon object destruction.
	~FlatMemoryAllocator() {
		memory.clear();
	}

	/**
	 * @brief Allocate a block of memory.
	 * @param size Index of block to allocate.
	 */
	void* allocate(size_t size) override;

	/**
	 * @brief Deallocate a block of memory.
	 * @param ptr Pointer to block to deallocate.
	 */
	void deallocate(void* ptr) override;

	/**
	 * @brief Display a visual representation of the memory.
	 */
	std::string displayMemory() override;

private:
	size_t maxSize; // memory size.
	size_t allocatedSize; // amount of memory allocated.
	std::vector<char> memory; // vector representing memory usage.
	std::unordered_map<size_t, bool> allocationMap; // track allocated blocks.

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
	void allocateAt(size_t index, size_t size);

	/**
	 * @brief Deallocate the block at a given index.
	 */
	void deallocateAt(size_t index);
};