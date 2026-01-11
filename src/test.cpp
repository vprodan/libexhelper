#include <cassert>
#include <cstdio>
#include <tuple>

extern "C" void **eh_get_exception_ptr();
extern "C" void wrap_caller(const char *c, int i, double f);
extern "C" void wrap_throw_ex(bool b, char c, const char *s);

static const char *test_ex = "test exception";
static bool called = false;
static std::tuple<const char *, int, double> caller_args = {"caller arg", 42, 3.14f};
static std::tuple<bool, char, const char *> throw_ex_args = {true, 'a', "throw_ex arg"};

extern "C" void caller(const char *c, int i, double f)
{
    assert(std::get<0>(caller_args) == c);
    assert(std::get<1>(caller_args) == i);
    assert(std::get<2>(caller_args) == f);
    assert(*eh_get_exception_ptr() == nullptr);
    std::printf("wrapping throw_ex()\n");
    std::apply(wrap_throw_ex, throw_ex_args);
    assert(*eh_get_exception_ptr() != nullptr);
    called = true;
}

extern "C" void throw_ex(bool b, char c, const char *s)
{
    assert(std::get<0>(throw_ex_args) == b);
    assert(std::get<1>(throw_ex_args) == c);
    assert(std::get<2>(throw_ex_args) == s);
    std::printf("throwing: %s\n", test_ex);
    throw test_ex;
}

int main()
{
    assert(!called);
    try
    {
        std::printf("wrapping caller()\n");
        std::apply(wrap_caller, caller_args);
    }
    catch (const char *e)
    {
        std::printf("caught: %s\n", e);
        assert(e == test_ex);
    }
    assert(called);

    std::printf("OK\n");
    return 0;
}
