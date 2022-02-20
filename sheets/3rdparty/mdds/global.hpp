/*************************************************************************
 *
 * Copyright (c) 2008-2020 Kohei Yoshida
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#ifndef INCLUDED_MDDS_GLOBAL_HPP
#define INCLUDED_MDDS_GLOBAL_HPP

#include <exception>
#include <string>
#include <memory>
#include <utility>
#include <type_traits>

/**
 * \def MDDS_ASCII(literal)
 *
 * Expands a \a literal string into two arguments: the first one is the
 * literal string itself, and the second one is the length of that string.
 *
 * Note that this macro only works with literal strings defined inline; it
 * does not work with pointer values that point to strings defined
 * elsewhere.
 */
#define MDDS_ASCII(literal) literal, sizeof(literal)-1

/**
 * \def MDDS_N_ELEMENTS(name)
 *
 * Calculates the length of \a name array provided that the array definition
 * is given in the same compilation unit.
 */
#define MDDS_N_ELEMENTS(name) sizeof(name)/sizeof(name[0])

#ifdef __GNUC__
    #define MDDS_DEPRECATED __attribute__ ((deprecated))
#elif defined(_MSC_VER)
    #define MDDS_DEPRECATED __declspec(deprecated)
#else
    #define MDDS_DEPRECATED
#endif

#ifndef MDDS_LOOP_UNROLLING
#define MDDS_LOOP_UNROLLING 1
#endif

#ifndef MDDS_USE_OPENMP
#define MDDS_USE_OPENMP 0
#endif

#if defined(__AVX__) || defined(__AVX2__)
    #ifndef __SSE2__
        #define __SSE2__ 1
    #endif
#endif

namespace mdds {

class general_error : public ::std::exception
{
public:
    general_error(const ::std::string& msg) : m_msg(msg) {}
    virtual ~general_error() throw() {}

    virtual const char* what() const throw()
    {
        return m_msg.c_str();
    }
private:
    ::std::string m_msg;
};

class invalid_arg_error : public general_error
{
public:
    invalid_arg_error(const ::std::string& msg) : general_error(msg) {}
};

class size_error : public general_error
{
public:
    size_error(const std::string& msg) : general_error(msg) {}
};

class type_error : public general_error
{
public:
    type_error(const std::string& msg) : general_error(msg) {}
};

class integrity_error : public general_error
{
public:
    integrity_error(const std::string& msg) : general_error(msg) {}
};

template<bool B>
using bool_constant = std::integral_constant<bool, B>;

template <typename T>
class has_value_type
{
    using y_type = char;
    using n_type = long;

    template <typename U> static y_type test(typename U::value_type);
    template <typename U> static n_type test(...);

public:
    static constexpr bool value = sizeof(test<T>(0)) == sizeof(y_type);
};

template<typename _T, typename _IsConst>
struct const_or_not;

template<typename _T>
struct const_or_not<_T, std::true_type>
{
    using type = typename std::add_const<_T>::type;
};

template<typename _T>
struct const_or_not<_T, std::false_type>
{
    using type = _T;
};

template<typename _T, bool _Const>
using const_t = typename const_or_not<_T, bool_constant<_Const>>::type;

template<typename _T, typename _IsConst>
struct get_iterator_type;

template<typename _T>
struct get_iterator_type<_T, std::true_type>
{
    using type = typename _T::const_iterator;
};

template<typename _T>
struct get_iterator_type<_T, std::false_type>
{
    using type = typename _T::iterator;
};

template<int T>
constexpr bool invalid_static_int()
{
    return false;
}

}

#endif
