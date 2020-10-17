/**
 * @file    ScriptEngine.h
 * @author  Samuel Martel
 * @p       https://github.com/smartel99
 * @date    10/11/2020 4:44:03 PM
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
#pragma once

/*********************************************************************************************************************/
// [SECTION] Includes
/*********************************************************************************************************************/


#include <string>

namespace Brigerad
{
class LuaScriptEntity;
/*********************************************************************************************************************/
// [SECTION] Defines
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Class Declarations
/*********************************************************************************************************************/
class ScriptEngine
{
public:
    static void Init();
    static void Shutdown();

    static void ExecuteScript(const std::string& file);
    static void LoadEntityScript(const std::string& file);

    // Lua functions to call from C++.
    static void OnCreate(LuaScriptEntity* entity);
    static void OnDestroyed(const LuaScriptEntity* entity);
    static void OnUpdate(const LuaScriptEntity* entity, float ts);
};
}    // namespace Brigerad
