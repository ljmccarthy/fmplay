#ifndef util_hpp_included__
#define util_hpp_included__

#include <cstddef>

template<typename T, std::size_t n>
constexpr std::size_t array_size(const T (&)[n])
{
    return n;
}

class noncopyable {
protected:
    constexpr noncopyable() = default;
    ~noncopyable() = default;
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};

#endif // util_hpp_included__
