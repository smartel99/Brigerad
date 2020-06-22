/**
 * @file Application.cpp
 * @author Samuel Martel (martelsamuel00@gmail.com)
 * @brief   Header for the Application module.
 * @version 0.1
 * @date    2020-05-14
 *
 * @copyright Copyright (c) 2020
 *
 */
#include "brpch.h"
#include "Application.h"

#include "Input.h"
#include "Brigerad/Core/Time.h"
#include "KeyCodes.h"

namespace Brigerad
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

/**
 * @brief   Singleton instance of the running application
 *
 */
Application* Application::s_instance = nullptr;

/**
 * @brief   Construct a new Application:: Application object
 *          This creates a new window and binds the event function to it.
 */
Application::Application(const std::string& name)
{
    BR_PROFILE_FUNCTION();

    // Don't allow multiple instances of Application.
    BR_CORE_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;

    // Create the window for the application.
    m_window = Scope<Window>(Window::Create(WindowProps(name)));
    // Bind the Application's events to the window's.
    m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

    // Initialize the rendering pipeline.
    Renderer::Init();

    // Initialize all ImGui related things.
    m_imguiLayer = new ImGuiLayer();

    // Add the ImGui layer to the layer stack as an overlay (on top of everything).
    PushOverlay(m_imguiLayer);
}

/**
 * @brief   Destroy the Application:: Application object
 */
Application::~Application() = default;

/**
 * @brief   The main run loop of the application, where all layers are updated.
 *
 * @param   None
 * @retval  None
 */
void Application::Run()
{
    BR_PROFILE_FUNCTION();

    // For as long as the application should be running:
    while (m_running)
    {
        BR_PROFILE_SCOPE("RunLoop");

        // Get the time elapsed since the last frame.
        float    time     = float(GetTime());
        Timestep timestep = time - m_lastFrameTime;
        m_lastFrameTime   = time;

        // If the window is not minimized:
        // (If the window is minimized, we don't want to waste time rendering stuff!)
        if (m_minimized == false)
        {
            {
                // Update all Application Layers.
                BR_PROFILE_SCOPE("Layer Stack OnUpdate");
                for (Layer* layer : m_layerStack)
                {
                    layer->OnUpdate(timestep);
                }
            }

            // Render all ImGui Layers.
            m_imguiLayer->Begin();
            {
                BR_PROFILE_SCOPE("LayerStack OnImGuiRender");
                for (Layer* layer : m_layerStack)
                {
                    layer->OnImGuiRender();
                }
            }
            m_imguiLayer->End();
        }

        // Do the per-frame window updating tasks.
        m_window->OnUpdate();
    }
}

/**
 * @brief   Callback function for all events happening in the application.
 *          It dispatches and propagates the event through all layers until it is handled.
 *
 * @param   e The event to be dispatched.
 */
void Application::OnEvent(Event& e)
{
    BR_PROFILE_FUNCTION();

    // Creates a dispatch context with the event.
    EventDispatcher dispatcher(e);
    // Dispatch it to the proper handling function in the Application, if the type matches.
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnKeyPressed));

    // For each layers in the layer stack, from the last one to the first:
    for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
    {
        // Pass the event to the layer.
        (*--it)->OnEvent(e);
        // If the event has been handled by that layer:
        if (e.Handled())
        {
            // Stop propagating it.
            break;
        }
    }
}

/**
 * @brief   Push a new layer at the back of the layer stack.
 *
 * @param   layer A pointer to the layer
 * @retval  None
 */
void Application::PushLayer(Layer* layer)
{
    BR_PROFILE_FUNCTION();

    // Push the new layer to the stack.
    m_layerStack.PushLayer(layer);
    // Initialize the layer.
    layer->OnAttach();
}

/**
 * @brief   Push a new layer at the front of the layer stack.
 *
 * @param   layer A pointer to the layer
 * @retval  None
 */
void Application::PushOverlay(Layer* layer)
{
    BR_PROFILE_FUNCTION();

    // Push the new layer to the stack.
    m_layerStack.PushOverlay(layer);
    // Initalize the layer.
    layer->OnAttach();
}

void Application::Close()
{
    m_running = false;
}

/**
 * @brief   Handle the window close event.
 *          This event happens whenever the main application window closes.
 *
 * @param   e The event
 * @retval  Always return true
 */
bool Application::OnWindowClose(WindowCloseEvent& e)
{
    m_running = false;
    return true;
}

/**
 * @brief   Handle the window resize event.
 *
 * @param   e
 * @retval  true
 * @retval  false
 */
bool Application::OnWindowResize(WindowResizeEvent& e)
{
    BR_PROFILE_FUNCTION();

    if (e.GetHeight() == 0 || e.GetWidth() == 0)
    {
        m_minimized = true;
        return false;
    }
    m_minimized = false;

    Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

    return false;
}

bool Application::OnKeyPressed(KeyPressedEvent& e)
{
    BR_PROFILE_FUNCTION();

    if (e.GetKeyCode() == BR_KEY_ESCAPE && e.GetRepeatCount() == 0)
    {
        m_imguiLayer->ToggleIsVisible();
        return true;
    }
    else
    {
        return false;
    }
}

}    // namespace Brigerad
