#pragma once

#include "core/bytearray.h"
#include <functional>

namespace Objnet
{
    
// десериалтизация простых типов
template <class T>
T fromBytes(const ByteArray& b)
{
    T v;
    if (b.size() == int(sizeof(T)))    
        std::memcpy(&v, b.data(), sizeof(T));
    return v;
}

// сериализация в ByteArray
template <class T>
ByteArray toBytes(const T& v) {
    return ByteArray(reinterpret_cast<const char*>(&v), sizeof(T));
}

// специализации для нетривиальных типов

template <>
inline ByteArray fromBytes<ByteArray>(const ByteArray& b) { return b; }

template <>
inline ByteArray toBytes<ByteArray>(const ByteArray& v) { return v; }

template <>
inline string fromBytes<string>(const ByteArray& b) { return b.toStdString(); }

template <>
inline ByteArray toBytes<string>(const string& v) { return ByteArray::fromStdString(v); }

// для void toBytes не нужен — отсекается if constexpr


struct ICallable 
{
    virtual ByteArray call(const ByteArray& in) = 0;
    virtual ~ICallable() = default;
};

// обычный случай: R не void, P не void
template <class R, class P>
struct Callable : ICallable 
{
    std::function<R(P)> f;
    Callable(std::function<R(P)> f) : f(std::move(f)) {}

    ByteArray call(const ByteArray& in) override
    {
        if constexpr (std::is_void_v<R>)
        {
            f(fromBytes<P>(in));
            return {};
        }
        else 
            return toBytes<R>(f(fromBytes<P>(in)));
    }
};

// P = void: std::function<R()>
template <class R>
struct Callable<R, void> : ICallable {
    std::function<R()> f;
    Callable(std::function<R()> f) : f(std::move(f)) {}

    ByteArray call(const ByteArray&) override
    {
        if constexpr (std::is_void_v<R>)
        {
            f();
            return {};
        }
        else
            return toBytes<R>(f());
    }
};

// трейты для лямбд:
template <class F> struct fn_traits : fn_traits<decltype(&F::operator())> {};

// с аргументом
template <class F, class R, class P> struct fn_traits<R (F::*)(P) const> {
    using R_ = R; using P_ = P;
};
template <class F, class R, class P> struct fn_traits<R (F::*)(P)> {
    using R_ = R; using P_ = P;
};

// без аргумента
template <class F, class R> struct fn_traits<R (F::*)() const> {
    using R_ = R; using P_ = void;
};
template <class F, class R> struct fn_traits<R (F::*)()> {
    using R_ = R; using P_ = void;
};

// указатель на функцию
template <class R, class P> struct fn_traits<R(*)(P)> {
    using R_ = R; using P_ = P;
};
template <class R> struct fn_traits<R(*)()> {
    using R_ = R; using P_ = void;
};


}