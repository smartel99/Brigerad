/**
 ******************************************************************************
 * @addtogroup Layer
 * @{
 * @file    Layer
 * @author  Samuel Martel
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
#include "Brigerad/Core/Core.h"
#include "Brigerad/Events/Event.h"
#include "Brigerad/Core/Timestep.h"

namespace Brigerad
{
/*****************************************************************************/
/* Exported defines */

/*****************************************************************************/
/* Exported macro */

/*****************************************************************************/
/* Exported types */
/**
 * @brief   Layer class, where most of the client's logic should occur.
 *
 * To use it in your client code, define a class that inherits from the layer and then override the
 * functions you need.
 */
class BRIGERAD_API Layer
{
public:
    /**
     * The constructor of Layer.
     * You shouldn't load your assets in here, but instead in the OnAttach method, where the layer
     * is attached to the application.
     *
     * @param   name The name of the layer.
     */
    Layer(const std::string& name = "Layer");
    virtual ~Layer();

    /**
     * @brief   Called when the layer gets attached to the application.
     */
    virtual void OnAttach() {}

    virtual void OnDetach() {}

    virtual void OnUpdate(Timestep timestep) {}

    virtual void OnImGuiRender() {}

    virtual void OnEvent(Event& event) {}

    inline const std::string& GetName() const { return m_debugName; }

protected:
    std::string m_debugName;
};

/*****************************************************************************/
/* Exported functions */

}    // namespace Brigerad
/* Have a wonderful day :) */
#endif /* _Layer */
/**
 * @}
 */
/****** END OF FILE ******/
