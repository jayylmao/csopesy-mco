#include "DemandPagingAllocator.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstring> // For memset

DemandPagingAllocator::DemandPagingAllocator(size_t memory, size_t pageSize)
    : memory(memory), pageSize(pageSize), mainMemory(new char[memory])
{

    frameCount = memory / pageSize; // calculate number of frames.
    frameAllocationMap.resize(frameCount, false); // initialize all frames as unallocated.
    frameTable.resize(frameCount, { -1, static_cast<size_t>(-1) });


    std::ofstream backingStore("csopesy-backing-store.txt", std::ios::out | std::ios::binary | std::ios::trunc);
    if (backingStore) {
        std::cout << "Backing store 'csopesy-backing-store.txt' created/cleared." << std::endl;
        backingStore.close();
    }
    else {
        std::cerr << "Error: Could not create backing store file." << std::endl;
    }
}

DemandPagingAllocator::~DemandPagingAllocator()
{
    pageTables.clear();
    frameAllocationMap.clear();
    processPageBase.clear();
    frameTable.clear();
    delete[] mainMemory;
}

void* DemandPagingAllocator::allocate(size_t size, int pid)
{
    std::lock_guard<std::mutex> lock(allocatorMutex);
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
            false
            });
    }

    pageTables[pid] = std::move(table);

    totalAllocatedMemory += numPages * pageSize;


    return nullptr;
}

void DemandPagingAllocator::deallocate(int pid)
{
    std::lock_guard<std::mutex> lock(allocatorMutex);
    auto it = pageTables.find(pid);
    if (it == pageTables.end()) return;

    std::vector<size_t> framesToClear;

    // check for frames occupied by this process to clear
    for (const auto& entry : it->second) {
        if (entry.present && entry.physicalFrame != static_cast<size_t>(-1)) {
            size_t frame = entry.physicalFrame;
            frameAllocationMap[frame] = false;
            frameTable[frame] = { -1, static_cast<size_t>(-1) };
            framesToClear.push_back(frame);
        }
    }

    // rebuild frame queue without cleared frames.
    if (!framesToClear.empty()) {
        std::queue<size_t> newFrameQueue;

        while (!frameQueue.empty()) {
            size_t currentFrame = frameQueue.front();
            frameQueue.pop();

            if (std::find(framesToClear.begin(), framesToClear.end(), currentFrame) == framesToClear.end()) {
                newFrameQueue.push(currentFrame);
            }
        }

        frameQueue = newFrameQueue;
    }
    totalAllocatedMemory -= it->second.size() * pageSize;
    pageTables.erase(it);
    processPageBase.erase(pid);
}

std::string DemandPagingAllocator::displayMemory()
{
    std::lock_guard<std::mutex> lock(allocatorMutex);

    std::stringstream ss;
    size_t usedFrames = std::count(frameAllocationMap.begin(), frameAllocationMap.end(), true);

    size_t usedMemory = usedFrames * pageSize;
    size_t totalMemory = this->memory;

    double utilizationPercent = (totalMemory > 0) ? (usedMemory / totalMemory) * 100.0 : 0.0;

    ss << "--- Physical Memory State ---\n";
    ss << "Usage: " << usedMemory << " KB / "
        << totalMemory << " KB (" << static_cast<int>(utilizationPercent) << "%)\n";
    ss << "Frames Used: " << usedFrames << " / " << frameCount << "\n";
    ss << "-----------------------------\n";

    return ss.str();
}

void DemandPagingAllocator::pageFaultHandler(int pid, size_t virtualPage)
{
    size_t frame = findFreeFrame();

    // evict frames when none are found.
    if (frame == static_cast<size_t>(-1)) {
        if (frameQueue.empty()) {
            return;
        }
        size_t victimFrame = frameQueue.front();
        frameQueue.pop();

        auto [victimPid, victimPage] = frameTable[victimFrame];
        auto& victimTable = pageTables[victimPid];

        char* buffer = new char[pageSize];

        // copy buffer from frame to buffer
        memcpy(buffer, mainMemory + (victimFrame * pageSize), pageSize);

        // write buffer to backing store
        pageOut(victimPid, victimPage, buffer);
        delete[] buffer;

        // update victim page table entry
        victimTable[victimPage].present = false;
        victimTable[victimPage].physicalFrame = static_cast<size_t>(-1);

        frame = victimFrame;
    }

    char* buffer = new char[pageSize];

    // read page from backing store into buffer
    pageIn(pid, virtualPage, buffer);

    auto& table = pageTables[pid];

    if (virtualPage >= table.size()) {
        delete[] buffer;
        return;
    }

    // update new page table entry
    table[virtualPage].physicalFrame = frame;
    table[virtualPage].present = true;

    // copy data from buffer into physical mem frame.
    memcpy(mainMemory + (frame * pageSize), buffer, pageSize);

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


//CHECK IF REMOVING THIS BREAKS ANYTHING

void DemandPagingAllocator::pageIn(int pid, size_t virtualPage, char* buffer)
{
    std::ifstream backingStore("csopesy-backing-store.txt", std::ios::binary);
    size_t offset = calculatePageOffset(pid, virtualPage);
    backingStore.seekg(offset);
    backingStore.read(buffer, pageSize);
    backingStore.close();

    numPagedIn++;
}


void DemandPagingAllocator::pageOut(int pid, size_t virtualPage, const char* buffer)
{
    std::fstream backingStore("csopesy-backing-store.txt", std::ios::in | std::ios::out | std::ios::binary);

    if (!backingStore) {
        std::cerr << "[!] Could not open backing store." << std::endl;
    }

    size_t offset = calculatePageOffset(pid, virtualPage);
    backingStore.seekp(offset);
    backingStore.write(buffer, pageSize);
    backingStore.close();

    numPagedOut++;
}

void DemandPagingAllocator::accessPage(int pid, size_t virtualPage)
{
    auto it = pageTables.find(pid);
    if (it == pageTables.end()) {
        throw std::runtime_error("PID " + std::to_string(pid) + " not found in page tables");
    }

    auto& table = it->second;
    if (virtualPage >= table.size()) {
        throw std::runtime_error("Virtual page " + std::to_string(virtualPage) +
            " out of range for PID " + std::to_string(pid));
    }

    if (!table[virtualPage].present) {
        pageFaultHandler(pid, virtualPage);
    }
}

size_t DemandPagingAllocator::getPageSize() const
{
    return this->pageSize;
}

int DemandPagingAllocator::getUsedMemory() const {
    return static_cast<int>(totalAllocatedMemory);
}

int DemandPagingAllocator::getPagedIn() const {
    return static_cast<int>(numPagedIn);
}

int DemandPagingAllocator::getPagedOut() const {
    return static_cast<int>(numPagedOut);
}

void DemandPagingAllocator::pageInWithZeroInit(int pid, size_t virtualPage, char* buffer) {
    size_t pageSizeBytes = pageSize * 1024;
    size_t offset = calculatePageOffset(pid, virtualPage);

    std::ifstream backingStore("csopesy-backing-store.txt", std::ios::binary);
    if (!backingStore) {
        memset(buffer, 0, pageSizeBytes);
        return;
    }

    backingStore.seekg(0, std::ios::end);
    size_t fileSize = backingStore.tellg();

    if (offset >= fileSize) {
        memset(buffer, 0, pageSizeBytes);
        return;
    }

    backingStore.seekg(offset);
    backingStore.read(buffer, pageSizeBytes);

    if (static_cast<size_t>(backingStore.gcount()) < pageSizeBytes) {
        memset(buffer + backingStore.gcount(), 0, pageSizeBytes - backingStore.gcount());
    }
}

// Update readUint16 and writeUint16 to use pageInWithZeroInit
uint16_t DemandPagingAllocator::readUint16(int pid, size_t address) {
    size_t pageSizeBytes = pageSize * 1024;
    size_t pageIndex = address / pageSizeBytes;
    size_t offset = address % pageSizeBytes;

    // Check if access crosses page boundary
    if (offset > pageSizeBytes - sizeof(uint16_t)) {
        throw std::runtime_error("Cross-page access not allowed");
    }

    // Use the zero-initializing version
    char* buffer = new char[pageSize];
    pageInWithZeroInit(pid, pageIndex, buffer);

    auto& table = pageTables[pid];
    if (pageIndex >= table.size()) {
        delete[] buffer;
        throw std::runtime_error("Page index out of range");
    }

    PageTableEntry& entry = table[pageIndex];
    if (!entry.present) {
        delete[] buffer;
        throw std::runtime_error("Page not present");
    }

    uint16_t value = 0;
    memcpy(&value, buffer + offset, sizeof(uint16_t));
    delete[] buffer;
    return value;
}

void DemandPagingAllocator::writeUint16(int pid, size_t address, uint16_t value) {
    size_t pageSizeBytes = pageSize * 1024;
    size_t pageIndex = address / pageSizeBytes;
    size_t offset = address % pageSizeBytes;

    // Check if access crosses page boundary
    if (offset > pageSizeBytes - sizeof(uint16_t)) {
        throw std::runtime_error("Cross-page access not allowed");
    }

    // First ensure the page is loaded with zero-init if needed
    char* buffer = new char[pageSize];
    pageInWithZeroInit(pid, pageIndex, buffer);

    auto& table = pageTables[pid];
    if (pageIndex >= table.size()) {
        delete[] buffer;
        throw std::runtime_error("Page index out of range");
    }

    PageTableEntry& entry = table[pageIndex];
    if (!entry.present) {
        delete[] buffer;
        throw std::runtime_error("Page not present");
    }

    // Update the value in the buffer
    memcpy(buffer + offset, &value, sizeof(uint16_t));

    // Write the modified page back
    pageOut(pid, pageIndex, buffer);
    delete[] buffer;
}