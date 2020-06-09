#pragma once

#include "fly/types/string/detail/string_converter.hpp"
#include "fly/types/string/detail/string_formatter.hpp"
#include "fly/types/string/detail/string_streamer.hpp"
#include "fly/types/string/detail/string_traits.hpp"
#include "fly/types/string/detail/string_unicode.hpp"
#include "fly/types/string/string_literal.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ios>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

namespace fly {

/**
 * Forward declarations of the supported BasicString<> specializations.
 */
template <typename StringType>
class BasicString;

using String = BasicString<std::string>;
using WString = BasicString<std::wstring>;
using String16 = BasicString<std::u16string>;
using String32 = BasicString<std::u32string>;

/**
 * Static class to provide string utilities not provided by the STL.
 *
 * @author Timothy Flynn (trflynn89@pm.me)
 * @version March 21, 2019
 */
template <typename StringType>
class BasicString
{
public:
    // Forward some aliases from detail::BasicStringTraits<> for convenience.
    using traits = detail::BasicStringTraits<StringType>;
    using size_type = typename traits::size_type;
    using char_type = typename traits::char_type;
    using ostream_type = typename traits::ostream_type;
    using streamed_type = typename traits::streamer_type::streamed_type;

    /**
     * Split a string into a vector of strings.
     *
     * @param input The string to split.
     * @param delimiter The delimiter to split the string on.
     *
     * @return A vector containing the split strings.
     */
    static std::vector<StringType> split(const StringType &input, char_type delimiter) noexcept;

    /**
     * Split a string into a vector of strings, up to a maximum size. If the max size is reached,
     * the rest of the string is appended to the last element in the vector.
     *
     * @param input The string to split.
     * @param delimiter The delimiter to split the string on.
     * @param count The maximum return vector size. Zero implies unlimited.
     *
     * @return A vector containing the split strings.
     */
    static std::vector<StringType>
    split(const StringType &input, char_type delimiter, std::uint32_t count) noexcept;

    /**
     * Remove leading and trailing whitespace from a string.
     *
     * @param target The string to trim.
     */
    static void trim(StringType &target) noexcept;

    /**
     * Replace all instances of a substring in a string with a character.
     *
     * @param target The string container which will be modified.
     * @param search The string to search for and replace.
     * @param replace The replacement character.
     */
    static void
    replace_all(StringType &target, const StringType &search, const char_type &replace) noexcept;

    /**
     * Replace all instances of a substring in a string with another string.
     *
     * @param target The string container which will be modified.
     * @param search The string to search for and replace.
     * @param replace The replacement string.
     */
    static void
    replace_all(StringType &target, const StringType &search, const StringType &replace) noexcept;

    /**
     * Remove all instances of a substring in a string.
     *
     * @param target The string container which will be modified.
     * @param search The string to search for and remove.
     */
    static void remove_all(StringType &target, const StringType &search) noexcept;

    /**
     * Check if a string begins with a character.
     *
     * @param source The string to check.
     * @param search The beginning to search for.
     *
     * @return True if the string begins with the search character.
     */
    static bool starts_with(const StringType &source, const char_type &search) noexcept;

    /**
     * Check if a string begins with another string.
     *
     * @param source The string to check.
     * @param search The beginning to search for.
     *
     * @return True if the string begins with the search string.
     */
    static bool starts_with(const StringType &source, const StringType &search) noexcept;

    /**
     * Check if a string ends with a character.
     *
     * @param source The string to check.
     * @param search The ending to search for.
     *
     * @return True if the string ends with the search character.
     */
    static bool ends_with(const StringType &source, const char_type &search) noexcept;

    /**
     * Check if a string ends with another string.
     *
     * @param source The string to check.
     * @param search The ending to search for.
     *
     * @return True if the string ends with the search string.
     */
    static bool ends_with(const StringType &source, const StringType &search) noexcept;

    /**
     * Check if a string matches another string with wildcard expansion.
     *
     * @param source The source string to match against.
     * @param search The wildcard string to search with.
     *
     * @return True if the wildcard string matches the source string.
     */
    static bool wildcard_match(const StringType &source, const StringType &search) noexcept;

    /**
     * Unescape all escaped sequences of unicode characters in a string.
     *
     * Accepts the following unicode encodings, where each character n is a hexadecimal digit:
     *
     *     UTF-8 encodings of the form: \unnnn
     *     UTF-16 paried surrogate encodings of the form: \unnnn\unnnn
     *     UTF-32 encodings of the form: \Unnnnnnnn
     *
     * @param source The string containing the escaped character sequence.
     *
     * @return A copy of the source string with all sequences of unicode characters unescaped.
     *
     * @throws UnicodeException If any escaped sequence is not a valid unicode character.
     */
    static StringType unescape_unicode_string(const StringType &source) noexcept(false);

    /**
     * Unescape a single escaped sequence of unicode characters, starting at the provided iterator.
     * If successful, after invoking this method, that iterator will point at the first character
     * after the escaped sequence in the source string.
     *
     * Accepts the following unicode encodings, where each character n is a hexadecimal digit:
     *
     *     UTF-8 encodings of the form: \unnnn
     *     UTF-16 paried surrogate encodings of the form: \unnnn\unnnn
     *     UTF-32 encodings of the form: \Unnnnnnnn
     *
     * @param it Pointer to the beginning of the escaped character sequence.
     * @param end Pointer to the end of the escaped character sequence.
     *
     * @return A string containing the unescaped unicode character.
     *
     * @throws UnicodeException If the escaped sequence is not a valid unicode character.
     */
    static StringType unescape_unicode_character(
        typename StringType::const_iterator &it,
        const typename StringType::const_iterator &end) noexcept(false);

    /**
     * Generate a random string of the given size.
     *
     * @param size The length of the string to generate.
     *
     * @return The generated string.
     */
    static StringType generate_random_string(size_type size) noexcept;

    /**
     * Format a string with variadic template arguments, returning the formatted string.
     *
     * This is type safe in that argument types need not match the format specifier (i.e. there is
     * no error if %s is given an integer). However, specifiers such as %x are still attempted to be
     * handled. That is, if the matching argument for %x is numeric, then it will be converted to a
     * hexadecimal representation.
     *
     * There is also no checking done on the number of format specifiers and the number of
     * arguments. The format specifiers will be replaced one at a time until all arguments are
     * exhausted, then the rest of the string is taken as-is. Any extra specifiers will be in the
     * string. Any extra arguments are dropped.
     *
     * @tparam Args Variadic template arguments.
     *
     * @param fmt The string to format.
     * @param args The variadic list of arguments to be formatted.
     *
     * @return A string that has been formatted with the given arguments.
     */
    template <typename... Args>
    static streamed_type format(const char_type *fmt, const Args &... args) noexcept;

    /**
     * Format a string with variadic template arguments, inserting the formatted string into a
     * stream.
     *
     * This is type safe in that argument types need not match the format specifier (i.e. there is
     * no error if %s is given an integer). However, specifiers such as %x are still attempted to be
     * handled. That is, if the matching argument for %x is numeric, then it will be converted to a
     * hexadecimal representation.
     *
     * There is also no checking done on the number of format specifiers and the number of
     * arguments. The format specifiers will be replaced one at a time until all arguments are
     * exhausted, then the rest of the string is taken as-is. Any extra specifiers will be in the
     * string. Any extra arguments are dropped.
     *
     * @tparam Args Variadic template arguments.
     *
     * @param ostream The stream to insert the formatted string into.
     * @param fmt The string to format.
     * @param args The variadic list of arguments to be formatted.
     *
     * @return The same stream object.
     */
    template <typename... Args>
    static ostream_type &
    format(ostream_type &ostream, const char_type *fmt, const Args &... args) noexcept;

    /**
     * Concatenate a list of objects with the given separator.
     *
     * @tparam Args Variadic template arguments.
     *
     * @param separator Character to use as a separator.
     * @param args The variadic list of arguments to be joined.
     *
     * @return The resulting join of the given arguments.
     */
    template <typename... Args>
    static streamed_type join(const char_type &separator, const Args &... args) noexcept;

    /**
     * Convert a string to a plain-old-data type, e.g. int or bool.
     *
     * @tparam T The desired plain-old-data type.
     *
     * @param value The string to convert.
     *
     * @return The string coverted to the specified type.
     *
     * @throws std::invalid_argument Conversion could not be performed.
     * @throws std::out_of_range Converted value is out of range of result type.
     */
    template <typename T>
    static T convert(const StringType &value) noexcept(std::is_same_v<StringType, std::decay_t<T>>);

private:
    /**
     * Recursively join one argument into the given ostream.
     */
    template <typename T, typename... Args>
    static void join_internal(
        ostream_type &ostream,
        const char_type &separator,
        const T &value,
        const Args &... args) noexcept;

    /**
     * Terminator for the variadic template joiner. Join the last argument into the given ostream.
     */
    template <typename T>
    static void
    join_internal(ostream_type &ostream, const char_type &separator, const T &value) noexcept;

    /**
     * A list of alpha-numeric characters in the range [0-9A-Za-z].
     */
    static constexpr const char_type *s_alpha_num = FLY_STR(
        char_type,
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz");

    static constexpr std::size_t s_alpha_num_length =
        std::char_traits<char_type>::length(s_alpha_num);
};

//==================================================================================================
template <typename StringType>
std::vector<StringType>
BasicString<StringType>::split(const StringType &input, char_type delimiter) noexcept
{
    return split(input, delimiter, 0);
}

//==================================================================================================
template <typename StringType>
std::vector<StringType> BasicString<StringType>::split(
    const StringType &input,
    char_type delimiter,
    std::uint32_t count) noexcept
{
    std::vector<StringType> elements;
    std::uint32_t num_items = 0;
    StringType item;

    size_type start = 0;
    size_type end = input.find(delimiter);

    auto push_item = [&](const StringType &str) {
        if (!str.empty())
        {
            if ((count > 0) && (++num_items > count))
            {
                elements.back() += delimiter;
                elements.back() += str;
            }
            else
            {
                elements.push_back(str);
            }
        }
    };

    while (end != std::string::npos)
    {
        item = input.substr(start, end - start);
        push_item(item);

        start = end + 1;
        end = input.find(delimiter, start);
    }

    item = input.substr(start, end);
    push_item(item);

    return elements;
}

//==================================================================================================
template <typename StringType>
void BasicString<StringType>::trim(StringType &target) noexcept
{
    auto is_non_space = [](int ch) { return !std::isspace(ch); };

    // Remove leading whitespace.
    target.erase(target.begin(), std::find_if(target.begin(), target.end(), is_non_space));

    // Remove trailing whitespace.
    target.erase(std::find_if(target.rbegin(), target.rend(), is_non_space).base(), target.end());
}

//==================================================================================================
template <typename StringType>
void BasicString<StringType>::replace_all(
    StringType &target,
    const StringType &search,
    const char_type &replace) noexcept
{
    size_type index = target.find(search);

    while (!search.empty() && (index != StringType::npos))
    {
        target.replace(index, search.length(), 1, replace);
        index = target.find(search);
    }
}

//==================================================================================================
template <typename StringType>
void BasicString<StringType>::replace_all(
    StringType &target,
    const StringType &search,
    const StringType &replace) noexcept
{
    size_type index = target.find(search);

    while (!search.empty() && (index != StringType::npos))
    {
        target.replace(index, search.length(), replace);
        index = target.find(search);
    }
}

//==================================================================================================
template <typename StringType>
void BasicString<StringType>::remove_all(StringType &target, const StringType &search) noexcept
{
    replace_all(target, search, StringType());
}

//==================================================================================================
template <typename StringType>
bool BasicString<StringType>::starts_with(
    const StringType &source,
    const char_type &search) noexcept
{
    bool result = false;

    if (!source.empty())
    {
        result = source[0] == search;
    }

    return result;
}

//==================================================================================================
template <typename StringType>
bool BasicString<StringType>::starts_with(
    const StringType &source,
    const StringType &search) noexcept
{
    bool result = false;

    const size_type source_sz = source.length();
    const size_type search_sz = search.length();

    if (source_sz >= search_sz)
    {
        result = source.compare(0, search_sz, search) == 0;
    }

    return result;
}

//==================================================================================================
template <typename StringType>
bool BasicString<StringType>::ends_with(const StringType &source, const char_type &search) noexcept
{
    bool result = false;

    const size_type source_sz = source.length();

    if (source_sz > 0)
    {
        result = source[source_sz - 1] == search;
    }

    return result;
}

//==================================================================================================
template <typename StringType>
bool BasicString<StringType>::ends_with(const StringType &source, const StringType &search) noexcept
{
    bool result = false;

    const size_type source_sz = source.length();
    const size_type search_sz = search.length();

    if (source_sz >= search_sz)
    {
        result = source.compare(source_sz - search_sz, search_sz, search) == 0;
    }

    return result;
}

//==================================================================================================
template <typename StringType>
bool BasicString<StringType>::wildcard_match(
    const StringType &source,
    const StringType &search) noexcept
{
    static constexpr char_type s_wildcard = '*';
    bool result = !search.empty();

    const std::vector<StringType> segments = split(search, s_wildcard);
    size_type index = 0;

    if (!segments.empty())
    {
        if (result && (search.front() != s_wildcard))
        {
            result = starts_with(source, segments.front());
        }
        if (result && (search.back() != s_wildcard))
        {
            result = ends_with(source, segments.back());
        }

        for (auto it = segments.begin(); result && (it != segments.end()); ++it)
        {
            index = source.find(*it, index);

            if (index == StringType::npos)
            {
                result = false;
            }
        }
    }

    return result;
}

//==================================================================================================
template <typename StringType>
StringType
BasicString<StringType>::unescape_unicode_string(const StringType &source) noexcept(false)
{
    StringType result;
    result.reserve(source.size());

    const auto end = source.cend();

    for (auto it = source.cbegin(); it != end;)
    {
        if ((*it == '\\') && ((it + 1) != end))
        {
            switch (*(it + 1))
            {
                case detail::BasicStringUnicode<StringType>::utf8:
                case detail::BasicStringUnicode<StringType>::utf32:
                    result += unescape_unicode_character(it, end);
                    break;

                default:
                    result += *(it++);
                    break;
            }
        }
        else
        {
            result += *(it++);
        }
    }

    return result;
}

//==================================================================================================
template <typename StringType>
StringType BasicString<StringType>::unescape_unicode_character(
    typename StringType::const_iterator &it,
    const typename StringType::const_iterator &end) noexcept(false)
{
    return detail::BasicStringUnicode<StringType>::unescape_character(it, end);
}

//==================================================================================================
template <typename StringType>
StringType BasicString<StringType>::generate_random_string(size_type size) noexcept
{
    using short_distribution = std::uniform_int_distribution<short>;

    constexpr auto limit = static_cast<short_distribution::result_type>(s_alpha_num_length - 1);
    static_assert(limit > 0);

    short_distribution distribution(0, limit);

    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const auto seed = static_cast<std::mt19937::result_type>(now.count());
    std::mt19937 engine(seed);

    StringType result;
    result.reserve(size);

    while (size-- != 0)
    {
        result += s_alpha_num[distribution(engine)];
    }

    return result;
}

//==================================================================================================
template <typename StringType>
template <typename... Args>
auto BasicString<StringType>::format(const char_type *fmt, const Args &... args) noexcept
    -> streamed_type
{
    return detail::BasicStringFormatter<StringType>::format(fmt, args...);
}

//==================================================================================================
template <typename StringType>
template <typename... Args>
auto BasicString<StringType>::format(
    ostream_type &ostream,
    const char_type *fmt,
    const Args &... args) noexcept -> ostream_type &
{
    return detail::BasicStringFormatter<StringType>::format(ostream, fmt, args...);
}

//==================================================================================================
template <typename StringType>
template <typename... Args>
auto BasicString<StringType>::join(const char_type &separator, const Args &... args) noexcept
    -> streamed_type
{
    typename traits::ostringstream_type ostream;
    join_internal(ostream, separator, args...);

    return ostream.str();
}

//==================================================================================================
template <typename StringType>
template <typename T, typename... Args>
void BasicString<StringType>::join_internal(
    ostream_type &ostream,
    const char_type &separator,
    const T &value,
    const Args &... args) noexcept
{
    detail::BasicStringFormatter<StringType>::stream(ostream, value);
    detail::BasicStringFormatter<StringType>::stream(ostream, separator);

    join_internal(ostream, separator, args...);
}

//==================================================================================================
template <typename StringType>
template <typename T>
void BasicString<StringType>::join_internal(
    ostream_type &ostream,
    const char_type &,
    const T &value) noexcept
{
    detail::BasicStringFormatter<StringType>::stream(ostream, value);
}

//==================================================================================================
template <typename StringType>
template <typename T>
T BasicString<StringType>::convert(const StringType &value) noexcept(
    std::is_same_v<StringType, std::decay_t<T>>)
{
    if constexpr (std::is_same_v<StringType, std::decay_t<T>>)
    {
        return value;
    }
    else if constexpr (traits::has_stoi_family_v)
    {
        return detail::BasicStringConverter<StringType, T>::convert(value);
    }
    else
    {
        typename traits::ostringstream_type ostream;
        detail::BasicStringFormatter<StringType>::stream(ostream, value);

        return detail::BasicStringConverter<streamed_type, T>::convert(ostream.str());
    }
}

} // namespace fly
