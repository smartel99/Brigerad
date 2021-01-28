/**
 * @file Application.h
 * @author Samuel Martel (martelsamuel00@gmail.com)
 * @brief   Header for the Application module.
 * @version 0.1
 * @date    2021-01-02
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once

#include "Brigerad/Core/Core.h"
#include "Brigerad/Events/Event.h"
#include "Brigerad/Core/LayerStack.h"
#include "Brigerad/Events/ApplicationEvent.h"

#include "Brigerad/Core/Window.h"

#include "Brigerad/ImGui/ImGuiLayer.h"

#include "Brigerad/Core/Timestep.h"

#include "Brigerad/Renderer/Shader.h"
#include "Brigerad/Renderer/Buffer.h"
#include "Brigerad/Renderer/VertexArray.h"
#include "Brigerad/Renderer/Renderer.h"
#include "Brigerad/Renderer/OrthographicCamera.h"

namespace Brigerad
{
/**
 * @brief Base class for the client application using the Brigerad engine.\n
 *
 * All client application class must inherit from this class.
 */
class BRIGERAD_API Application
{
public:
    /**
     * @brief Default constructor of the Application.\n
     * This creates a new window and binds the event functions to it.
     *
     * @param name The name of the
     * application. This is the name that is shown in the top bar of the window.
     */
    Application(const std::string& name = "Brigerad Engine");

    /**
     * @brief   Destroy the Application:: Application object.
     */
    virtual ~Application();

    /**
     * @brief Where all of the magic happens.
     * During normal execution, this function never returns, for as long as the process window is
     * open.
     *
     * It iterates through all of the layers and ImGui layers contained in the application,
     * handles the refreshing of window events (key press, mouse events, window closed/minimized,
     * etc.), as well as handling the frame creation/rendering for ImGui.
     *
     */
    void Run();

    /**
     * @brief Callback function for all events happening in the application.
     *        It dispatches and propagates the event through all layers until it is handled.
     *
     * @par How to use this
     * @code
     * // A button has been released.
     * button.state = ImGuiButtonState::Released;
     *
     * // Create the event object.
     * ImGuiButtonReleasedEvent e(entity);
     *
     * // Send the event to the application.
     * * Application::Get().OnEvent(e);
     * @endcode
     *
     * @param e The event
     * itself.
     */
    void OnEvent(Event& e);

    /**
     * @brief   Push a new layer at the back of the layer stack.
     *
     * @param   layer A pointer to the layer
     */
    void PushLayer(Layer* layer);
    /**
     * @brief   Push a new layer at the front of the layer stack.
     *
     * @param   layer A pointer to the layer
     */
    void PushOverlay(Layer* layer);

    /**
     * @brief Queue a layer to be removed from the application at the end of this frame.
     *
     * @param layer A pointer to the layer to remove
     *
     * @attention If @ref layer is NULL, this function will assert false.
     */
    void PopLayer(Layer* layer);

    /**
     * Mark the application as closed, which will cause the process to be terminated at the end of
     * this frame.
     */
    void Close();

    /**
     * @brief Get a reference to the Application's window handle.
     *
     * @retval A reference to the window handle.
     */
    inline Window& GetWindow() { return *m_window; }

    /**
     * @brief Get a pointer to the Application's ImGui layer.
     *
     * The only reason why you would want to get the ImGui layer is to set the block event flag.
     *
     * @retval A pointer to the ImGui Layer.
     */
    inline ImGuiLayer* GetImGuiLayer() { return m_imguiLayer; }

    /**
     * @brief Queue a task to be executed at the end of the current frame.
     *
     * This could be tasks such as removing a layer from another layer, which would cause the
     * application to crash if it is done during the run loop.
     *
     * @param   fn The task to execute.
     */
    inline void QueuePostFrameTask(const std::function<void()>& fn)
    {
        if (fn)
        {
            m_postFrameTasks.push_back(fn);
        }
    }

    inline static Application& Get() { return *s_instance; }

private:
    /**
     * @brief   Handle the window close event.
     *
     * This event happens whenever the main application window closes.
     *
     * @param   e The event
     * @return  Always returns true.
     */
    bool OnWindowClose(WindowCloseEvent& e);
    /**
     * @brief   Handle the window resize event.
     *
     * @param   e The event
     * @return  Always returns false.
     */
    bool OnWindowResize(WindowResizeEvent& e);
    /**
     * @brief   Handle the key pressed event, which mainly consists of opening or closing the
     *          profiling window.
     *
     * @param   e The event
     * @retval  true If the key pressed if the escape key.
     * @retval  false If any other key is pressed.
     *
     */
    bool OnKeyPressed(KeyPressedEvent& e);

private:
    /**
     * The application's window context.
     */
    Scope<Window> m_window;
    /**
     * The application's main ImGui layer.
     */
    ImGuiLayer* m_imguiLayer;

    /**
     * Flag indicating if the application should keep running or close.
     */
    bool m_running = true;
    /**
     * Flag indicating if the application is minimized or active.
     */
    bool m_minimized = false;

    /**
     * The application's stack of layers.
     */
    LayerStack m_layerStack;

    /**
     * Process time of the last frame.
     */
    float m_lastFrameTime = 0.0f;

    /**
     * Queue of the tasks to execute at the end of this frame.
     */
    std::vector<std::function<void()>> m_postFrameTasks;

private:
    /**
     * @brief   Singleton instance of the running application.
     */
    static Application* s_instance;
};

// To be defined in client.
Application* CreateApplication();
}    // namespace Brigerad
