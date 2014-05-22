#include <gmock/gmock.h>
#include "json_parser.h"

#define BEGIN_SEQUENCE { ::testing::InSequence _;
#define END_SEQUENCE }

class MockHandler : public IJsonHandler
{
public:
	MOCK_METHOD0(null_value,void());
	MOCK_METHOD1(boolean_value,void(bool value));
	MOCK_METHOD1(string_value,void(const std::string& value));
	MOCK_METHOD0(new_object,void());
	MOCK_METHOD0(end_object,void());
	MOCK_METHOD0(new_array,void());
	MOCK_METHOD0(end_array,void());
	MOCK_METHOD1(attribute,void(const std::string& name));
};

class JsonParserTests : public ::testing::Test
{
protected:
	CJsonParser parser;
};

TEST_F(JsonParserTests,CanCreateParser)
{
}

TEST_F(JsonParserTests,ParserSucceedsForEmptyString)
{
	MockHandler handler;
	// no expectations
	ASSERT_TRUE(parser.parse("",handler));
}

TEST_F(JsonParserTests,ParseEmptyObject)
{
	MockHandler handler;
	EXPECT_CALL(handler,new_object());
	EXPECT_CALL(handler,end_object());
	ASSERT_TRUE(parser.parse("{}",handler));
}

TEST_F(JsonParserTests,ParseEmptyArray)
{
	MockHandler handler;
	EXPECT_CALL(handler,new_array());
	EXPECT_CALL(handler,end_array());
	ASSERT_TRUE(parser.parse("[]",handler));
}

TEST_F(JsonParserTests,ErrorOnUnbalancedArray)
{
	MockHandler handler;
	EXPECT_CALL(handler,new_array());
	ASSERT_FALSE(parser.parse("[",handler));
}

TEST_F(JsonParserTests,ErrorOnUnbalancedClosingArray)
{
	MockHandler handler;
	ASSERT_FALSE(parser.parse("]",handler));
}

TEST_F(JsonParserTests,ParseBoolean_true)
{
	MockHandler handler;
	EXPECT_CALL(handler, boolean_value(true));
	ASSERT_TRUE(parser.parse("true",handler));
}

TEST_F(JsonParserTests,ParseBoolean_tru)
{
	MockHandler handler;
	ASSERT_FALSE(parser.parse("tru",handler));
}

TEST_F(JsonParserTests,ParseBoolean_ture)
{
	MockHandler handler;
	ASSERT_FALSE(parser.parse("ture",handler));
}

TEST_F(JsonParserTests,ParseBoolean_truu)
{
	MockHandler handler;
	ASSERT_FALSE(parser.parse("truu",handler));
}

TEST_F(JsonParserTests,ParseBoolean_false)
{
	MockHandler handler;
	EXPECT_CALL(handler,boolean_value(false));
	ASSERT_TRUE(parser.parse("false",handler));
}

TEST_F(JsonParserTests,ParseNull)
{
	MockHandler handler;
	EXPECT_CALL(handler,null_value());
	ASSERT_TRUE(parser.parse("null",handler));
}

TEST_F(JsonParserTests,CanParseSimpleString)
{
	MockHandler handler;
	EXPECT_CALL(handler,string_value("string"));
	ASSERT_TRUE(parser.parse("\"string\"",handler));
}

TEST_F(JsonParserTests,CantParseUnterminated)
{
	MockHandler handler;
	ASSERT_FALSE(parser.parse("\"string",handler));
}

TEST_F(JsonParserTests,CanParseEscapedQuote)
{
	MockHandler handler;
	EXPECT_CALL(handler,string_value("stri\"ng"));
	ASSERT_TRUE(parser.parse("\"stri\\\"ng\"",handler));
}

TEST_F(JsonParserTests,CanParseEscapedReverseSolidus)
{
	MockHandler handler;
	EXPECT_CALL(handler,string_value("stri\\ng"));
	ASSERT_TRUE(parser.parse("\"stri\\\\ng\"",handler));
}

TEST_F(JsonParserTests,CanParseSingleElementArray)
{
	MockHandler handler;
	EXPECT_CALL(handler,new_array());
	EXPECT_CALL(handler,string_value("element"));
	EXPECT_CALL(handler,end_array());
	ASSERT_TRUE(parser.parse("[\"element\"]",handler));
}

TEST_F(JsonParserTests,CanParseMultiElementArray)
{
	MockHandler handler;
	EXPECT_CALL(handler,new_array());
	EXPECT_CALL(handler,string_value("element1"));
	EXPECT_CALL(handler,string_value("element2"));
	EXPECT_CALL(handler,end_array());
	ASSERT_TRUE(parser.parse("[\"element1\",\"element2\"]",handler));
}

TEST_F(JsonParserTests,CommaOutOfContext)
{
	MockHandler handler;
	EXPECT_CALL(handler,string_value("string1"));
	ASSERT_FALSE(parser.parse("\"string1\",\"string2\"",handler));
}

TEST_F(JsonParserTests,SimpleObjectWithAttributeAndValue)
{
	MockHandler handler;
	EXPECT_CALL(handler,new_object());
	EXPECT_CALL(handler,attribute("attr"));
	EXPECT_CALL(handler,string_value("val"));
	EXPECT_CALL(handler,end_object());
	ASSERT_TRUE(parser.parse("{\"attr\":\"val\"}",handler));

}
TEST_F(JsonParserTests,SimpleObjectWithMultipleAttributesAndValues)
{
	MockHandler handler;
	EXPECT_CALL(handler,new_object());
	EXPECT_CALL(handler,attribute("attr1"));
	EXPECT_CALL(handler,string_value("val1"));
	EXPECT_CALL(handler,attribute("attr2"));
	EXPECT_CALL(handler,string_value("val2"));
	EXPECT_CALL(handler,end_object());
	ASSERT_TRUE(parser.parse("{\"attr1\":\"val1\",\"attr2\":\"val2\"}",handler));

}
TEST_F(JsonParserTests,NestedObjects)
{
	MockHandler handler;
	BEGIN_SEQUENCE
		EXPECT_CALL(handler,new_object());
		EXPECT_CALL(handler,attribute("attr1"));
		EXPECT_CALL(handler,new_object());
		EXPECT_CALL(handler,attribute("attr2"));
		EXPECT_CALL(handler,string_value("val2"));
		EXPECT_CALL(handler,end_object());
		EXPECT_CALL(handler,end_object());
	END_SEQUENCE
	ASSERT_TRUE(parser.parse("{\"attr1\":{\"attr2\":\"val2\"}}",handler));

}

TEST_F(JsonParserTests,ParserSampleServerConfiguration)
{
	MockHandler handler;
	BEGIN_SEQUENCE
		EXPECT_CALL(handler,new_object());
		EXPECT_CALL(handler,attribute("servers"));
		EXPECT_CALL(handler,new_array());
		EXPECT_CALL(handler,new_object());
		EXPECT_CALL(handler,attribute("id"));
		EXPECT_CALL(handler,string_value("servername.example.com"));
		EXPECT_CALL(handler,attribute("roles"));
		EXPECT_CALL(handler,new_array());
		EXPECT_CALL(handler,string_value("Master"));
		EXPECT_CALL(handler,end_array());
		EXPECT_CALL(handler,attribute("config"));
		EXPECT_CALL(handler,new_object());
		EXPECT_CALL(handler,attribute("data_key"));
		EXPECT_CALL(handler,string_value("data_value"));
		EXPECT_CALL(handler,attribute("data_key2"));
		EXPECT_CALL(handler,string_value("data_value2"));
		EXPECT_CALL(handler,end_object());
		EXPECT_CALL(handler,end_object());
		EXPECT_CALL(handler,end_array());
		EXPECT_CALL(handler,end_object());
	END_SEQUENCE
	ASSERT_TRUE(parser.parse("{\"servers\":[{\"id\":\"servername.example.com\",\"roles\":[\"Master\"],\"config\":{\"data_key\":\"data_value\",\"data_key2\":\"data_value2\"}}]}",handler));
}

TEST_F(JsonParserTests,ParserSampleServerConfigurationFailsAfterRoles)
{
	MockHandler handler;
	BEGIN_SEQUENCE
		EXPECT_CALL(handler,new_object());
		EXPECT_CALL(handler,attribute("servers"));
		EXPECT_CALL(handler,new_array());
		EXPECT_CALL(handler,new_object());
		EXPECT_CALL(handler,attribute("id"));
		EXPECT_CALL(handler,string_value("servername.example.com"));
		EXPECT_CALL(handler,attribute("roles"));
		EXPECT_CALL(handler,new_array());
		EXPECT_CALL(handler,string_value("Master"));
		EXPECT_CALL(handler,string_value("config"));
	END_SEQUENCE
	ASSERT_FALSE(parser.parse("{\"servers\":[{\"id\":\"servername.example.com\",\"roles\":[\"Master\",\"config\":{\"data_key\":\"data_value\",\"data_key2\":\"data_value2\"}}]}",handler));
}

int main(int argc, char *argv[])
{
  ::testing::InitGoogleMock(&argc,argv);
  return RUN_ALL_TESTS();
}
