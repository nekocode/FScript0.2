#pragma once
#include "luawrapper\\LuaContext.h"

class myLua
{
public:
	Lua::LuaContext L;

	myLua();
	~myLua();
	void executeCode(char *code);
};

void fsecho(std::string str,std::string name);