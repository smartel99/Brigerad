#include "brpch.h"

#include "RenderCommandQueue.h"


namespace Brigerad
{
RenderCommandQueue::RenderCommandQueue()
{
    m_commandBuffer    = new unsigned char[10 * 1024 * 1024];    // 10mB buffer.
    m_commandBufferPtr = m_commandBuffer;
    memset(m_commandBuffer, 0, 10 * 1024 * 1024);
}

RenderCommandQueue::~RenderCommandQueue()
{
    delete[] m_commandBuffer;
}

void* RenderCommandQueue::Allocate(RenderCommandFn fun, unsigned int size)
{
    *(RenderCommandFn*)m_commandBufferPtr = fun;
    m_commandBufferPtr += sizeof(RenderCommandFn);

    *(int*)m_commandBufferPtr = size;
    m_commandBufferPtr += sizeof(unsigned int);

    void* memory = m_commandBufferPtr;
    m_commandBufferPtr += size;

    m_commandCount++;
    return memory;
}

void RenderCommandQueue::Execute()
{
    BR_CORE_TRACE("RenderCommandQueue::Execute -- {0} commands, {1} bytes",
                  m_commandCount,
                  (m_commandBufferPtr - m_commandBuffer));

    unsigned char* buffer = m_commandBuffer;

    for (unsigned int i = 0; i < m_commandCount; i++)
    {
        RenderCommandFn function = *(RenderCommandFn*)buffer;
        buffer += sizeof(RenderCommandFn);

        unsigned int size = *(unsigned int*)buffer;
        buffer += sizeof(unsigned int);
        function(buffer);
        buffer += size;
    }

    m_commandBufferPtr = m_commandBuffer;
    m_commandCount     = 0;
}

}    // namespace Brigerad