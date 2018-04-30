#include <gtest/gtest.h>

#include "fly/parser/exceptions.h"
#include "fly/parser/ini_parser.h"
#include "fly/path/path.h"

//==============================================================================
class IniParserTest : public ::testing::Test
{
public:
    IniParserTest() : m_spParser(std::make_shared<fly::IniParser>())
    {
    }

protected:
    fly::ParserPtr m_spParser;
};

//==============================================================================
TEST_F(IniParserTest, NonExistingPathTest)
{
    ASSERT_NO_THROW(m_spParser->Parse("foo_abc", "abc.ini"));
    EXPECT_EQ(m_spParser->GetValues().Size(), 0);
}

//==============================================================================
TEST_F(IniParserTest, NonExistingFileTest)
{
    ASSERT_NO_THROW(m_spParser->Parse(fly::Path::GetTempDirectory(), "abc.ini"));
    EXPECT_EQ(m_spParser->GetValues().Size(), 0);
}

//==============================================================================
TEST_F(IniParserTest, EmptyFileTest)
{
    const std::string contents;

    ASSERT_NO_THROW(m_spParser->Parse(contents));
    EXPECT_EQ(m_spParser->GetValues().Size(), 0);
}

//==============================================================================
TEST_F(IniParserTest, EmptySectionTest)
{
    const std::string contents("[section]");

    ASSERT_NO_THROW(m_spParser->Parse(contents));
    EXPECT_EQ(m_spParser->GetValues().Size(), 0);
}

//==============================================================================
TEST_F(IniParserTest, NonEmptySectionTest)
{
    const std::string contents(
        "[section]\n"
        "name=John Doe\n"
        "address=USA"
    );

    ASSERT_NO_THROW(m_spParser->Parse(contents));

    EXPECT_EQ(m_spParser->GetValues().Size(), 1);
    EXPECT_EQ(m_spParser->GetValues("section").Size(), 2);
}

//==============================================================================
TEST_F(IniParserTest, NonExistingTest)
{
    const std::string contents(
        "[section]\n"
        "name=John Doe\n"
        "address=USA"
    );

    ASSERT_NO_THROW(m_spParser->Parse(contents));

    EXPECT_EQ(m_spParser->GetValues("section").Size(), 2);
    EXPECT_EQ(m_spParser->GetValues("bad-section").Size(), 0);
    EXPECT_EQ(m_spParser->GetValues("section-bad").Size(), 0);
}

//==============================================================================
TEST_F(IniParserTest, CommentTest)
{
    const std::string contents(
        "[section]\n"
        "name=John Doe\n"
        "; [other-section]\n"
        "; name=Jane Doe\n"
    );

    ASSERT_NO_THROW(m_spParser->Parse(contents));

    EXPECT_EQ(m_spParser->GetValues().Size(), 1);
    EXPECT_EQ(m_spParser->GetValues("section").Size(), 1);
    EXPECT_EQ(m_spParser->GetValues("other-section").Size(), 0);
}

//==============================================================================
TEST_F(IniParserTest, ErrantSpacesTest)
{
    const std::string contents(
        "   [section   ]  \n"
        "\t\t\n   name=John Doe\t  \n"
        "\taddress  = USA\t \r \n"
    );

    ASSERT_NO_THROW(m_spParser->Parse(contents));

    EXPECT_EQ(m_spParser->GetValues().Size(), 1);
    EXPECT_EQ(m_spParser->GetValues("section").Size(), 2);
}

//==============================================================================
TEST_F(IniParserTest, QuotedValueTest)
{
    const std::string contents(
        "[section]\n"
        "name=\"  John Doe  \"\n"
        "address= \t '\\tUSA'"
    );

    ASSERT_NO_THROW(m_spParser->Parse(contents));

    EXPECT_EQ(m_spParser->GetValues().Size(), 1);
    EXPECT_EQ(m_spParser->GetValues("section").Size(), 2);
}

//==============================================================================
TEST_F(IniParserTest, MutlipleSectionTypeTest)
{
    const std::string contents(
        "[section1]\n"
        "name=John Doe\n"
        "age=26\n"
        "[section2]\n"
        "name=Jane Doe\n"
        "age=30.12\n"
        "[section3]\n"
        "name=Joe Doe\n"
        "noage=1\n"
    );

    ASSERT_NO_THROW(m_spParser->Parse(contents));

    EXPECT_EQ(m_spParser->GetValues().Size(), 3);
    EXPECT_EQ(m_spParser->GetValues("section1").Size(), 2);
    EXPECT_EQ(m_spParser->GetValues("section2").Size(), 2);
    EXPECT_EQ(m_spParser->GetValues("section3").Size(), 2);
}

//==============================================================================
TEST_F(IniParserTest, DuplicateSectionTest)
{
    const std::string contents1(
        "[section]\n"
        "name=John Doe\n"
        "[section]\n"
        "name=Jane Doe\n"
    );

    EXPECT_NO_THROW(m_spParser->Parse(contents1));
    EXPECT_EQ(m_spParser->GetValues().Size(), 1);
    EXPECT_EQ(m_spParser->GetValues("section").Size(), 1);
    EXPECT_EQ(m_spParser->GetValues("section")["name"], "Jane Doe");

    const std::string contents2(
        "[  \tsection]\n"
        "name=John Doe\n"
        "[section  ]\n"
        "name=Jane Doe\n"
    );

    EXPECT_NO_THROW(m_spParser->Parse(contents2));
    EXPECT_EQ(m_spParser->GetValues().Size(), 1);
    EXPECT_EQ(m_spParser->GetValues("section").Size(), 1);
    EXPECT_EQ(m_spParser->GetValues("section")["name"], "Jane Doe");
}

//==============================================================================
TEST_F(IniParserTest, DuplicateValueTest)
{
    const std::string contents(
        "[section]\n"
        "name=John Doe\n"
        "name=Jane Doe\n"
    );

    EXPECT_NO_THROW(m_spParser->Parse(contents));
    EXPECT_EQ(m_spParser->GetValues().Size(), 1);
    EXPECT_EQ(m_spParser->GetValues("section").Size(), 1);
    EXPECT_EQ(m_spParser->GetValues("section")["name"], "Jane Doe");
}

//==============================================================================
TEST_F(IniParserTest, ImbalancedBraceTest)
{
    const std::string contents1(
        "[section\n"
        "name=John Doe\n"
    );

    const std::string contents2(
        "section]\n"
        "name=John Doe\n"
    );

    EXPECT_THROW(m_spParser->Parse(contents1), fly::ParserException);
    EXPECT_THROW(m_spParser->Parse(contents2), fly::ParserException);
}

//==============================================================================
TEST_F(IniParserTest, ImbalancedQuoteTest)
{
    const std::string contents1(
        "[section]\n"
        "name=\"John Doe\n"
    );

    const std::string contents2(
        "[section]\n"
        "name=John Doe\"\n"
    );
    const std::string contents3(
        "[section]\n"
        "name='John Doe\n"
    );

    const std::string contents4(
        "[section]\n"
        "name=John Doe'\n"
    );

    const std::string contents5(
        "[section]\n"
        "name=\"John Doe'\n"
    );

    const std::string contents6(
        "[section]\n"
        "name='John Doe\"\n"
    );

    EXPECT_THROW(m_spParser->Parse(contents1), fly::ParserException);
    EXPECT_THROW(m_spParser->Parse(contents2), fly::ParserException);
    EXPECT_THROW(m_spParser->Parse(contents3), fly::ParserException);
    EXPECT_THROW(m_spParser->Parse(contents4), fly::ParserException);
    EXPECT_THROW(m_spParser->Parse(contents5), fly::ParserException);
    EXPECT_THROW(m_spParser->Parse(contents6), fly::ParserException);
}

//==============================================================================
TEST_F(IniParserTest, MisplacedQuoteTest)
{
    const std::string contents1(
        "[section]\n"
        "\"name\"=John Doe\n"
    );

    const std::string contents2(
        "[section]\n"
        "\'name\'=John Doe\n"
    );

    const std::string contents3(
        "[\"section\"]\n"
        "name=John Doe\n"
    );

    const std::string contents4(
        "[\'section\']\n"
        "name=John Doe\n"
    );

    const std::string contents5(
        "\"[section]\"\n"
        "name=John Doe\n"
    );

    const std::string contents6(
        "\'[section]\'\n"
        "name=John Doe\n"
    );

    EXPECT_THROW(m_spParser->Parse(contents1), fly::ParserException);
    EXPECT_THROW(m_spParser->Parse(contents2), fly::ParserException);
    EXPECT_THROW(m_spParser->Parse(contents3), fly::ParserException);
    EXPECT_THROW(m_spParser->Parse(contents4), fly::ParserException);
    EXPECT_THROW(m_spParser->Parse(contents5), fly::ParserException);
    EXPECT_THROW(m_spParser->Parse(contents6), fly::ParserException);
}

//==============================================================================
TEST_F(IniParserTest, MultipleAssignmentTest)
{
    const std::string contents1(
        "[section]\n"
        "name=John=Doe\n"
    );
    const std::string contents2(
        "[section]\n"
        "name=\"John=Doe\"\n"
    );

    ASSERT_NO_THROW(m_spParser->Parse(contents1));
    EXPECT_EQ(m_spParser->GetValues().Size(), 1);
    EXPECT_EQ(m_spParser->GetValues("section").Size(), 1);

    ASSERT_NO_THROW(m_spParser->Parse(contents2));
    EXPECT_EQ(m_spParser->GetValues().Size(), 1);
    EXPECT_EQ(m_spParser->GetValues("section").Size(), 1);
}

//==============================================================================
TEST_F(IniParserTest, MissingAssignmentTest)
{
    const std::string contents1(
        "[section]\n"
        "name\n"
    );

    const std::string contents2(
        "[section]\n"
        "name=\n"
    );

    EXPECT_THROW(m_spParser->Parse(contents1), fly::ParserException);
    EXPECT_THROW(m_spParser->Parse(contents2), fly::ParserException);
}

//==============================================================================
TEST_F(IniParserTest, EarlyAssignmentTest)
{
    const std::string contents1(
        "name=John Doe\n"
        "[section]\n"
    );

    const std::string contents2(
        "name=\n"
        "[section]\n"
    );

    const std::string contents3(
        "name\n"
        "[section]\n"
    );

    EXPECT_THROW(m_spParser->Parse(contents1), fly::ParserException);
    EXPECT_THROW(m_spParser->Parse(contents2), fly::ParserException);
    EXPECT_THROW(m_spParser->Parse(contents3), fly::ParserException);
}

//==============================================================================
TEST_F(IniParserTest, MultipleParseTest)
{
    const std::string contents(
        "[section]\n"
        "name=John Doe\n"
        "address=USA"
    );

    for (int i = 0; i < 5; ++i)
    {
        ASSERT_NO_THROW(m_spParser->Parse(contents));

        EXPECT_EQ(m_spParser->GetValues().Size(), 1);
        EXPECT_EQ(m_spParser->GetValues("section").Size(), 2);
    }
}