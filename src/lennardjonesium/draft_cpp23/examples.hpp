/**
 * Moved generator examples here
 */

#include <lennardjonesium/draft_cpp23/generator.hpp>

/////////////////////////////////////////////////////////////////////////////
// Examples start here

#include <array>
#include <cstdio>
#include <string>
#include <tuple>
#include <vector>

///////////////////////
// Simple non-nested serial generator

static std::generator<const uint64_t> fib(int max) {
    auto a = 0, b = 1;
    for (auto n = 0; n < max; n++) {
        co_yield b;
        const auto next = a + b;
        a = b, b = next;
    }
}

static std::generator<int> other_generator(int i, int j) {
    while (i != j) {
        co_yield i++;
    }
}

/////////////////////
// Demonstrate ability to yield nested sequences
//
// Need to use std::elements_of() to trigger yielding elements of nested
// sequence.
//
// Supports yielding same generator type (with efficient resumption for
// recursive cases)
//
// Also supports yielding any other range whose elements are convertible to
// the current generator's elements.


static std::generator<const uint64_t> nested_sequences_example() {
    std::array<const uint64_t, 5> data = {2, 4, 6, 8, 10};

    std::printf("yielding elements_of std::array\n");
#ifdef SYMMETRIC_TRANSFER
    co_yield std::elements_of(data);
#else
    for (const uint64_t& x : data) {
        co_yield x;
    }
#endif

    std::printf("yielding elements_of nested std::generator\n");
#ifdef SYMMETRIC_TRANSFER
    co_yield std::elements_of(fib(10));
#else
    for (const uint64_t& x : fib(10)) {
        co_yield x;
    }
#endif

    std::printf("yielding elements_of other kind of generator\n");
#ifdef SYMMETRIC_TRANSFER
    co_yield std::elements_of(other_generator(5, 8));
#else
    for (int x : other_generator(5, 8)) {
        co_yield x;
    }
#endif
}



//////////////////////////////////////
// Following examples show difference between:
//
//                                  If I co_yield a...
//                           X / X&&  | X&         | const X&
//                        ------------+------------+-----------
// - generator<X>         move + copy | 2x copy    | 2x copy
// - generator<const X&>     ref      |   ref      |   ref
// - generator<X&&>          ref      | ill-formed | ill-formed
// - generator<X&>         ill-formed |   ref      | ill-formed

struct X {
    int id;
    X(int id) : id(id) {
        std::printf("X::X(%i)\n", id);
    }
    X(const X &x) : id(x.id) {
        std::printf("X::X(copy %i)\n", id);
    }
    X(X &&x) : id(std::exchange(x.id, -1)) {
        std::printf("X::X(move %i)\n", id);
    }
    ~X() {
        std::printf("X::~X(%i)\n", id);
    }
};

static std::generator<X> by_value_example() {
    co_yield X{1};
    {
        X x{2};
        co_yield x; // copy
    }
    {
        const X x{3};
        co_yield x; // copy
    }
    {
        X x{4};
        co_yield std::move(x);
    }
}

static std::generator<X &&> by_rvalue_ref_example() {
    co_yield X{1};
    X x{2};
    // co_yield x;              // ill-formed: lvalue -> rvalue
    co_yield std::move(x);
}

static std::generator<const X &> by_const_ref_example() {
    co_yield X{1}; // OK
    const X x{2};
    co_yield x;            // OK
    co_yield std::move(x); // OK: same as above
}

static std::generator<X &> by_lvalue_ref_example() {
    // co_yield X{1};          // ill-formed: prvalue -> non-const lvalue
    X x{2};
    co_yield x; // OK
    // co_yield std::move(x);  // ill-formed: xvalue -> non-const lvalue
}

///////////////////////////////////
// These examples show different usages of reference/value_type
// template parameters

// reference = std::string_view
// value_type = std::string_view
//
// Use this when the strings are string-literals and won't be invalidated
// by incrementing the iterator.
static std::generator<std::string_view> string_views() {
    co_yield "foo";
    co_yield "bar";
}

// reference = std::string_view
// value_type = std::string
//
// Use this when the string_view would be invalidated by incrementing
// the iterator.
template <typename Allocator>
static std::generator<std::string_view, std::string>
strings(std::allocator_arg_t, Allocator a) {
    co_yield {};
    co_yield "start";
    std::string s;
    for (auto sv : string_views()) {
        s = sv;
        s.push_back('!');
        co_yield s;
    }
    co_yield "end";
}

// Resulting vector is deduced using ::value_type.
template <std::ranges::range R>
static std::vector<std::ranges::range_value_t<R>> to_vector(R &&r) {
    std::vector<std::ranges::range_value_t<R>> v;
    for (decltype(auto) x : r) {
        v.emplace_back(static_cast<decltype(x) &&>(x));
    }
    return v;
}

// zip() algorithm produces a generator of tuples where
// the reference type is a tuple of references and
// the value type is a tuple of values.
template <std::ranges::range... Rs, std::size_t... Indices>
static std::generator<std::tuple<std::ranges::range_reference_t<Rs>...>,
                      std::tuple<std::ranges::range_value_t<Rs>...>>
zip_impl(std::index_sequence<Indices...>, Rs... rs) {
    std::tuple<std::ranges::iterator_t<Rs>...> its{std::ranges::begin(rs)...};
    std::tuple<std::ranges::sentinel_t<Rs>...> itEnds{std::ranges::end(rs)...};
    while (true) {
        const bool anyAtEnd =
            ((std::get<Indices>(its) == std::get<Indices>(itEnds)) || ...);
        if (anyAtEnd)
            break;
        co_yield {*std::get<Indices>(its)...};
        (++std::get<Indices>(its), ...);
    }
}

template <std::ranges::range... Rs>
static std::generator<std::tuple<std::ranges::range_reference_t<Rs>...>,
                      std::tuple<std::ranges::range_value_t<Rs>...>>
zip(Rs &&...rs) {
    return zip_impl(std::index_sequence_for<Rs...>{}, std::forward<Rs>(rs)...);
}

static void print_string_view(std::string_view sv) {
    for (auto c : sv)
        std::putc(c, stdout);
    std::putc('\n', stdout);
}

static void value_type_example() {
    std::vector<std::string_view> s1 = to_vector(string_views());
    for (auto &s : s1)
        print_string_view(s);

    std::printf("\n");

    std::vector<std::string> s2 = to_vector(strings(
        std::allocator_arg,
        std::allocator<std::byte>{}));
    for (auto &s : s2) {
        std::printf("%s\n", s.c_str());
    }

    std::printf("\n");

    std::vector<std::tuple<std::string, std::string>> s3 = to_vector(zip(
        strings(
            std::allocator_arg,
            std::allocator<std::byte>{}),
        strings(std::allocator_arg,
                std::allocator<std::byte>{})));
    for (auto &[a, b] : s3) {
        std::printf("%s, %s\n", a.c_str(), b.c_str());
    }
}

template<typename T>
struct stateful_allocator {
    using value_type = T;

    int id;

    explicit stateful_allocator(int id) noexcept : id(id) {}

    template<typename U>
    stateful_allocator(stateful_allocator<U> x) : id(x.id) {}

    T* allocate(std::size_t count) const {
        std::printf("stateful_allocator(%i).allocate(%zu)\n", id, count);
        return static_cast<T*>(
            ::operator new(count * sizeof(T), std::align_val_t(alignof(T))));
    }

    void deallocate(T* ptr, std::size_t count) {
        std::printf("stateful_allocator(%i).deallocate(%zu)\n", id, count);
        ::operator delete(static_cast<void*>(ptr), count * sizeof(T), std::align_val_t(alignof(T)));
    }
};

std::generator<int, int, std::allocator<std::byte>> stateless_example() {
    co_yield 42;
}

std::generator<int, int, std::allocator<std::byte>> stateless_example(std::allocator_arg_t, std::allocator<std::byte> alloc) {
    co_yield 42;
}

template <typename Allocator>
std::generator<int, int, Allocator> stateful_alloc_example(std::allocator_arg_t, Allocator alloc) {
    co_yield 42;
}

struct member_coro {
    std::generator<int> f() {
        co_yield 42;
    }
};

int main() {
    setbuf(stdout, NULL);

    std::printf("nested_sequences_example\n");
    for (int a : nested_sequences_example()) {
        std::printf("-> %i\n", a);
    }

    std::printf("\nby_value_example\n");

    for (auto &&x : by_value_example()) {
        std::printf("-> %i\n", x.id);
    }

    std::printf("\nby_rvalue_ref_example\n");

    for (auto &&x : by_rvalue_ref_example()) {
        std::printf("-> %i\n", x.id);
    }

    std::printf("\nby_const_ref_example\n");

    for (auto &&x : by_const_ref_example()) {
        std::printf("-> %i\n", x.id);
    }

    std::printf("\nby_lvalue_ref_example\n");

    for (auto &&x : by_lvalue_ref_example()) {
        std::printf("-> %i\n", x.id);
    }

    std::printf("\nvalue_type example\n");

    value_type_example();

    stateful_alloc_example<std::allocator<std::byte>>(std::allocator_arg, {});

    member_coro m;
    *(m.f().begin());
}
/*

int main(int argc, char** argv)
{
   // return *nested_sequences_example().begin();
    return *fib(argc).begin();
}*/
