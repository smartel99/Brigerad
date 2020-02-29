﻿/**
 ******************************************************************************
 * @addtogroup Layer
 * @{
 * @file    Layer
 * @author  Client Microdata
 * @brief   Header for the Layer module.
 *
 * @date 2/29/2020 12:45:33 PM
 *
 ******************************************************************************
 */
#ifndef _Layer
#define _Layer

/*****************************************************************************/
/* Includes */
#include "Brigerad/Core.h"
#include "Brigerad/Events/Event.h"


namespace Brigerad
{
/*****************************************************************************/
/* Exported defines */


/*****************************************************************************/
/* Exported macro */


/*****************************************************************************/
/* Exported types */
class BRIGERAD_API Layer
{
public:
    Layer(const std::string& name = "Layer");
    virtual ~Layer();

    virtual void OnAttach()
    {
    }

    virtual void OnDetach()
    {
    }

    virtual void OnUpdate()
    {
    }

    virtual void OnEvent(Event& event)
    {
    }

    inline const std::string& GetName() const
    {
        return m_debugName;
    }

protected:
    std::string m_debugName;
};

/*****************************************************************************/
/* Exported functions */


}
/* Have a wonderful day :) */
#endif /* _Layer */
/**
 * @}
 */
/****** END OF FILE ******/
