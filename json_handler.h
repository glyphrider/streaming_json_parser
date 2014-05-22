#ifndef __json_handler__
#define __json_handler__

#include <string>
#include <stack>
#include <algorithm>

class IJsonHandler
{
public:
	virtual void null_value() = 0;
	virtual void boolean_value(bool value) = 0;
	virtual void string_value(const std::string& value) = 0;
	virtual void new_object() = 0;
	virtual void end_object() = 0;
	virtual void new_array() = 0;
	virtual void end_array() = 0;
	virtual void attribute(const std::string& name) = 0;
};

class CJsonHandlerBase : public IJsonHandler
{
public:
    virtual void null_value() {}
    virtual void boolean_value(bool value) {}
    virtual void string_value(const std::string& value) {}
    virtual void new_object() {}
    virtual void end_object() {}
    virtual void new_array() {}
    virtual void end_array() {}
    virtual void attribute(const std::string& name) {}
};

#endif
