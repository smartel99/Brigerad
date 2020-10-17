/**
 * @file    ScriptableEntity.h
 * @author  Samuel Martel
 * @p       https://github.com/smartel99
 * @date    10/10/2020 2:31:47 PM
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
#include "Entity.h"
#include "Brigerad/Core/Timestep.h"
#include "Brigerad/Script/ScriptEngine.h"


namespace Brigerad
{
/*********************************************************************************************************************/
// [SECTION] Defines
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Class Declarations
/*********************************************************************************************************************/
class ScriptableEntity
{
public:
    ScriptableEntity()          = default;
    virtual ~ScriptableEntity() = default;

    template<typename T>
    T& GetComponentRef()
    {
        return m_entity.GetComponentRef<T>();
    }

    template<typename T>
    const T& GetComponent() const
    {
        return m_entity.GetComponent<T>();
    }


protected:
    virtual void OnCreate() {}
    virtual void OnUpdate(Timestep ts) {}
    virtual void OnDestroy() {}

private:
    Entity m_entity;
    friend class Scene;
};

class LuaScriptEntity
{
public:
    LuaScriptEntity(const std::string& path) : m_path(path)
    {
        ScriptEngine::LoadEntityScript(path);
    }

    virtual ~LuaScriptEntity() = default;

    const std::string& GetPath() const { return m_path; }

    template<typename T>
    T& GetComponentRef()
    {
        return m_entity.GetComponentRef<T>();
    }

    template<typename T>
    const T& GetComponent() const
    {
        return m_entity.GetComponent<T>();
    }

protected:
    void OnCreate() { ScriptEngine::OnCreate(this); }
    void OnUpdate(Timestep ts) { ScriptEngine::OnUpdate(this, ts); }
    void OnDestroy() { ScriptEngine::OnDestroyed(this); }

private:
    Entity      m_entity;
    std::string m_path = "";
    friend class Scene;
};
}    // namespace Brigerad
