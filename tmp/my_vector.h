#ifndef my_vector_H
#define my_vector_H

#include <cstdint>
#include <cstddef>
#include <memory>

class my_vector
{
    struct big_vector
    {
        size_t capacity;
        std::shared_ptr <uint32_t> data;

        big_vector(size_t capacity, std::shared_ptr<uint32_t> data);
        big_vector(big_vector const &other);

        uint32_t* begin();
        const uint32_t* begin() const;
        uint32_t* end();
        const uint32_t* end() const;

        const uint32_t& operator[](size_t idx) const;
        uint32_t& operator[](size_t idx);

        //makes object unique
        void cut(size_t useful_data);

        void guarantee_capacity(size_t cap);
    };

    static const size_t SMALL_OBJECT_SIZE = sizeof(big_vector) / sizeof(uint32_t);

    size_t siz;
    union {
        uint32_t small_data[SMALL_OBJECT_SIZE];
        big_vector data;
    };

private:
    bool is_small() const;
    void to_big();
    void to_small();

public:
    my_vector();
    my_vector(size_t n);
    my_vector(size_t n, uint32_t val);

    my_vector(my_vector const& other);
    my_vector(std::initializer_list<uint32_t> data);

    ~my_vector();

    const uint32_t& operator[](size_t idx) const;
    uint32_t& operator[](size_t idx);

    my_vector& operator=(my_vector const &other);
    void swap(my_vector& other) noexcept;

    void push_back(uint32_t const &val);
    void pop_back();
    const uint32_t& back() const;
    uint32_t& back();

    uint32_t* begin();
    const uint32_t* begin() const;
    uint32_t* end();
    const uint32_t* end() const;

    void resize(size_t n);
    size_t size() const;
    void cut();
};

void swap(my_vector &a, my_vector& b);

#endif //my_vector_H
