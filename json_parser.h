#ifndef __json_parser__
#define __json_parser__

#include "json_handler.h"

class CJsonParser
{
typedef enum { array_context, object_context, attribute_context } ContextType;
protected:
	bool eat_string(const std::string& expected, std::string::const_iterator& it, const std::string::const_iterator& end);
	bool can_eat_until(std::string::value_type delim, std::string::const_iterator it, const std::string::const_iterator& end);
	std::string eat_until(std::string::value_type delim, std::string::const_iterator& it, const std::string::const_iterator& end);

public:
	CJsonParser(void);
	virtual ~CJsonParser(void);

	virtual bool parse(const std::string& json, IJsonHandler& handler);
};

#endif
