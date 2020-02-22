#include "Brigerad.h"

class Sandbox :public Brigerad::Application
{
public:
    Sandbox() = default;

    ~Sandbox() override = default;
};

Brigerad::Application* Brigerad::CreateApplication()
{
    return new Sandbox();
}