#include "my_vector.h"
#include <cstring>

using std::make_shared;
using std::shared_ptr;

my_vector::big_vector::big_vector(size_t capacity, shared_ptr<uint32_t> data) :
        capacity(capacity),
        data(std::move(data))
{}

my_vector::big_vector::big_vector(big_vector const &other) :
        capacity(other.capacity),
        data(other.data)
{}

const uint32_t* my_vector::big_vector::begin() const {
    return data.get();
}

uint32_t* my_vector::big_vector::begin() {
    return data.get();
}

const uint32_t* my_vector::big_vector::end() const {
    return data.get() + capacity;
}

uint32_t* my_vector::big_vector::end() {
    return data.get() + capacity;
}

const uint32_t& my_vector::big_vector::operator[](size_t idx) const {
    return data.get()[idx];
}

uint32_t& my_vector::big_vector::operator[](size_t idx) {
    return data.get()[idx];
}

void my_vector::big_vector::cut(size_t useful_size) {
    if (data.unique())
        return;

    static const size_t OVERSIZE = 2;
    capacity = useful_size + OVERSIZE;

    auto new_data = new uint32_t[capacity];
    std::copy(begin(), begin() + useful_size, new_data);
    data.reset(new_data, std::default_delete<uint32_t[]>());
}

void my_vector::big_vector::guarantee_capacity(size_t cap) {
    if (cap > capacity) {
        size_t new_capacity = std::max(cap + 2, capacity * 2);
        auto new_data = new uint32_t[new_capacity];
        std::copy(begin(), end(), new_data);

        capacity = new_capacity;
        data.reset(new_data, std::default_delete<uint32_t[]>());
    }
}

my_vector::my_vector() : siz(0) {}

my_vector::my_vector(size_t n) : siz(n) {
    if (is_small()) {
        for (size_t i = 0; i < n; ++i)
            small_data[i] = 0;
    } else {
        new(&data) big_vector(n, shared_ptr<uint32_t>(new uint32_t[n], std::default_delete<uint32_t[]>()));
    }
}

my_vector::my_vector(size_t n, uint32_t val) : siz(n) {
    if (is_small())
        for (size_t i = 0; i < n; ++i)
            small_data[i] = val;
    else {
        new(&data) big_vector(n, shared_ptr<uint32_t>(new uint32_t[n], std::default_delete<uint32_t[]>()));
        for (size_t i = 0; i < n; ++i)
            data.begin()[i] = val;
    }
}

my_vector::my_vector(my_vector const &other) : siz(other.siz) {
    if (is_small())
        std::copy(other.small_data, other.small_data + SMALL_OBJECT_SIZE, small_data);
    else {
        new(&data) big_vector(other.data.capacity, other.data.data);
    }
}

my_vector::my_vector(std::initializer_list<uint32_t> init_data) : siz(init_data.size()) {
    if (is_small())
        std::copy(init_data.begin(), init_data.end(), small_data);
    else {
        new(&data) big_vector(init_data.size(),
                              shared_ptr<uint32_t>(new uint32_t[init_data.size()], std::default_delete<uint32_t[]>()));
        std::copy(init_data.begin(), init_data.end(), data.begin());
    }
}

my_vector::~my_vector() {
    if (!is_small()) {
        data.~big_vector();
    }
}

bool my_vector::is_small() const {
    return siz <= SMALL_OBJECT_SIZE;
}

const uint32_t& my_vector::operator[](size_t idx) const {
    return (is_small() ? small_data[idx] : data[idx]);
}

uint32_t& my_vector::operator[](size_t idx) {
    cut();
    if (is_small())
        return small_data[idx];
    return data[idx];
}

void my_vector::swap(my_vector &other) noexcept {
    char tmp[sizeof(my_vector)];
    memcpy(&tmp, this, sizeof(my_vector));
    memcpy(this, &other, sizeof(my_vector));
    memcpy(&other, &tmp, sizeof(my_vector));
}

void swap(my_vector &a, my_vector &b) {
    a.swap(b);
}

my_vector& my_vector::operator=(my_vector const &other) {
    my_vector tmp(other);
    swap(tmp);
    return *this;
}

void my_vector::cut() {
    if (!is_small()) {
        data.cut(siz);
    }
}

size_t my_vector::size() const {
    return siz;
}

void my_vector::to_big() {
    size_t capacity = std::max(SMALL_OBJECT_SIZE + 2, siz * 2);

    shared_ptr<uint32_t> ptr(new uint32_t[capacity], std::default_delete<uint32_t[]>());
    std::copy(small_data, small_data + siz, ptr.get());
    new(&data) big_vector(capacity, ptr);
}

void my_vector::to_small() {
    uint32_t tmp[SMALL_OBJECT_SIZE];
    std::move(data.begin(), data.begin() + siz, tmp);
    data.data.~shared_ptr<uint32_t>();
    std::move(tmp, tmp + SMALL_OBJECT_SIZE, small_data);
}

void my_vector::push_back(uint32_t const &val) {
    if (siz < SMALL_OBJECT_SIZE) {
        small_data[siz] = val;
        ++siz;
        return;
    }
    if (siz == SMALL_OBJECT_SIZE)
        to_big();
    data.guarantee_capacity(siz + 1);
    data[siz] = val;
    ++siz;
}

const uint32_t& my_vector::back() const {
    if (siz == 0)
        throw std::runtime_error("back() is not allowed in empty vector");
    if (is_small())
        return small_data[siz - 1];
    return data[siz - 1];
}

uint32_t& my_vector::back() {
    if (siz == 0)
        throw std::runtime_error("back() is not allowed in empty vector");
    if (is_small())
        return small_data[siz - 1];
    return data[siz - 1];
}

void my_vector::pop_back() {
    --siz;
    if (siz == SMALL_OBJECT_SIZE)
        to_small();
}

const uint32_t* my_vector::begin() const {
    if (is_small())
        return small_data;
    return data.begin();
}

uint32_t* my_vector::begin() {
    if (is_small())
        return small_data;
    return data.begin();
}


const uint32_t* my_vector::end() const {
    if (is_small())
        return small_data + siz;
    return data.begin() + siz;
}

uint32_t* my_vector::end() {
    if (is_small())
        return small_data + siz;
    return data.begin() + siz;
}

void my_vector::resize(size_t n) {
    if (is_small()) {
        if (n <= SMALL_OBJECT_SIZE) {
            for (size_t i = siz; i < SMALL_OBJECT_SIZE; ++i)
                small_data[i] = 0;
        } else {
            to_big();
            data.guarantee_capacity(n);
            for (size_t i = siz; i < n; ++i)
                data[i] = 0;
        }
    } else {
        if (n <= SMALL_OBJECT_SIZE) {
            siz = n;
            to_small();
        } else {
            data.guarantee_capacity(n);
            for (size_t i = siz; i < n; ++i)
                data[i] = 0;
        }
    }
    siz = n;
}