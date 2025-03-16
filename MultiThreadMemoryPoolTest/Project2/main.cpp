#include "MemoryPool.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <random>
#include <algorithm>
#include <string>

// Define test objects of various sizes
struct SmallObject {
    int value;
    char data[16];

    SmallObject() : value(0) { memset(data, 0, sizeof(data)); }
    SmallObject(int v) : value(v) { memset(data, v % 256, sizeof(data)); }
    ~SmallObject() {}
};

struct MediumObject {
    int value1;
    float value2;
    double value3;
    char data[112]; // Total: ~128 bytes

    MediumObject() : value1(0), value2(0.0f), value3(0.0) { memset(data, 0, sizeof(data)); }
    MediumObject(int v) : value1(v), value2(v * 1.5f), value3(v * 2.0) { memset(data, v % 256, sizeof(data)); }
    ~MediumObject() {}
};

struct LargeObject {
    int values[32];
    double doubleValues[32];
    char data[256]; // Total: ~512+ bytes

    LargeObject() {
        memset(values, 0, sizeof(values));
        memset(doubleValues, 0, sizeof(doubleValues));
        memset(data, 0, sizeof(data));
    }

    LargeObject(int v) {
        for (int i = 0; i < 32; ++i) {
            values[i] = v + i;
            doubleValues[i] = (v + i) * 1.5;
        }
        memset(data, v % 256, sizeof(data));
    }

    ~LargeObject() {}
};

struct HugeObject {
    int values[128];
    double doubleValues[128];
    char data[1024]; // Total: ~2048+ bytes

    HugeObject() {
        memset(values, 0, sizeof(values));
        memset(doubleValues, 0, sizeof(doubleValues));
        memset(data, 0, sizeof(data));
    }

    HugeObject(int v) {
        for (int i = 0; i < 128; ++i) {
            values[i] = v + i;
            doubleValues[i] = (v + i) * 1.5;
        }
        memset(data, v % 256, sizeof(data));
    }

    ~HugeObject() {}
};

// Base template for allocation tests
template<typename T>
void StandardAllocationTest(int iterations) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<T*> objects;
    objects.reserve(iterations);

    for (int i = 0; i < iterations; ++i) {
        T* obj = new T(i);
        objects.push_back(obj);
    }

    for (auto& obj : objects) {
        delete obj;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Standard allocation (" << typeid(T).name() << ", " << sizeof(T)
        << " bytes): " << duration.count() << " microseconds" << std::endl;
}

template<typename T>
void PoolAllocationTest(int iterations) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<T*> objects;
    objects.reserve(iterations);

    for (int i = 0; i < iterations; ++i) {
        T* obj = xnew<T>(i);
        objects.push_back(obj);
    }

    for (auto& obj : objects) {
        xdelete(obj);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Pool allocation (" << typeid(T).name() << ", " << sizeof(T)
        << " bytes): " << duration.count() << " microseconds" << std::endl;
}

template<typename T>
void SharedPtrStandardTest(int iterations) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::shared_ptr<T>> objects;
    objects.reserve(iterations);

    for (int i = 0; i < iterations; ++i) {
        objects.push_back(std::make_shared<T>(i));
    }

    objects.clear();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Standard shared_ptr (" << typeid(T).name() << ", " << sizeof(T)
        << " bytes): " << duration.count() << " microseconds" << std::endl;
}

template<typename T>
void SharedPtrPoolTest(int iterations) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::shared_ptr<T>> objects;
    objects.reserve(iterations);

    for (int i = 0; i < iterations; ++i) {
        objects.push_back(MakeShared<T>(i));
    }

    objects.clear();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Pool shared_ptr (" << typeid(T).name() << ", " << sizeof(T)
        << " bytes): " << duration.count() << " microseconds" << std::endl;
}

// Mixed-size allocation test
void MixedSizeAllocationTest(int iterations, bool usePool) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distType(0, 3); // 0-3 for different object types

    auto start = std::chrono::high_resolution_clock::now();

    // To keep track of all allocated objects
    std::vector<void*> smallObjects;
    std::vector<void*> mediumObjects;
    std::vector<void*> largeObjects;
    std::vector<void*> hugeObjects;

    //smallObjects.reserve(iterations / 2);  // Allocate more small objects
    //mediumObjects.reserve(iterations / 4);
    //largeObjects.reserve(iterations / 8);
    //hugeObjects.reserve(iterations / 8);

    smallObjects.reserve(iterations);  // Allocate more small objects
    mediumObjects.reserve(iterations);
    largeObjects.reserve(iterations);
    hugeObjects.reserve(iterations);

    for (int i = 0; i < iterations; ++i) {
        int objectType = distType(gen);

        if (usePool) {
            switch (objectType) {
            case 0:
                smallObjects.push_back(xnew<SmallObject>(i));
                break;
            case 1:
                mediumObjects.push_back(xnew<MediumObject>(i));
                break;
            case 2:
                largeObjects.push_back(xnew<LargeObject>(i));
                break;
            case 3:
                hugeObjects.push_back(xnew<HugeObject>(i));
                break;
            }
        }
        else {
            switch (objectType) {
            case 0:
                smallObjects.push_back(new SmallObject(i));
                break;
            case 1:
                mediumObjects.push_back(new MediumObject(i));
                break;
            case 2:
                largeObjects.push_back(new LargeObject(i));
                break;
            case 3:
                hugeObjects.push_back(new HugeObject(i));
                break;
            }
        }
    }

    // Deallocate in random order to simulate real-world scenario
    std::vector<std::pair<int, void*>> allObjects;
    for (auto obj : smallObjects) allObjects.push_back({ 0, obj });
    for (auto obj : mediumObjects) allObjects.push_back({ 1, obj });
    for (auto obj : largeObjects) allObjects.push_back({ 2, obj });
    for (auto obj : hugeObjects) allObjects.push_back({ 3, obj });

    std::shuffle(allObjects.begin(), allObjects.end(), gen);

    for (auto& object : allObjects) {
        if (usePool) {
            switch (object.first) {
            case 0: xdelete(static_cast<SmallObject*>(object.second)); break;
            case 1: xdelete(static_cast<MediumObject*>(object.second)); break;
            case 2: xdelete(static_cast<LargeObject*>(object.second)); break;
            case 3: xdelete(static_cast<HugeObject*>(object.second)); break;
            }
        }
        else {
            switch (object.first) {
            case 0: delete static_cast<SmallObject*>(object.second); break;
            case 1: delete static_cast<MediumObject*>(object.second); break;
            case 2: delete static_cast<LargeObject*>(object.second); break;
            case 3: delete static_cast<HugeObject*>(object.second); break;
            }
        }
    }


    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << (usePool ? "Mixed pool allocation" : "Mixed standard allocation")
        << ": " << duration.count() << " microseconds" << std::endl;
}

// Multi-threaded allocation test with mixed sizes
void MultiThreadedMixedTest(int threadCount, int iterationsPerThread) {
    std::cout << "Multi-threaded mixed-size test with " << threadCount << " threads, "
        << iterationsPerThread << " iterations per thread" << std::endl;

    std::vector<std::thread> threads;

    auto standardTest = [iterationsPerThread]() {
        MixedSizeAllocationTest(iterationsPerThread, false);
        };

    auto poolTest = [iterationsPerThread]() {
        MixedSizeAllocationTest(iterationsPerThread, true);
        };

    // Run standard allocation in multiple threads
    auto standardStart = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < threadCount; ++i) {
        threads.push_back(std::thread(standardTest));
    }

    for (auto& t : threads) {
        t.join();
    }

    auto standardEnd = std::chrono::high_resolution_clock::now();
    auto standardDuration = std::chrono::duration_cast<std::chrono::microseconds>(standardEnd - standardStart);

    threads.clear();

    // Run pool allocation in multiple threads
    auto poolStart = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < threadCount; ++i) {
        threads.push_back(std::thread(poolTest));
    }

    for (auto& t : threads) {
        t.join();
    }

    auto poolEnd = std::chrono::high_resolution_clock::now();
    auto poolDuration = std::chrono::duration_cast<std::chrono::microseconds>(poolEnd - poolStart);

    std::cout << "Multi-threaded mixed standard allocation total: " << standardDuration.count() << " microseconds" << std::endl;
    std::cout << "Multi-threaded mixed pool allocation total: " << poolDuration.count() << " microseconds" << std::endl;
    std::cout << "Speedup ratio: " << static_cast<double>(standardDuration.count()) / poolDuration.count() << "x" << std::endl;
}

// Allocation stress test - allocate and free in patterns
void AllocationStressTest(int iterations) {
    std::cout << "\n--- Allocation Stress Test ---" << std::endl;

    // Allocation patterns to test
    const int patternCount = 4;
    const char* patternNames[patternCount] = {
        "Allocate all, then free all",
        "Allocate one, free one",
        "Allocate 100, free 100",
        "Random allocation/deallocation"
    };

    for (int pattern = 0; pattern < patternCount; pattern++) {
        std::cout << "\nPattern: " << patternNames[pattern] << std::endl;

        // Standard allocation test
        auto stdStart = std::chrono::high_resolution_clock::now();

        std::vector<void*> stdObjects;
        stdObjects.reserve(iterations);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 3);

        switch (pattern) {
        case 0: { // Allocate all, then free all
            for (int i = 0; i < iterations; i++) {
                int type = dist(gen);
                switch (type) {
                case 0: stdObjects.push_back(new SmallObject(i)); break;
                case 1: stdObjects.push_back(new MediumObject(i)); break;
                case 2: stdObjects.push_back(new LargeObject(i)); break;
                case 3: stdObjects.push_back(new HugeObject(i)); break;
                }
            }

            for (auto obj : stdObjects) {
                delete static_cast<SmallObject*>(obj); // Delete as smallest type, safe because we don't use virtual
            }

            stdObjects.clear();
            break;
        }
        case 1: { // Allocate one, free one
            for (int i = 0; i < iterations; i++) {
                int type = dist(gen);
                void* obj = nullptr;

                switch (type) {
                case 0: obj = new SmallObject(i); break;
                case 1: obj = new MediumObject(i); break;
                case 2: obj = new LargeObject(i); break;
                case 3: obj = new HugeObject(i); break;
                }

                switch (type) {
                case 0: delete static_cast<SmallObject*>(obj); break;
                case 1: delete static_cast<MediumObject*>(obj); break;
                case 2: delete static_cast<LargeObject*>(obj); break;
                case 3: delete static_cast<HugeObject*>(obj); break;
                }
            }
            break;
        }
        case 2: { // Allocate 100, free 100
            for (int i = 0; i < iterations; i++) {
                if (i % 100 == 0 && !stdObjects.empty()) {
                    for (auto obj : stdObjects) {
                        delete static_cast<SmallObject*>(obj);
                    }
                    stdObjects.clear();
                }

                int type = dist(gen);
                switch (type) {
                case 0: stdObjects.push_back(new SmallObject(i)); break;
                case 1: stdObjects.push_back(new MediumObject(i)); break;
                case 2: stdObjects.push_back(new LargeObject(i)); break;
                case 3: stdObjects.push_back(new HugeObject(i)); break;
                }
            }

            for (auto obj : stdObjects) {
                delete static_cast<SmallObject*>(obj);
            }

            stdObjects.clear();
            break;
        }
        case 3: { // Random allocation/deallocation
            std::uniform_int_distribution<> opDist(0, 1); // 0 = allocate, 1 = deallocate

            for (int i = 0; i < iterations; i++) {
                bool shouldAllocate = (opDist(gen) == 0) || stdObjects.empty();

                if (shouldAllocate) {
                    int type = dist(gen);
                    switch (type) {
                    case 0: stdObjects.push_back(new SmallObject(i)); break;
                    case 1: stdObjects.push_back(new MediumObject(i)); break;
                    case 2: stdObjects.push_back(new LargeObject(i)); break;
                    case 3: stdObjects.push_back(new HugeObject(i)); break;
                    }
                }
                else {
                    std::uniform_int_distribution<> objDist(0, static_cast<int>(stdObjects.size()) - 1);
                    int index = objDist(gen);
                    delete static_cast<SmallObject*>(stdObjects[index]);
                    stdObjects[index] = stdObjects.back();
                    stdObjects.pop_back();
                }
            }

            for (auto obj : stdObjects) {
                delete static_cast<SmallObject*>(obj);
            }

            stdObjects.clear();
            break;
        }
        }

        auto stdEnd = std::chrono::high_resolution_clock::now();
        auto stdDuration = std::chrono::duration_cast<std::chrono::microseconds>(stdEnd - stdStart);

        // Pool allocation test
        auto poolStart = std::chrono::high_resolution_clock::now();

        std::vector<void*> poolObjects;
        poolObjects.reserve(iterations);

        switch (pattern) {
        case 0: { // Allocate all, then free all
            for (int i = 0; i < iterations; i++) {
                int type = dist(gen);
                switch (type) {
                case 0: poolObjects.push_back(xnew<SmallObject>(i)); break;
                case 1: poolObjects.push_back(xnew<MediumObject>(i)); break;
                case 2: poolObjects.push_back(xnew<LargeObject>(i)); break;
                case 3: poolObjects.push_back(xnew<HugeObject>(i)); break;
                }
            }

            for (auto obj : poolObjects) {
                xdelete(static_cast<SmallObject*>(obj)); // Delete as smallest type
            }

            poolObjects.clear();
            break;
        }
        case 1: { // Allocate one, free one
            for (int i = 0; i < iterations; i++) {
                int type = dist(gen);
                void* obj = nullptr;

                switch (type) {
                case 0: obj = xnew<SmallObject>(i); break;
                case 1: obj = xnew<MediumObject>(i); break;
                case 2: obj = xnew<LargeObject>(i); break;
                case 3: obj = xnew<HugeObject>(i); break;
                }

                switch (type) {
                case 0: xdelete(static_cast<SmallObject*>(obj)); break;
                case 1: xdelete(static_cast<MediumObject*>(obj)); break;
                case 2: xdelete(static_cast<LargeObject*>(obj)); break;
                case 3: xdelete(static_cast<HugeObject*>(obj)); break;
                }
            }
            break;
        }
        case 2: { // Allocate 100, free 100
            for (int i = 0; i < iterations; i++) {
                if (i % 100 == 0 && !poolObjects.empty()) {
                    for (auto obj : poolObjects) {
                        xdelete(static_cast<SmallObject*>(obj));
                    }
                    poolObjects.clear();
                }

                int type = dist(gen);
                switch (type) {
                case 0: poolObjects.push_back(xnew<SmallObject>(i)); break;
                case 1: poolObjects.push_back(xnew<MediumObject>(i)); break;
                case 2: poolObjects.push_back(xnew<LargeObject>(i)); break;
                case 3: poolObjects.push_back(xnew<HugeObject>(i)); break;
                }
            }

            for (auto obj : poolObjects) {
                xdelete(static_cast<SmallObject*>(obj));
            }

            poolObjects.clear();
            break;
        }
        case 3: { // Random allocation/deallocation
            std::uniform_int_distribution<> opDist(0, 1); // 0 = allocate, 1 = deallocate

            for (int i = 0; i < iterations; i++) {
                bool shouldAllocate = (opDist(gen) == 0) || poolObjects.empty();

                if (shouldAllocate) {
                    int type = dist(gen);
                    switch (type) {
                    case 0: poolObjects.push_back(xnew<SmallObject>(i)); break;
                    case 1: poolObjects.push_back(xnew<MediumObject>(i)); break;
                    case 2: poolObjects.push_back(xnew<LargeObject>(i)); break;
                    case 3: poolObjects.push_back(xnew<HugeObject>(i)); break;
                    }
                }
                else {
                    std::uniform_int_distribution<> objDist(0, static_cast<int>(poolObjects.size()) - 1);
                    int index = objDist(gen);
                    xdelete(static_cast<SmallObject*>(poolObjects[index]));
                    poolObjects[index] = poolObjects.back();
                    poolObjects.pop_back();
                }
            }

            for (auto obj : poolObjects) {
                xdelete(static_cast<SmallObject*>(obj));
            }

            poolObjects.clear();
            break;
        }
        }

        auto poolEnd = std::chrono::high_resolution_clock::now();
        auto poolDuration = std::chrono::duration_cast<std::chrono::microseconds>(poolEnd - poolStart);

        std::cout << "Standard: " << stdDuration.count() << " microseconds" << std::endl;
        std::cout << "Pool: " << poolDuration.count() << " microseconds" << std::endl;
        std::cout << "Speedup: " << static_cast<double>(stdDuration.count()) / poolDuration.count() << "x" << std::endl;
    }
}

int main() {
    const int iterations = 100000;

    std::cout << "===== MEMORY POOL PERFORMANCE TEST =====" << std::endl;
    std::cout << "Size of objects:" << std::endl;
    std::cout << "SmallObject: " << sizeof(SmallObject) << " bytes" << std::endl;
    std::cout << "MediumObject: " << sizeof(MediumObject) << " bytes" << std::endl;
    std::cout << "LargeObject: " << sizeof(LargeObject) << " bytes" << std::endl;
    std::cout << "HugeObject: " << sizeof(HugeObject) << " bytes" << std::endl;
    std::cout << std::endl;

    // Single-size basic tests
    std::cout << "===== BASIC ALLOCATION TESTS =====" << std::endl;

    std::cout << "\n--- Single-size standard vs pool ---" << std::endl;
    StandardAllocationTest<SmallObject>(iterations);
    PoolAllocationTest<SmallObject>(iterations);

    StandardAllocationTest<MediumObject>(iterations);
    PoolAllocationTest<MediumObject>(iterations);

    StandardAllocationTest<LargeObject>(iterations);
    PoolAllocationTest<LargeObject>(iterations);

    StandardAllocationTest<HugeObject>(iterations / 10); // Less iterations for huge objects
    PoolAllocationTest<HugeObject>(iterations / 10);

    // Shared pointer tests
    std::cout << "\n--- Shared pointer standard vs pool ---" << std::endl;
    SharedPtrStandardTest<SmallObject>(iterations);
    SharedPtrPoolTest<SmallObject>(iterations);

    SharedPtrStandardTest<MediumObject>(iterations);
    SharedPtrPoolTest<MediumObject>(iterations);

    // Mixed size tests
    std::cout << "\n--- Mixed size allocation tests ---" << std::endl;
    MixedSizeAllocationTest(iterations, false); // Standard allocation
    MixedSizeAllocationTest(iterations, true);  // Pool allocation

    // Multi-threaded tests with mixed sizes
    std::cout << "\n--- Multi-threaded mixed size tests ---" << std::endl;
    MultiThreadedMixedTest(4, iterations);

    // Allocation stress test
    AllocationStressTest(iterations / 10);

    // Print memory pool statistics
    std::cout << "\n===== MEMORY POOL STATISTICS =====" << std::endl;
    constexpr int32 statSizes[] = { 32, 64, 128, 256, 512, 1024, 2048, 4096 };
    for (int32 i = 0; i < sizeof(statSizes) / sizeof(statSizes[0]); ++i) {
        int32 size = statSizes[i];
        MemoryPool* pool = GMemory->GetPoolBySize(size);
        if (pool) {
            std::cout << "Pool size " << size << " bytes: "
                << "Used = " << pool->GetUseCount() << ", "
                << "Reserved = " << pool->GetReserveCount() << std::endl;
        }
    }

    return 0;
}