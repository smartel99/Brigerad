/**
 * @file    Scene
 * @author  Samuel Martel
 * @p       https://github.com/smartel99
 * @date    9/25/2020 3:09:52 PM
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
#include "entt.hpp"

#include "Brigerad/Core/Timestep.h"

/*********************************************************************************************************************/
// [SECTION] Defines
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Class Declarations
/*********************************************************************************************************************/
namespace Brigerad
{

class Entity;

class Scene
{
public:
    Scene();
    ~Scene();

    Entity CreateEntity(const std::string& name = std::string());

    entt::registry& Reg() { return m_registry; }

    void OnUpdate(Timestep ts);

private:
    entt::registry m_registry;

    friend class Entity;
};
}    // namespace Brigerad
