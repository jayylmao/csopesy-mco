#include "DemandPagingAllocator.h"
#include <iostream>
#include <fstream>

DemandPagingAllocator::DemandPagingAllocator(size_t memory, size_t pageSize)
    : memory(memory), pageSize(pageSize)
{
    frameCount = memory / pageSize; // calculate number of frames.
    frameAllocationMap.resize(frameCount, false); // initialize all frames as unallocated.
}

void* DemandPagingAllocator::allocate(size_t size, int pid)
{
    // shift free page base tracker over by the number of pages allocated.
    size_t numPages = (size + pageSize - 1) / pageSize;
    processPageBase[pid] = nextFreePageBase;
    nextFreePageBase += numPages;

    std::cout << "Allocating " << size << " bytes for PID " << pid << std::endl;
    return nullptr;
}

void DemandPagingAllocator::deallocate(int pid)
{

}

void DemandPagingAllocator::pageFaultHandler()
{

}

size_t DemandPagingAllocator::findFreeFrame()
{
    for (size_t i = 0; i < frameAllocationMap.size(); ++i) {
        if (!frameAllocationMap[i]) return i;
    }

    return -1;
}

size_t DemandPagingAllocator::calculatePageOffset(int pid, size_t virtualPage)
{
    return (processPageBase[pid] + virtualPage) * pageSize;
}

void DemandPagingAllocator::pageIn(int pid, size_t virtualPage, char* buffer)
{
    std::ifstream backingStore("csopesy-backing-store.txt", std::ios::binary);
    size_t offset = calculatePageOffset(pid, virtualPage);
    backingStore.seekg(offset);
    backingStore.read(buffer, pageSize);
    backingStore.close();
}

void DemandPagingAllocator::pageOut(int pid, size_t virtualPage, const char* buffer)
{
    std::fstream backingStore("csopesy-backing-store.txt", std::ios::in | std::ios::out | std::ios::binary);
    size_t offset = calculatePageOffset(pid, virtualPage);
    backingStore.seekp(offset);
    backingStore.write(buffer, pageSize);
    backingStore.close();
}