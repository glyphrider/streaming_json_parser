#include "json_parser.h"
#include <stack>

CJsonParser::CJsonParser(void)
{
}

CJsonParser::~CJsonParser(void)
{
}

bool CJsonParser::parse(const std::string& json, IJsonHandler& handler)
{
	std::stack<ContextType> context;

	for(std::string::const_iterator it = json.begin(); it != json.end(); it++)
	{
		switch(*it)
		{
		case '{':
			context.push(object_context);
			handler.new_object();
			break;
		case '}':
			if(context.empty() || context.top() != object_context) return false;
			context.pop();
			handler.end_object();
			if(context.size() && context.top() == attribute_context) context.pop();
			break;
		case '[':
			context.push(array_context);
			handler.new_array();
			break;
		case ']':
			if(context.empty() || context.top() != array_context) return false;
			context.pop();
			handler.end_array();
			if(context.size() && context.top() == attribute_context) context.pop();
			break;
		case 't':
			if(!eat_string("true",it,json.end())) return false;
			handler.boolean_value(true);
			if(context.size() && context.top() == attribute_context) context.pop();
			break;
		case 'f':
			if(!eat_string("false",it,json.end())) return false;
			handler.boolean_value(false);
			if(context.size() && context.top() == attribute_context) context.pop();
			break;
		case 'n':
			if(!eat_string("null",it,json.end())) return false;
			handler.null_value();
			if(context.size() && context.top() == attribute_context) context.pop();
			break;
		case '"':
			if(!can_eat_until('"',it,json.end())) return false;
			if(context.size() && context.top() == object_context)
			{
				handler.attribute(eat_until('"',it,json.end()));
				context.push(attribute_context);
			} else {
				if(context.size() && context.top() == attribute_context) context.pop();
				handler.string_value(eat_until('"',it,json.end()));
			}
			break;
		case ',':
			if(context.empty() || context.top() == attribute_context) return false;
			break;
		case ':':
			if(context.empty() || context.top() != attribute_context) return false;
			break;
		}
	}
	return context.empty();
}

bool CJsonParser::eat_string(const std::string& expected, std::string::const_iterator& it, const std::string::const_iterator& end)
{
	for(std::string::const_iterator next = expected.begin(); next != expected.end(); next++, it++)
	{
		if(it == end) return false;
		if(*it != *next) return false;
	}
	it--;
	return true;
}

bool CJsonParser::can_eat_until(std::string::value_type delim, std::string::const_iterator it, const std::string::const_iterator& end)
{
	bool isEscaped = false;
	while(1)
	{
		it++;
		if(it == end) return false;
		if(*it == '\\') 
		{
			isEscaped = true;
			continue;
		}
		if(isEscaped) isEscaped = false;
		else if(*it == delim) return true;
	}
}

std::string CJsonParser::eat_until(std::string::value_type delim, std::string::const_iterator& it, const std::string::const_iterator& end)
{
	std::string value;
	bool isEscaped = false;
	while(it != end)
	{
		it++;
		if(*it == '\\' && !isEscaped)
		{
			isEscaped = true;
			continue;
		}
		if(isEscaped) isEscaped = false;
		else if(*it == delim) break;
		value += *it;
	}
	return value; 
}
