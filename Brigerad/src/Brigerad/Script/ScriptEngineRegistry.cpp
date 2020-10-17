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
#include "Brigerad/Renderer/Renderer.h"
#include "Brigerad/Renderer/Renderer2D.h"


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
    auto lua = Scripting::GetState();

    auto texture2D = lua->new_usertype<Texture2D>(
      "Texture2D",
      "Create",
      [](Texture2D&, const std::string& path) { Texture2D::Create(path); },
      "GetWidth",
      [](Texture2D& self) -> uint32_t { return self.GetWidth(); },
      "GetHeight",
      [](Texture2D& self) -> uint32_t { return self.GetHeight(); },
      "GetFormat",
      [](Texture2D& self) -> uint32_t { return self.GetFormat(); },
      "GetFilePath",
      [](Texture2D& self) -> std::string { return self.GetFilePath(); });
}

void RegisterRenderer()
{
    auto lua = Scripting::GetState();

    // auto renderer = lua->new_usertype<Renderer>("Renderer", sol::no_constructor);
    // renderer["DrawQuad"] =
    //  static_cast<void (*)(const glm::vec2&, const glm::vec2&, const glm::vec4&)>(
    //    &Renderer2D::DrawQuad);
    // renderer["DrawQuadasdf"] =
    //  static_cast<void (*)(const glm::vec3&, const glm::vec2&, const glm::vec4&)>(
    //    &Renderer2D::DrawQuad);
}

}    // namespace Brigerad
