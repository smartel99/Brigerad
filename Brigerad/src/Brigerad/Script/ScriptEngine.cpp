/**
 * @file    ScriptEngine
 * @author  Samuel Martel
 * @p       https://github.com/smartel99
 * @date    10/11/2020 4:48:04 PM
 *
 * @brief
 ******************************************************************************
 * Copyright (C) 2020  Samuel Martel
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *****************************************************************************/

/*********************************************************************************************************************/
// [SECTION] Includes
/*********************************************************************************************************************/
#include "brpch.h"
#include "ScriptEngine.h"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#include "lstate.h"
#include "setjmp.h"

#include "ScriptEngineRegistry.h"
#include "Brigerad/Scene/Components.h"

#include "Brigerad/Scene/ScriptableEntity.h"

namespace Brigerad
{
/*********************************************************************************************************************/
// [SECTION] Private Macro Definitions
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Private Variable Definitions
/*********************************************************************************************************************/
struct ScriptEngineData
{
    sol::state* LuaState = nullptr;
};

static ScriptEngineData s_data;

namespace Scripting
{
sol::state* GetState()
{
    return s_data.LuaState;
}
}    // namespace Scripting

static jmp_buf s_luaPanicJump;

/*********************************************************************************************************************/
// [SECTION] Private Function Declarations
/*********************************************************************************************************************/
static int Lua_AtPanicHandler(lua_State* lua)
{
    longjmp(s_luaPanicJump, 1);
    return 0;    // Will never return.
}

static void OnInternalLuaError()
{
    BR_CORE_ERROR("[ScriptEngine] Internal Lua error!");
}

/*********************************************************************************************************************/
// [SECTION] Public Method Definitions
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Private Method Definitions
/*********************************************************************************************************************/


void ScriptEngine::Init()
{
    BR_CORE_INFO("[ScriptEngine] Initializing.");

    s_data.LuaState = new sol::state();
    s_data.LuaState->open_libraries(sol::lib::base, sol::lib::math);

    lua_State* L  = s_data.LuaState->lua_state();
    L->l_G->panic = [](lua_State* L) {
        BR_CORE_CRITICAL("[ScriptEngine] ERROR!!! We should never reach this line!!!");
        return 0;
    };

    lua_atpanic(L, &Lua_AtPanicHandler);

    ScriptEngineRegistry::RegisterAllTypes();
}

void ScriptEngine::Shutdown()
{
    BR_CORE_INFO("[ScriptEngine] Shutting down.");

    delete s_data.LuaState;
    s_data.LuaState = nullptr;
}

#define LUA_CALL(Namespace, Function, ...)                                                         \
    do                                                                                             \
    {                                                                                              \
        if (setjmp(s_luaPanicJump) == 0)                                                           \
        {                                                                                          \
            lua[Namespace][Function](__VA_ARGS__);                                                 \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            OnInternalLuaError();                                                                  \
        }                                                                                          \
    } while (0)

void ScriptEngine::ExecuteScript(const std::string& file)
{
    BR_CORE_INFO("[ScriptEngine] Running {}...", file);

    s_data.LuaState->script_file(file, [](lua_State*, sol::protected_function_result result) {
        BR_CORE_ERROR("[ScriptEngine] Lua error!");
        return result;
    });
}

void ScriptEngine::LoadEntityScript(const std::string& file)
{
    BR_CORE_INFO("[ScriptEngine] Running {}...", file.c_str());

    sol::load_result loadResult = s_data.LuaState->load_file(file);
    if (!loadResult.valid())
    {
        sol::error error = loadResult;
        BR_CORE_ERROR("[ScriptEngine] Lua error! {}", error.what());
    }
    else
    {
        sol::protected_function_result functionResult = loadResult();
        if (!functionResult.valid())
        {
            sol::error error = functionResult;
            BR_CORE_ERROR("[ScriptEngine] Lua error! {}", error.what());
        }
    }
}

void ScriptEngine::OnCreate(LuaScriptEntity* entity)
{
    auto& lua = *s_data.LuaState;
    LUA_CALL(entity->GetPath(), "OnCreate");
}

void ScriptEngine::OnDestroyed(const LuaScriptEntity* entity)
{
    auto& lua = *s_data.LuaState;
    LUA_CALL(entity->GetPath(), "OnDestroyed");
}

void ScriptEngine::OnUpdate(const LuaScriptEntity* entity, float ts)
{
    auto& lua = *s_data.LuaState;
    LUA_CALL(entity->GetPath(), "OnUpdate", ts);
}


/*********************************************************************************************************************/
// [SECTION] Private Function Declarations
/*********************************************************************************************************************/
}    // namespace Brigerad