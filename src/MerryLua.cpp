#include "MerryLua.h"
#include "MerryLuaExport.h"
#include "MerryConfig.h"

MerryLua* g_lua = NULL;

MerryLua::MerryLua()
{
	L = luaL_newstate();
	assert(L);
	luaL_openlibs(L);

	lua_register(L, "addCommand",                         LuaAddCommand);
	lua_register(L, "enableCommandKey",                   LuaEnableCommandKey);
	lua_register(L, "disableCommandKey",                  LuaDisableCommandKey);
	lua_register(L, "shellExecute",                       LuaShellExecute);
	lua_register(L, "getForegroundWindow",                LuaGetForegroundWindow);
	lua_register(L, "setForegroundWindow",                LuaSetForegroundWindow);
	lua_register(L, "toggleMerry",                        LuaToggleMerry);
	lua_register(L, "showWindow",                         LuaShowWindow);
	lua_register(L, "closeWindow",                        LuaCloseWindow);
	lua_register(L, "isWindowMax",                        LuaIsWindowMax);
	lua_register(L, "isWindowMin",                        LuaIsWindowMin);
	lua_register(L, "isWindowShown",                      LuaIsWindowShown);
	lua_register(L, "getWindowText",                      LuaGetWindowText);
	lua_register(L, "setWindowText",                      LuaSetWindowText);
	lua_register(L, "getWindowSize",                      LuaGetWindowSize);
	lua_register(L, "setWindowSize",                      LuaSetWindowSize);
	lua_register(L, "getWindowPosition",                  LuaGetWindowPosition);
	lua_register(L, "setWindowPosition",                  LuaSetWindowPosition);
	lua_register(L, "findWindow",                         LuaFindWindow);
	lua_register(L, "getMousePosition",                   LuaGetMousePosition);
	lua_register(L, "setMousePosition",                   LuaSetMousePosition);
	lua_register(L, "message",                            LuaMessage);
	lua_register(L, "enterKey",                           LuaEnterKey);
	lua_register(L, "enterText",                          LuaEnterText);
	lua_register(L, "setTimer",                           LuaSetTimer);
	lua_register(L, "clearTimer",                         LuaClearTimer);

	lua_pushboolean(L, true);
#ifdef __WXMSW__
	lua_setglobal(L, "WINDOWS");
	lua_pushstring(L, "windows");
#elif __WXOSX__
	lua_setglobal(L, "MAC");
	lua_pushstring(L, "mac");
#elif __WXGTK__
	lua_setglobal(L, "GTK");
	lua_pushstring(L, "gtk");
#endif
	lua_setglobal(L, "PLATFORM");

	if (luaL_dofile(L, MERRY_DEFAULT_CONFIG_FILE))
	{
		new MerryInformationDialog(
			wxT("Configure failed"),
			wxString(lua_tostring(L, -1), wxConvLocal)
		);
		lua_pop(L, 1);
	}
}

MerryLua::~MerryLua()
{
	assert(lua_gettop(L) == 0);
	lua_close(L);
}

lua_State* MerryLua::GetLua()
{
	return L;
}

void MerryLua::OnClose()
{
	if (!L)
		return;

	lua_getglobal(L, "onClose");
	if (lua_isnil(L, 1))
	{
		lua_pop(L, 1);
		return;
	}

	if (lua_pcall(L, 0, 0, 0))
	{
		new MerryInformationDialog(
			wxT("Event onClose execute failed"),
			wxString(lua_tostring(L, -1), wxConvLocal)
		);
		lua_pop(L, 1);
	}
}

void MerryLua::OnUndefinedCommand(const wxString& commandName, const wxString& commandArg)
{
	assert(L);

	lua_getglobal(L, "onUndefinedCommand");
	if (lua_isnil(L, 1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushstring(L, commandName.c_str());
	lua_pushstring(L, commandArg.c_str());
	if (lua_pcall(L, 2, 0, 0))
	{
		new MerryInformationDialog(
			wxT("Event onUndefinedCommand execute failed"),
			wxString(lua_tostring(L, -1), wxConvLocal)
		);
		lua_pop(L, 1);
	}
}
