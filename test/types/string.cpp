#include "fly/types/string/string.hpp"

#include "fly/types/numeric/literals.hpp"

#include "catch2/catch.hpp"

#include <regex>
#include <string>
#include <vector>

namespace {
template <typename StringType>
class Streamable
{
public:
    using ostream_type = typename fly::BasicString<StringType>::ostream_type;

    Streamable(const StringType &str, int num) noexcept : m_str(str), m_num(num)
    {
    }

    StringType str() const
    {
        return m_str;
    };

    int num() const
    {
        return m_num;
    };

    friend ostream_type &operator<<(ostream_type &stream, const Streamable &obj)
    {
        stream << '[';
        stream << obj.str() << ' ' << std::hex << obj.num() << std::dec;
        stream << ']';

        return stream;
    }

private:
    StringType m_str;
    int m_num;
};

struct NotStreamable
{
};

} // namespace

CATCH_TEMPLATE_TEST_CASE(
    "BasicString",
    "[string]",
    std::string,
    std::wstring,
    std::u8string,
    std::u16string,
    std::u32string)
{
    using StringType = TestType;
    using BasicString = fly::BasicString<StringType>;
    using size_type = typename BasicString::size_type;
    using char_type = typename BasicString::char_type;
    using view_type = typename BasicString::view_type;
    using streamed_type = typename BasicString::streamed_type;
    using streamed_char = typename streamed_type::value_type;

    CATCH_SECTION("Get the size of a string-like type")
    {
        const char_type *cstr = FLY_STR(char_type, "ten chars!");
        StringType str = cstr;
        view_type view = str;

        CATCH_CHECK(BasicString::size(cstr) == 10);
        CATCH_CHECK(BasicString::size(str) == 10);
        CATCH_CHECK(BasicString::size(view) == 10);
    }

    CATCH_SECTION("Split a string by a character delimeter")
    {
        static constexpr size_type s_size = 10;
        std::vector<StringType> input_split(s_size);

        constexpr char_type delim = ' ';
        StringType input;

        for (size_type i = 0; i < s_size; ++i)
        {
            const StringType curr = BasicString::generate_random_string(10);

            input += curr + delim;
            input_split[i] = curr;
        }

        const auto output_split = BasicString::split(input, delim);
        CATCH_CHECK(input_split.size() == output_split.size());

        for (size_type i = 0; i < s_size; ++i)
        {
            CATCH_CHECK(input_split[i] == output_split[i]);
        }
    }

    CATCH_SECTION("Split a string by a character delimeter, with a maximum number of results")
    {
        static constexpr size_type s_size = 10;
        static constexpr size_type s_count = 6;
        std::vector<StringType> input_split(s_count);

        constexpr char_type delim = ';';
        StringType input;

        for (size_type i = 0; i < s_size; ++i)
        {
            const StringType curr = BasicString::generate_random_string(10);
            input += curr + delim;

            if (i < s_count)
            {
                input_split[i] = curr;
            }
            else
            {
                input_split.back() += delim;
                input_split.back() += curr;
            }
        }

        const auto output_split = BasicString::split(input, delim, s_count);
        CATCH_CHECK(input_split.size() == output_split.size());

        for (size_type i = 0; i < s_count; ++i)
        {
            CATCH_CHECK(input_split[i] == output_split[i]);
        }
    }

    CATCH_SECTION("Trim whitespace from a string")
    {
        StringType test1;
        StringType test2 = FLY_STR(char_type, "   abc");
        StringType test3 = FLY_STR(char_type, "abc   ");
        StringType test4 = FLY_STR(char_type, "   abc   ");
        StringType test5 = FLY_STR(char_type, " \n\t\r  abc  \n\t\r ");
        StringType test6 = FLY_STR(char_type, " \n\t\r  a   c  \n\t\r ");
        StringType test7 = FLY_STR(char_type, " \n\t\r  a\n \tc  \n\t\r ");

        const StringType expected1;
        const StringType expected2 = FLY_STR(char_type, "abc");
        const StringType expected3 = FLY_STR(char_type, "a   c");
        const StringType expected4 = FLY_STR(char_type, "a\n \tc");

        BasicString::trim(test1);
        BasicString::trim(test2);
        BasicString::trim(test3);
        BasicString::trim(test4);
        BasicString::trim(test5);
        BasicString::trim(test6);
        BasicString::trim(test7);

        CATCH_CHECK(test1 == expected1);
        CATCH_CHECK(test2 == expected2);
        CATCH_CHECK(test3 == expected2);
        CATCH_CHECK(test4 == expected2);
        CATCH_CHECK(test5 == expected2);
        CATCH_CHECK(test6 == expected3);
        CATCH_CHECK(test7 == expected4);
    }

    CATCH_SECTION("Replace all instances of a substring in a string with a character")
    {
        StringType source = FLY_STR(char_type, "To Be Replaced! To Be Replaced!");
        const StringType search = FLY_STR(char_type, "Be Replaced");
        const char_type replace('x');

        BasicString::replace_all(source, search, replace);
        CATCH_CHECK(source == FLY_STR(char_type, "To x! To x!"));
    }

    CATCH_SECTION("Replace all instances of a substring in a string with another string")
    {
        StringType source = FLY_STR(char_type, "To Be Replaced! To Be Replaced!");
        const StringType search = FLY_STR(char_type, "Be Replaced");
        const StringType replace = FLY_STR(char_type, "new value");

        BasicString::replace_all(source, search, replace);
        CATCH_CHECK(source == FLY_STR(char_type, "To new value! To new value!"));
    }

    CATCH_SECTION("Replace all instances of a substring in a string with an empty string")
    {
        StringType source = FLY_STR(char_type, "To Be Replaced! To Be Replaced!");
        const StringType replace = FLY_STR(char_type, "new value");

        BasicString::replace_all(source, StringType(), replace);
        CATCH_CHECK(source == FLY_STR(char_type, "To Be Replaced! To Be Replaced!"));
    }

    CATCH_SECTION("Remove all instances of a substring in a string")
    {
        StringType source = FLY_STR(char_type, "To Be Replaced! To Be Replaced!");
        const StringType search = FLY_STR(char_type, "Be Rep");

        BasicString::remove_all(source, search);
        CATCH_CHECK(source == FLY_STR(char_type, "To laced! To laced!"));
    }

    CATCH_SECTION("Remove all instances of an empty string in a string")
    {
        StringType source = FLY_STR(char_type, "To Be Replaced! To Be Replaced!");

        BasicString::remove_all(source, StringType());
        CATCH_CHECK(source == FLY_STR(char_type, "To Be Replaced! To Be Replaced!"));
    }

    CATCH_SECTION("Check if a string begins with a character")
    {
        StringType test1, test2;

        test1 = FLY_STR(char_type, "abc");
        CATCH_CHECK(BasicString::starts_with(test1, 'a'));

        test1 = FLY_STR(char_type, "");
        CATCH_CHECK_FALSE(BasicString::starts_with(test1, 'a'));

        test1 = FLY_STR(char_type, "b");
        CATCH_CHECK_FALSE(BasicString::starts_with(test1, 'a'));
    }

    CATCH_SECTION("Check if a string begins with another string")
    {
        StringType test1, test2;

        test1 = FLY_STR(char_type, "");
        test2 = FLY_STR(char_type, "");
        CATCH_CHECK(BasicString::starts_with(test1, test2));

        test1 = FLY_STR(char_type, "a");
        test2 = FLY_STR(char_type, "");
        CATCH_CHECK(BasicString::starts_with(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "a");
        CATCH_CHECK(BasicString::starts_with(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "ab");
        CATCH_CHECK(BasicString::starts_with(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "abc");
        CATCH_CHECK(BasicString::starts_with(test1, test2));

        test1 = FLY_STR(char_type, "");
        test2 = FLY_STR(char_type, "a");
        CATCH_CHECK_FALSE(BasicString::starts_with(test1, test2));

        test1 = FLY_STR(char_type, "a");
        test2 = FLY_STR(char_type, "ab");
        CATCH_CHECK_FALSE(BasicString::starts_with(test1, test2));

        test1 = FLY_STR(char_type, "ab");
        test2 = FLY_STR(char_type, "abc");
        CATCH_CHECK_FALSE(BasicString::starts_with(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "abd");
        CATCH_CHECK_FALSE(BasicString::starts_with(test1, test2));
    }

    CATCH_SECTION("Check if a string ends with a character")
    {
        StringType test1, test2;

        test1 = FLY_STR(char_type, "abc");
        CATCH_CHECK(BasicString::ends_with(test1, 'c'));

        test1 = FLY_STR(char_type, "");
        CATCH_CHECK_FALSE(BasicString::ends_with(test1, 'a'));

        test1 = FLY_STR(char_type, "ab");
        CATCH_CHECK_FALSE(BasicString::ends_with(test1, 'a'));
    }

    CATCH_SECTION("Check if a string ends with another string")
    {
        StringType test1, test2;

        test1 = FLY_STR(char_type, "");
        test2 = FLY_STR(char_type, "");
        CATCH_CHECK(BasicString::ends_with(test1, test2));

        test1 = FLY_STR(char_type, "a");
        test2 = FLY_STR(char_type, "");
        CATCH_CHECK(BasicString::ends_with(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "c");
        CATCH_CHECK(BasicString::ends_with(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "bc");
        CATCH_CHECK(BasicString::ends_with(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "abc");
        CATCH_CHECK(BasicString::ends_with(test1, test2));

        test1 = FLY_STR(char_type, "");
        test2 = FLY_STR(char_type, "a");
        CATCH_CHECK_FALSE(BasicString::ends_with(test1, test2));

        test1 = FLY_STR(char_type, "a");
        test2 = FLY_STR(char_type, "ba");
        CATCH_CHECK_FALSE(BasicString::ends_with(test1, test2));

        test1 = FLY_STR(char_type, "ab");
        test2 = FLY_STR(char_type, "a");
        CATCH_CHECK_FALSE(BasicString::ends_with(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "dbc");
        CATCH_CHECK_FALSE(BasicString::ends_with(test1, test2));
    }

    CATCH_SECTION("Check if a wildcarded string matches another string")
    {
        StringType test1, test2;

        test1 = FLY_STR(char_type, "");
        test2 = FLY_STR(char_type, "*");
        CATCH_CHECK(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "");
        test2 = FLY_STR(char_type, "**");
        CATCH_CHECK(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "a");
        test2 = FLY_STR(char_type, "a");
        CATCH_CHECK(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "b");
        test2 = FLY_STR(char_type, "*");
        CATCH_CHECK(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "c");
        test2 = FLY_STR(char_type, "**");
        CATCH_CHECK(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "a*");
        CATCH_CHECK(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "a*c");
        CATCH_CHECK(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "a*");
        CATCH_CHECK(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "*b*");
        CATCH_CHECK(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "*bc");
        CATCH_CHECK(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "*c");
        CATCH_CHECK(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "");
        test2 = FLY_STR(char_type, "");
        CATCH_CHECK_FALSE(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "a");
        test2 = FLY_STR(char_type, "");
        CATCH_CHECK_FALSE(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "a");
        test2 = FLY_STR(char_type, "b");
        CATCH_CHECK_FALSE(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "a");
        test2 = FLY_STR(char_type, "b*");
        CATCH_CHECK_FALSE(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "a");
        test2 = FLY_STR(char_type, "*b");
        CATCH_CHECK_FALSE(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "a");
        CATCH_CHECK_FALSE(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "b*");
        CATCH_CHECK_FALSE(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "*b");
        CATCH_CHECK_FALSE(BasicString::wildcard_match(test1, test2));

        test1 = FLY_STR(char_type, "abc");
        test2 = FLY_STR(char_type, "*d*");
        CATCH_CHECK_FALSE(BasicString::wildcard_match(test1, test2));
    }

    CATCH_SECTION("Generate a random string with a specific size")
    {
        static constexpr size_type s_size = (1 << 10);

        const auto random = BasicString::generate_random_string(s_size);
        CATCH_CHECK(s_size == random.size());
    }

    CATCH_SECTION("Join generic types by a delimeter")
    {
        StringType str = FLY_STR(char_type, "a");
        const char_type *ctr = FLY_STR(char_type, "b");
        const char_type arr[] = {'c', '\0'};
        const char_type chr = 'd';

        const Streamable<streamed_type> obj1(FLY_STR(streamed_char, "hi"), 0xbeef);
        const NotStreamable obj2;

        CATCH_CHECK(FLY_STR(streamed_char, "a") == BasicString::join('.', str));
        CATCH_CHECK(FLY_STR(streamed_char, "b") == BasicString::join('.', ctr));
        CATCH_CHECK(FLY_STR(streamed_char, "c") == BasicString::join('.', arr));
        CATCH_CHECK(FLY_STR(streamed_char, "d") == BasicString::join('.', chr));
        CATCH_CHECK(FLY_STR(streamed_char, "a,a") == BasicString::join(',', str, str));
        CATCH_CHECK(FLY_STR(streamed_char, "a,b") == BasicString::join(',', str, ctr));
        CATCH_CHECK(FLY_STR(streamed_char, "a,c") == BasicString::join(',', str, arr));
        CATCH_CHECK(FLY_STR(streamed_char, "a,d") == BasicString::join(',', str, chr));
        CATCH_CHECK(FLY_STR(streamed_char, "b,a") == BasicString::join(',', ctr, str));
        CATCH_CHECK(FLY_STR(streamed_char, "b,b") == BasicString::join(',', ctr, ctr));
        CATCH_CHECK(FLY_STR(streamed_char, "b,c") == BasicString::join(',', ctr, arr));
        CATCH_CHECK(FLY_STR(streamed_char, "b,d") == BasicString::join(',', ctr, chr));
        CATCH_CHECK(FLY_STR(streamed_char, "c,a") == BasicString::join(',', arr, str));
        CATCH_CHECK(FLY_STR(streamed_char, "c,b") == BasicString::join(',', arr, ctr));
        CATCH_CHECK(FLY_STR(streamed_char, "c,c") == BasicString::join(',', arr, arr));
        CATCH_CHECK(FLY_STR(streamed_char, "c,d") == BasicString::join(',', arr, chr));
        CATCH_CHECK(FLY_STR(streamed_char, "d,a") == BasicString::join(',', chr, str));
        CATCH_CHECK(FLY_STR(streamed_char, "d,b") == BasicString::join(',', chr, ctr));
        CATCH_CHECK(FLY_STR(streamed_char, "d,c") == BasicString::join(',', chr, arr));
        CATCH_CHECK(FLY_STR(streamed_char, "d,d") == BasicString::join(',', chr, chr));
        CATCH_CHECK(FLY_STR(streamed_char, "[hi beef]") == BasicString::join('.', obj1));
        CATCH_CHECK(
            FLY_STR(streamed_char, "a:[hi beef]:c:d") ==
            BasicString::join(':', str, obj1, arr, chr));
        CATCH_CHECK(FLY_STR(streamed_char, "a:c:d") == BasicString::join(':', str, arr, chr));

        std::basic_regex<streamed_char> test(
            FLY_STR(streamed_char, "\\[(0x)?[0-9a-fA-F]+\\]:2:\\[hi beef\\]"));
        CATCH_CHECK(std::regex_match(BasicString::join(':', obj2, 2, obj1), test));
    }
}
