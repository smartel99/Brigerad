/**
 * @file    ScriptEngineRegistry
 * @author  Samuel Martel
 * @p       https://github.com/smartel99
 * @date    10/11/2020 4:55:29 PM
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
#include "ScriptEngineRegistry.h"


#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "Brigerad/Renderer/Texture.h"
#include "Brigerad/Renderer/Renderer2D.h"

#include <string>


namespace Brigerad
{
namespace Scripting
{
extern sol::state* GetState();

}
/*********************************************************************************************************************/
// [SECTION] Private Macro Definitions
/*********************************************************************************************************************/



/*********************************************************************************************************************/
// [SECTION] Private Function Declarations
/*********************************************************************************************************************/
static void RegisterTexture2D();
static void RegisterRenderer();

/*********************************************************************************************************************/
// [SECTION] Public Method Definitions
/*********************************************************************************************************************/
void ScriptEngineRegistry::RegisterAllTypes()
{
    RegisterTexture2D();
    RegisterRenderer();
}

/*********************************************************************************************************************/
// [SECTION] Private Method Definitions
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Private Function Declarations
/*********************************************************************************************************************/
void RegisterTexture2D()
{
    // https://github.com/ThePhD/sol2/issues/1008
    auto lua = Scripting::GetState();

    auto texture2D      = lua->new_usertype<Texture2D>("Texture2D", sol::no_constructor);
    texture2D["Create"] = sol::overload(
      static_cast<Ref<Texture2D> (*)(const std::string&)>(&Texture2D::Create),
      static_cast<Ref<Texture2D> (*)(uint32_t, uint32_t, uint8_t)>(&Texture2D::Create));
    // texture2D["Create"] = static_cast<Ref<Texture2D> (*)(const
    // std::string&)>(&Texture2D::Create);
    texture2D["GetWidth"]    = &Texture2D::GetWidth;
    texture2D["GetHeight"]   = &Texture2D::GetHeight;
    texture2D["GetFormat"]   = &Texture2D::GetFormat;
    texture2D["GetFilePath"] = &Texture2D::GetFilePath;
}

void RegisterRenderer()
{
    auto lua = Scripting::GetState();

    auto renderer = lua->new_usertype<Renderer2D>("Renderer2D", sol::no_constructor);
    renderer["DrawQuad"] =
      static_cast<void (*)(const glm::vec2&, const glm::vec2&, const glm::vec4&)>(
        &Renderer2D::DrawQuad);
}

}    // namespace Brigerad
