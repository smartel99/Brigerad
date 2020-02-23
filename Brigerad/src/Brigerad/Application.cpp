#include "brpch.h"
#include "Application.h"

#include "Brigerad/Events/ApplicationEvent.h"
#include "Brigerad/Log.h"

namespace Brigerad
{
Application::Application()
{

}


Application::~Application()
{

}


void Application::Run()
{
    WindowResizeEvent e(1200, 720);
    BR_TRACE(e);


    while (true)
    {
    }
}


}