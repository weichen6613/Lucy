#include <assert.h>
#include <string.h>
#include "state_util.h"


static lucy_Type GetType(lua_State *state, int index)
{
    if (lua_isnumber(state, index)) {
        return lucy_TypeNum;
    } else if (lua_isstring(state, index)) {
        return lucy_TypeStr;
    } else if (lua_isboolean(state, index)) {
        return lucy_TypeBool;
    } else if (lua_isfunction(state, index)) {
        return lucy_TypeFunc;
    } else if (lua_istable(state, index)) {
        return lucy_TypeTbl;
    } else if (lua_isnil(state, index)) {
        return lucy_TypeNil;
    } else {
        DASSERT(false);
        return index;
    }
}


static lucy_Data GetNum(lua_State *state, int index)
{
    lucy_Data data = {lucy_TypeNum};
    data.cntnt_.num_ = lua_tonumber(state, index);
    return data;
}


static lucy_Data GetStr(lua_State *state, int index)
{
    lucy_Data data = {lucy_TypeStr};
    data.cntnt_.str_ = lua_tostring(state, index);
    return data;
}


static lucy_Data GetBool(lua_State *state, int index)
{
    lucy_Data data = {lucy_TypeBool};
    data.cntnt_.boolean_ = lua_toboolean(state, index);
    return data;
}


static lucy_Data GetRef(lua_State *state, int index)
{
    lucy_Data data = {GetType(state, index)};
    lucy_Ref *ref = &(data.cntnt_.ref_);
    ref->state_ = state;
    return data;
}


static lucy_Data GetNil(lua_State *state, int index)
{
    lucy_Data data = {lucy_TypeNil};
    return data;
}


lucy_Data GetLuaDataOnStack(lua_State *state, int index)
{
    static lucy_Data (*GetData[lucy_TypesCount])(lua_State *, int) = {
        &GetNum,
        &GetStr,
        &GetBool,
        &GetRef,
        &GetRef,
        &GetNil
    };

    lucy_Type type = GetType(state, index);
    lucy_Data r = GetData[type](state, index);
    return r;
}


void PopTop(lua_State *state, lucy_Data *data)
{
    if (lucy_ShoudRegister(*data)) {
        data->cntnt_.ref_.index_ = luaL_ref(state,
                                            LUA_REGISTRYINDEX);
    } else {
        lua_pop(state, 1);
    }
}

static void PushNum(lua_State *state, const lucy_Data *luadata)
{
    DASSERT(luadata->type_ == lucy_TypeNum);
    lua_pushnumber(state, luadata->cntnt_.num_);
}


static void PushStr(lua_State *state, const lucy_Data *luadata)
{
    DASSERT(luadata->type_ == lucy_TypeStr);
    lua_pushstring(state, luadata->cntnt_.str_);
}


static void PushBool(lua_State *state, const lucy_Data *luadata)
{
    DASSERT(luadata->type_ == lucy_TypeBool);
    lua_pushboolean(state, luadata->cntnt_.boolean_);
}


static void PushRef(lua_State *state, const lucy_Data *luadata)
{
    DASSERT(luadata->type_ == lucy_TypeFunc ||
            luadata->type_ == lucy_TypeTbl);
    lua_rawgeti(state, LUA_REGISTRYINDEX, lucy_GetIndex(*luadata));
}


static void PushNil(lua_State *state, const lucy_Data *luadata)
{
    DASSERT(luadata->type_ == lucy_TypeNil);
    lua_pushnil(state);
}


void PushData(lua_State *state, const lucy_Data *data)
{
    static void (*Push[lucy_TypesCount])(lua_State *, const lucy_Data *) = {
        PushNum,
        PushStr,
        PushBool,
        PushRef,
        PushRef,
        PushNil
    };

    (*Push[data->type_])(state, data);
}
