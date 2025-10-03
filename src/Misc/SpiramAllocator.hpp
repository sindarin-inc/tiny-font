#pragma once

#include <memory>
#include <unordered_map>

#if CONFIG_USE_SPIRAM
#include <esp_heap_caps.h>

template <typename T>
struct SpiramAllocator {
    using value_type = T;

    SpiramAllocator() = default;

    template <typename U>
    SpiramAllocator(const SpiramAllocator<U> &) {}

    auto allocate(std::size_t n) -> T * {
        // Using a pointer type as T will otherwise raises a warning since a common bug is to use a
        // pointer type in sizeof incorrectly. But we really do want the size of the pointer. As of
        // this writing, there are a couple of `SpiramMap` that have `const char *` as their values.
        // NOLINTNEXTLINE(bugprone-sizeof-expression)
        return reinterpret_cast<T *>(heap_caps_malloc(n * sizeof(T), MALLOC_CAP_SPIRAM));
    }

    template <typename... ARGS>
    auto allocateShared(ARGS &&...args) -> std::shared_ptr<T> {
        T *tp = allocate(1);
        assert(tp);
        T *p = new (tp) T(std::forward<ARGS>(args)...);
        return std::shared_ptr<T>(p, &heap_caps_free);
    }

    void deallocate(T *p, std::size_t n) { heap_caps_free(reinterpret_cast<void *>(p)); }
};


template <typename K, typename V>
using SpiramMap = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>,
                                     SpiramAllocator<std::pair<const K, V>>>;

#endif
