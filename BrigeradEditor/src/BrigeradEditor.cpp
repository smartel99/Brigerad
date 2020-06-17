#include "Brigerad.h"
#include "Brigerad/Core/EntryPoint.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "EditorLayer.h"

#include "ImGui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

#define sizeof_array(x) sizeof(x) / sizeof(x[0])

namespace Brigerad
{
class BrigeradEditor : public Application
{
public:
    BrigeradEditor() :
        Application("Brigerad Editor")
    {
        PushLayer(new EditorLayer());
    }

    ~BrigeradEditor() override = default;
};

Application* CreateApplication()
{
    return new BrigeradEditor();
}
}
