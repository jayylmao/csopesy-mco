#pragma once
#include "IMemoryAllocator.h"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <queue>
#include <mutex>
#include <atomic>

// used for internal management.
struct PageTableEntry {
	size_t virtualPage;
	size_t physicalFrame;
	bool present;
};

// used for external visualization.

/**
 * @class DemandPagingAllocator
 * @brief A memory allocator that simulates pages, page tables, faults, and a backing store.
 */
class DemandPagingAllocator : public IMemoryAllocator {
public:
	DemandPagingAllocator(size_t memory, size_t pageSize);
	~DemandPagingAllocator();

	void* allocate(size_t size, int pid) override;
	void deallocate(int pid) override;
	std::string displayMemory() override;
	void accessPage(int pid, size_t virtualPage);

	size_t getPageSize() const;
	int getUsedMemory() const;
	int getPagedIn() const;
	int getPagedOut() const;

	int read(int pid, size_t address);

	void write(int pid, size_t address, int value);

	/**
	 * @brief Get physical memory usage per process.
	 */
	int getProcessMemory(int pid);

private:
	// maps pid to vector of page tables.
	std::unordered_map<int, std::vector<PageTableEntry>> pageTables;
	
	// true if frame is allocated, false otherwise.
	std::vector<bool> frameAllocationMap;

	// tracks the start of a process's page array.
	std::unordered_map<int, size_t> processPageBase;

	// tracks order of frame allocation by index.
	std::queue<size_t> frameQueue;

	// maps frame index to (pid, virtual page).
	std::vector<std::pair<int, size_t>> frameTable;

	// track the next free page base.
	size_t nextFreePageBase = 0;

	// amount of memory available.
	size_t memory;

	// amount of memory per page.
	size_t pageSize;

	// number of frames that the system has.
	size_t frameCount;

	// physical memory.
	char* mainMemory;

	std::mutex allocatorMutex;

	size_t totalAllocatedMemory = 0;
	std::atomic<size_t> numPagedIn{ 0 };
	std::atomic<size_t> numPagedOut{ 0 };

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

	/**
	 * @brief Evict frames when none are available.
	 */
	void pageFaultHandler(int pid, size_t virtualPage);
};