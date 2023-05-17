#pragma once
#include "GL/glew.h"

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class IndexBuffer;

#define DEBUG
#ifdef DEBUG
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall( x ) \
     GLClearError(); \
     x; \
     ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#define GLCallR( x ) [&]() { \
     GLClearError(); \
     auto retVal = x; \
     ASSERT(GLLogCall(#x, __FILE__, __LINE__)) \
     return retVal; \
   }()
#else
#define GLCallR( x ) x
#define GLCall( x ) x
#endif

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Clear() const;
};
