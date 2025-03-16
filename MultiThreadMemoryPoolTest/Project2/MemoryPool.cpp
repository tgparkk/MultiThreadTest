#include "MemoryPool.h"

// Global memory manager instance
std::unique_ptr<Memory> GMemory = std::make_unique<Memory>();

/*-----------------
    MemoryPool
------------------*/

MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{
    ::InitializeSListHead(&_header);
}

MemoryPool::~MemoryPool()
{
    while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)))
        ::_aligned_free(memory);
}

void MemoryPool::Push(MemoryHeader* ptr)
{
    ptr->allocSize = 0;

    ::InterlockedPushEntrySList(&_header, static_cast<PSLIST_ENTRY>(ptr));

    _useCount.fetch_sub(1);
    _reserveCount.fetch_add(1);
}

MemoryHeader* MemoryPool::Pop()
{
    MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header));

    // If no memory is available, allocate new memory
    if (memory == nullptr)
    {
        memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(_allocSize, SLIST_ALIGNMENT));
    }
    else
    {
        ASSERT_CRASH(memory->allocSize == 0);
        _reserveCount.fetch_sub(1);
    }

    _useCount.fetch_add(1);

    return memory;
}

/*-----------------
    Memory Manager
------------------*/

Memory::Memory()
{
    Initialize();
}

Memory::~Memory()
{
    _pools.clear();
    _poolTable.clear();
}

MemoryPool* Memory::GetPoolBySize(int32 size)
{
    if (size > MAX_ALLOC_SIZE)
        return nullptr;

    int32 poolSize = 32;
    while (poolSize < size)
        poolSize *= 2;

    auto it = _poolTable.find(poolSize);
    return (it != _poolTable.end()) ? it->second : nullptr;
}

void Memory::Initialize()
{
    // Create memory pools with different sizes
    // We'll create pools for sizes: 32, 64, 128, 256, 512, 1024, 2048, 4096
    const int32 sizes[] = { 32, 64, 128, 256, 512, 1024, 2048, MAX_ALLOC_SIZE };

    for (int32 size : sizes)
    {
        _pools.push_back(std::make_unique<MemoryPool>(size));
        _poolTable[size] = _pools.back().get();
    }
}

void* Memory::Allocate(int32 size)
{
    MemoryHeader* header = nullptr;
    const int32 allocSize = size + sizeof(MemoryHeader);

    if (allocSize > MAX_ALLOC_SIZE)
    {
        // If size exceeds the maximum pooling size, use regular allocation
        header = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(allocSize, SLIST_ALIGNMENT));
    }
    else
    {
        // Find the appropriate pool size
        int32 poolSize = 32;
        while (poolSize < allocSize)
            poolSize *= 2;

        // Get memory from the pool
        header = _poolTable[poolSize]->Pop();
    }

    return MemoryHeader::AttachHeader(header, allocSize);
}

void Memory::Release(void* ptr)
{
    MemoryHeader* header = MemoryHeader::DetachHeader(ptr);

    const int32 allocSize = header->allocSize;
    ASSERT_CRASH(allocSize > 0);

    if (allocSize > MAX_ALLOC_SIZE)
    {
        // If size exceeds the maximum pooling size, use regular deallocation
        ::_aligned_free(header);
    }
    else
    {
        // Find the appropriate pool size
        int32 poolSize = 32;
        while (poolSize < allocSize)
            poolSize *= 2;

        // Return memory to the pool
        _poolTable[poolSize]->Push(header);
    }
}

/*-----------------
    Pool Allocator
------------------*/

void* PoolAllocator::Alloc(int32 size)
{
    return GMemory->Allocate(size);
}

void PoolAllocator::Release(void* ptr)
{
    GMemory->Release(ptr);
}