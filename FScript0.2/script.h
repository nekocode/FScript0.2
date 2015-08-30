#pragma once

struct Game
{
	char game_path[MAX_PATH];
	char game_entry[100];
};

class myLua
{
public:
	myLua();
	~myLua();
	void executeCode(const char *code);

private:
	Lua::LuaContext *L;
};

class fscript
{
public:
	fscript();

	bool open(const char *path);
	void dofs();

private:
	FILE *f;
	myLua lua;

	char *buf;
	unsigned long filelen;
};


extern fscript *module_fscript;
extern Lua::LuaContext *FSLua;
extern bool ScriptThreadExit;

void Script_Process();
void fsecho(std::string str,std::string name);