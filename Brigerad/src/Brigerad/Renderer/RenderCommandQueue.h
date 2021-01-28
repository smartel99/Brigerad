#pragma once

namespace Brigerad
{
class RenderCommandQueue
{
public:
    typedef void (*RenderCommandFn)(void*);

    RenderCommandQueue();
    ~RenderCommandQueue();

    void* Allocate(RenderCommandFn fun, unsigned int size);

    void Execute();

private:
    unsigned char* m_commandBuffer;
    unsigned char* m_commandBufferPtr;
    unsigned int   m_commandCount = 0;
};
}    // namespace Brigerad