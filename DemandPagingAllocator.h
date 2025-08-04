#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>

struct PageTableEntry {
	size_t virtualPage;
	size_t physicalFrame;
	bool present;
	bool dirty; // track if page has been used.
};

/**
 * @class DemandPagingAllocator
 * @brief A memory allocator that simulates pages, page tables, faults, and a backing store.
 */
class DemandPagingAllocator : public IMemoryAllocator {
public:
	DemandPagingAllocator(size_t memory, size_t pageSize);
	void* allocate(size_t size, int pid) override;
	void deallocate(int pid) override;

private:
	// maps pid to vector of page tables.
	std::unordered_map<int, std::vector<PageTableEntry>> pageTables;
	
	// true if frame is allocated, false otherwise.
	std::vector<bool> frameAllocationMap;

	// track the start of a process's page array.
	std::unordered_map<int, size_t> processPageBase;

	// track the next free page base.
	size_t nextFreePageBase = 0;

	// amount of memory available.
	size_t memory;

	// amount of memory per page.
	size_t pageSize;

	// number of frames that the system has.
	size_t frameCount;

	/**
	 * @brief Find a free frame when loading a page into memory.
	 * @return Index of free frame in memory, or -1 if no frames are available.
	 */
	size_t findFreeFrame();

	size_t calculatePageOffset(int pid, size_t virtualPage);

	/**
	 * @brief Read a page from the backing store.
	 */
	void pageIn(int pid, size_t virtualPage, char* buffer);

	/**
	 * @brief Write a page to the backing store.
	 */
	void pageOut(int pid, size_t virtualPage, const char* buffer);
};