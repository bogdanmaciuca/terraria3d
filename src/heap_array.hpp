#pragma once
#include <cstddef>
#include <cstdlib>

template<typename TElem, std::size_t TSize>
struct HeapArray {
public:
    HeapArray() { _ptr = static_cast<TElem*>(std::malloc(sizeof(TElem) * TSize)); }
    ~HeapArray() { free(_ptr); }
    HeapArray(HeapArray&& other) : _ptr(other._ptr) { other._ptr = nullptr; }
    TElem& operator[](std::size_t idx) { return _ptr[idx]; }
    const TElem& operator[](std::size_t idx) const { return _ptr[idx]; }
    std::size_t Size() { return TSize; }
    std::size_t Size() const { return TSize; }
    TElem* Get() { return static_cast<TElem*>(_ptr); }
    const TElem* Get() const { return static_cast<TElem*>(_ptr); }
protected:
    TElem* _ptr;
};

template<typename TElem, std::size_t TSize>
struct HeapArrayCountable : public HeapArray<TElem, TSize> {
public:
    void Push(const TElem& elem) {
        this->_ptr[_count++] = elem;
    }
    std::size_t Size() { return _count; }
private:
    std::size_t _count = 0;
};

