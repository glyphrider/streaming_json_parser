#include "stdafx.h"
#include "DownCase.h"
#include "JsonHandler.h"

CJsonHandler::CJsonHandler():_data(_private)
{
}

CJsonHandler::CJsonHandler(CDataStorage& data):_data(data)
{
}

CJsonHandler::operator CDataStorage&()
{
	return _data;
}

const std::string& CJsonHandler::version() const
{
	return _version;
}

const std::string& CJsonHandler::version(const std::string& version_string)
{
	return _version = version_string;
}

void CJsonHandler::attribute(const std::string& name)
{
	if(_section == associations) _association_count = 0;
	_current_attribute = name;
}

void CJsonHandler::string_value(const std::string& value)
{
	if(_state.size() == 1 && _current_attribute == "version")
		version(value);
	else {
		switch(_section)
		{
		case globals:
			_data.SetGlobalConfig(_global_section_name,_current_attribute,value);
			break;
		case servers:
			if(_current_attribute == "id") _current_server = ConvertToLowerCase(value);
			else if(_current_attribute == "roles") _data.Associate(_current_server,value, (unsigned)_data.ServerCount(value));
			else _data.SetServerConfig(_current_server,_current_attribute,value);
			break;
		case associations:
			_data.Associate(ConvertToLowerCase(value),_current_attribute,_association_count++);
			break;
		}
	}
}

void CJsonHandler::new_object()
{
	if(_state.size() == 1 && _current_attribute == "globals") _section = globals;
	if(_state.size() == 1 && _current_attribute == "associations") _section = associations;
	if((_state.size() == 2) && (_section == globals)) _global_section_name = _current_attribute;
	_state.push(_current_attribute.empty() ? "root" : _current_attribute);
}

void CJsonHandler::end_object()
{
	_current_attribute = _state.top();
	_state.pop();
}

void CJsonHandler::new_array()
{
	if(_state.size() == 1 && _current_attribute == "servers") _section = servers;
	_state.push(_current_attribute.empty() ? "root" : _current_attribute);
}

void CJsonHandler::end_array()
{
	_current_attribute = _state.top();
	_state.pop();
}
