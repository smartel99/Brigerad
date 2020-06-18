# Brigerad Engine
Simplistic game engine/GUI framework developed in C++ following The Cherno's Game Engine series on [Youtube](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT).
It is meant to be simple to use, open source and cross platform, currently supporting OpenGL only for Windows and Linux.

## How do I get working with Brigerad?
Simply clone the repository on your system using `git clone https://github.com/smartel99/Brigerad --recurse` in your terminal/command prompt.

## How do I compile Brigerad?
### For Linux
Run the `generate.sh` script found in the `/script/Linux` directory to generate a Makefile.
You can then use the `make.sh` script in the same directory to build the debug version of Brigerad.

### For Windows
Run the `generate.bat` script found in the `/script/Windows` directory to generate a Visual Studio 2019 solution, then do as usual to compile the code.

## How do I make my own application with Brigerad?
### Brigerad::Application
The core of any Brigerad application is the `Brigerad::Application` class. In order to make your own application, you must first start by creating a class that inherits from `Brigerad::Application`
```cpp
#include "Brigerad.h"

class MyApplication : public Brigerad::Application
{
public:
    MyApplication();
    ~MyApplication override = default;
};
```
### Entry Point
To bind this newly create application to Brigerad, the `Brigerad::CreateApplication()` function must be implemented. This function is what is called by `main`.
```cpp
#include "Brigerad/Core/EntryPoint.h"

Brigerad::Application* Brigerad::CreateApplication()
{
    return new MyApplication();
}
```
### Brigerad::Layer
Now in order to do things inside that application, you need to create a layer.
A layer can be seen as a level or a scene, but can also be a HUD or GUI elements displayed on screen.
Layers in Brigerad are where things happens and contains the following methods:

- `(constructor)` - Like any classes in C++, the constructor is called upon creation of an object.
- `(destructor)` - Likewise, the destructor is called upon destruction of an object when it becomes out of scope or freed from memory
- `OnAttach` - This method is called automatically when you bind the layer to the application using the `Application::PushLayer` or the `Application::PushOverlay` method.
- `OnDetach` - This method is called automatically whenever you remove the layer from the application, which is on application shutdown at the moment, with no current ways of detaching a layer from the application
- `OnUpdate` - Called every frames, the `OnUpdate` method is the main function of a layer, where you should be doing everything that occurs every frame
- `OnImGuiRender` - Also called every frames, this method should be used to take care of ImGui related things.
- `OnEvent` - Called by the application whenever an event occurs.  The layer is in charge of checking what the event is and handling in in consequence, setting the `m_handled` flag of the `Event` object to true if the event shouldn't be propagated to the next layers

```cpp
#include "Brigerad.h"

class MyLayer: public Brigerad::Layer
{
public:
    MyLayer()
      : Layer("MyLayer"), m_camera(1280.0f/720.0f)
    {
        m_texture = Brigerad::Texture2D::Create("assets/textures/checkboard.png");
    }

    void OnUpdate(Brigerad::Timestep ts) override
    {
        // Update the camera.
        m_camera.OnUpdate(ts);
        // Render.
        Brigerad::RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});
        Brigerad::RenderCommand::Clear();

        Brigerad::Renderer2D::BeginScene(m_camera.GetCamera());
        Brigerad::Renderer2D::DrawQuad({0.0f, 0.0f, 0.0f},
                                       {1.0f, 1.0f},
                                       m_texture,
                                       {1.0f, 1.0f},
                                       {1.0f, 1.0f, 1.0f, 1.0f});
        Brigerad::Renderer2D::EndScene();
    }
    
    void OnImGuiRender()
    {
        ImGui::Begin("My Window");
        ImGui::Text("Hello, World!");
        ImGui::End();
    }

    void OnEvent(Brigerad::Event& e)
    {
        m_camera.OnEvent(e);
    }

private:
        Brigerad::OrthographicCameraController m_camera;
        Brigerad::Ref<Brigerad::Texture2D> m_texture;
};
```

Now that the layer is created, you need to bind it to the application. There are two ways of accomplishing this:
- `Application::PushLayer`
- `Application::PushOverlay`
The difference between a layer and an overlay is rather simple: an overlay will always be rendered last whereas normal layers are rendered in the order they were attached to the application.

```cpp
class MyApplication : public Brigerad::Application
{
public:
    MyApplication()
    {
        PushLayer(new MyLayer());
    }

    ~MyApplication() override = default;
};
``` 
