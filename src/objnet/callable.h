#pragma once

#ifndef QT_CORE_LIB
#include "core/bytearray.h"
#else
#include <QByteArray>
typedef QByteArray ByteArray;
#endif
#include <functional>

namespace Objnet
{

using std::string;

template <class T>
inline bool is_deserializable(const ByteArray& b)
{
    return b.size() == int(sizeof(T));
}

// десериализация простых типов
template <class T>
T fromBytes(const ByteArray &b)
{
    T v;
    memcpy(&v, b.data(), sizeof(T));
    return v;
}

// сериализация в ByteArray
template <class T>
ByteArray toBytes(const T &v) {
    return ByteArray(reinterpret_cast<const char*>(&v), sizeof(T));
}

// специализации для нетривиальных типов

template <>
inline bool is_deserializable<ByteArray>(const ByteArray &) {return true;}

template <>
inline ByteArray fromBytes<ByteArray>(const ByteArray &b) { return b; }

template <>
inline ByteArray toBytes<ByteArray>(const ByteArray &v) { return v; }

template <>
inline bool is_deserializable<string>(const ByteArray &) {return true;}

template <>
inline string fromBytes<string>(const ByteArray &b) { return b.toStdString(); }

template <>
inline ByteArray toBytes<string>(const string &v) { return ByteArray::fromStdString(v); }

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
        if (!is_deserializable<P>(in))
            return {};

        if constexpr (std::is_void_v<R>)
        {
            f(fromBytes<P>(in));
            return {};
        }
        else
        {
            return toBytes<R>(f(fromBytes<P>(in)));
        }
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
template <class F, class = void>
struct fn_traits {
    static constexpr bool valid = false;
};

template <class F>
struct fn_traits<F, std::void_t<decltype(&F::operator())>>
    : fn_traits<decltype(&F::operator())> {};

// с аргументом
template <class F, class R, class P> struct fn_traits<R (F::*)(P) const> {
    using R_ = std::decay_t<R>; 
    using P_ = std::decay_t<P>;   // ← decay для аргумента
    static constexpr bool valid = true;
};
template <class F, class R, class P> struct fn_traits<R (F::*)(P)> {
    using R_ = std::decay_t<R>; 
    using P_ = std::decay_t<P>;
    static constexpr bool valid = true;
};

// без аргумента
template <class F, class R> struct fn_traits<R (F::*)() const> {
    using R_ = std::decay_t<R>; 
    using P_ = void;
    static constexpr bool valid = true;
};
template <class F, class R> struct fn_traits<R (F::*)()> {
    using R_ = std::decay_t<R>; 
    using P_ = void;
    static constexpr bool valid = true;
};

// указатель на функцию
template <class R, class P> struct fn_traits<R(*)(P)> {
    using R_ = std::decay_t<R>; 
    using P_ = std::decay_t<P>;
    static constexpr bool valid = true;
};
template <class R> struct fn_traits<R(*)()> {
    using R_ = std::decay_t<R>; 
    using P_ = void;
    static constexpr bool valid = true;
};

template <class T>
inline constexpr bool is_callable_v = fn_traits<std::decay_t<T>>::valid;

}
