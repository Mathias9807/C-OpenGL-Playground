// g_lua.h - Defines functions used in the lua scripts

#ifndef G_LUA_H
#define G_LUA_H


#include "def.h"
#include "g_main.h"
#include "level.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


extern lua_State* 	G_luaState;
extern prop* 		G_currentProp;

void G_LoadLuaFunctions();


#endif // G_LUA_H
