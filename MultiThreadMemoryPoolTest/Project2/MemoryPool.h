#pragma once

#include <atomic>
#include <memory>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <cassert>
#include <Windows.h>
#include <map>
#include <set>

// Define common types
using int32 = int32_t;

// Define assert macro
#define ASSERT_CRASH(expr) assert(expr)

enum
{
    SLIST_ALIGNMENT = 16,
    MAX_ALLOC_SIZE = 4096 // Maximum size for memory pooling
};

/*-----------------
    MemoryHeader
------------------*/

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
    // [MemoryHeader][Data]
    MemoryHeader(int32 size) : allocSize(size) {}

    static void* AttachHeader(MemoryHeader* header, int32 size)
    {
        new(header)MemoryHeader(size); // placement new
        return reinterpret_cast<void*>(++header);
    }

    static MemoryHeader* DetachHeader(void* ptr)
    {
        MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
        return header;
    }

    int32 allocSize;
};

/*-----------------
    MemoryPool
------------------*/

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
    MemoryPool(int32 allocSize);
    ~MemoryPool();

    void Push(MemoryHeader* ptr);
    MemoryHeader* Pop();

    int32 GetAllocSize() const { return _allocSize; }
    int32 GetUseCount() const { return _useCount; }
    int32 GetReserveCount() const { return _reserveCount; }

private:
    SLIST_HEADER _header;
    int32 _allocSize = 0;
    std::atomic<int32> _useCount = 0;
    std::atomic<int32> _reserveCount = 0;
};

/*-----------------
    Memory Manager
------------------*/

class Memory
{
public:
    Memory();
    ~Memory();

    void Initialize();
    void* Allocate(int32 size);
    void Release(void* ptr);

    // For statistics
    MemoryPool* GetPoolBySize(int32 size);

private:
    std::vector<std::unique_ptr<MemoryPool>> _pools;
    std::unordered_map<int32, MemoryPool*> _poolTable; // Fast lookup by size
};

/*-----------------
    Pool Allocator
------------------*/

class PoolAllocator
{
public:
    static void* Alloc(int32 size);
    static void Release(void* ptr);
};

/*-----------------
    STL Allocator
------------------*/

template<typename T>
class StlAllocator
{
public:
    using value_type = T;

    StlAllocator() = default;

    template<typename U>
    StlAllocator(const StlAllocator<U>&) {}

    T* allocate(size_t n)
    {
        const int32 size = static_cast<int32>(n * sizeof(T));
        return static_cast<T*>(PoolAllocator::Alloc(size));
    }

    void deallocate(T* p, size_t n)
    {
        PoolAllocator::Release(p);
    }

    template<typename U>
    bool operator==(const StlAllocator<U>&) const { return true; }

    template<typename U>
    bool operator!=(const StlAllocator<U>&) const { return false; }
};

/*-----------------
    Custom New/Delete
------------------*/

template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
    Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
    new(memory)Type(std::forward<Args>(args)...); // placement new
    return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
    obj->~Type();
    PoolAllocator::Release(obj);
}

template<typename Type, typename... Args>
std::shared_ptr<Type> MakeShared(Args&&... args)
{
    return std::shared_ptr<Type>{ xnew<Type>(std::forward<Args>(args)...), xdelete<Type> };
}

/*-----------------
    STL Container Aliases
------------------*/

template<typename Type>
using Vector = std::vector<Type, StlAllocator<Type>>;

template<typename Key, typename Value, typename Pred = std::less<Key>>
using Map = std::map<Key, Value, Pred, StlAllocator<std::pair<const Key, Value>>>;

template<typename Key, typename Pred = std::less<Key>>
using Set = std::set<Key, Pred, StlAllocator<Key>>;

// Global memory manager instance
extern std::unique_ptr<Memory> GMemory;