#pragma once


namespace Brigerad
{
enum class RendererAPI
{
    None = 0,
    OpenGL
};

class Renderer
{
public:
    inline static RendererAPI GetAPI()
    {
        return s_rendererApi;
    }

private:
    static RendererAPI  s_rendererApi;

};

}