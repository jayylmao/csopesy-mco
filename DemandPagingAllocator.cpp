#include "DemandPagingAllocator.h"
#include <iostream>
#include <fstream>

DemandPagingAllocator::DemandPagingAllocator(size_t memory, size_t pageSize)
    : memory(memory), pageSize(pageSize)
{
    frameCount = memory / pageSize; // calculate number of frames.
    frameAllocationMap.resize(frameCount, false); // initialize all frames as unallocated.
    frameTable.resize(frameCount, { -1, static_cast<size_t>(-1) });
}

DemandPagingAllocator::~DemandPagingAllocator()
{
    pageTables.clear();
    frameAllocationMap.clear();
    processPageBase.clear();
    frameTable.clear();
}

void* DemandPagingAllocator::allocate(size_t size, int pid)
{
    // shift free page base tracker over to by the number of pages allocated.
    size_t numPages = (size + pageSize - 1) / pageSize;
    if (pageSize == 0) throw std::runtime_error("Page size is zero");
    processPageBase[pid] = nextFreePageBase;
    nextFreePageBase += numPages;

    std::vector<PageTableEntry> table;
    for (size_t i = 0; i < numPages; ++i) {
        table.push_back(PageTableEntry{
            i,
            static_cast<size_t>(-1),
            false,
            false
            });
    }

    pageTables[pid] = std::move(table);
    return nullptr;
}

void DemandPagingAllocator::deallocate(int pid)
{
    auto it = pageTables.find(pid);
    if (it == pageTables.end()) return;

    for (const auto& entry : it->second) {
        if (entry.present && entry.physicalFrame != static_cast<size_t>(-1)) {
            frameAllocationMap[entry.physicalFrame] = false;
        }
    }

    pageTables.erase(it);
    processPageBase.erase(pid);
}

std::string DemandPagingAllocator::displayMemory()
{
    return "TODO: implement memory display";
}

void DemandPagingAllocator::pageFaultHandler(int pid, size_t virtualPage)
{
    size_t frame = findFreeFrame();

    // evict frames when none are found.
    if (frame == static_cast<size_t>(-1)) {
        size_t victimFrame = frameQueue.front();
        frameQueue.pop();

        auto [victimPid, victimPage] = frameTable[victimFrame];
        auto& victimTable = pageTables[victimPid];

        if (victimTable[victimPage].present) {
            char* buffer = new char[pageSize];
            pageOut(victimPid, victimPage, buffer);
            delete[] buffer;
        }

        victimTable[victimPage].present = false;
        victimTable[victimPage].physicalFrame = static_cast<size_t>(-1);

        frame = victimFrame;
    }

    char* buffer = new char[pageSize];
    pageIn(pid, virtualPage, buffer);

    auto& table = pageTables[pid];

    if (virtualPage >= table.size()) {
        delete[] buffer;
        return;
    }

    table[virtualPage].physicalFrame = frame;
    table[virtualPage].present = true;
    table[virtualPage].dirty = false;

    frameAllocationMap[frame] = true;
    frameQueue.push(frame);
    frameTable[frame] = {pid, virtualPage};

    delete[] buffer;
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

void DemandPagingAllocator::accessPage(int pid, size_t virtualPage)
{
    auto it = pageTables.find(pid);
    if (it == pageTables.end()) std::cerr << "PID not found in page tables." << std::endl;
    auto& table = it->second;
    if (virtualPage >= table.size()) std::cerr << "Virtual page index out of range." << std::endl;

    if (!table[virtualPage].present) {
        pageFaultHandler(pid, virtualPage);
    }
}

size_t DemandPagingAllocator::getPageSize() const
{
    return this->pageSize;
}