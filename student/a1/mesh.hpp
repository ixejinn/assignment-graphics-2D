#pragma once
#include "course.hpp"
using namespace course;

struct Mesh 
{
    GLuint vao=0,vbo=0,ebo=0;
    GLsizei indexCount=0;
    Mesh()=default;
    Mesh(const Mesh&)=delete; 
    Mesh& operator=(const Mesh&)=delete;
    ~Mesh()
    {
        glDeleteVertexArrays(1,&vao);
        glDeleteBuffers(1,&vbo);
        glDeleteBuffers(1,&ebo);
    }
};

inline std::vector<Vertex> houseVertices()
{
    // TODO A1.1: build wall, roof, door, and window vertices (x,y,r,g,b).
    // Hint: Build CPU positions and colors in std::vector<Vertex>; no OpenGL calls are needed here.
    // In A1, (0, 0) maps to the screen center; x/y are normalized coordinates, not pixels.
    // Build the whole house around local (0, 0), using the center of its outer bounds
    // as the origin: (minX + maxX) / 2 = (minY + maxY) / 2 = 0.
    std::vector<Vertex> vertices(15);

    // roof
    vertices[0] = { -0.55f, 0.25f, 1, 0, 0 };
    vertices[1] = {  0.55f, 0.25f, 1, 0, 0 };
    vertices[2] = {  0.00f, 0.70f, 1, 0, 0 };

    // wall
    vertices[3] = { -0.45f, -0.70f, 1, 1, 1 };
    vertices[4] = {  0.45f, -0.70f, 1, 1, 1 };
    vertices[5] = {  0.45f,  0.25f, 1, 1, 1 };
    vertices[6] = { -0.45f,  0.25f, 1, 1, 1 };

    // door
    vertices[7]  = { -0.33f, -0.70f, 0, 1, 0 };
    vertices[8]  = { -0.07f, -0.70f, 0, 1, 0 };
    vertices[9]  = { -0.07f, -0.15f, 0, 1, 0 };
    vertices[10] = { -0.33f, -0.15f, 0, 1, 0 };

    // window
    vertices[11] = { 0.05f, -0.41f, 0, 0, 1 };
    vertices[12] = { 0.33f, -0.41f, 0, 0, 1 };
    vertices[13] = { 0.33f, -0.15f, 0, 0, 1 };
    vertices[14] = { 0.05f, -0.15f, 0, 0, 1 };

    // A2 rotates/scales about this local origin, then places it at Object::position in TRS mode.
    // Center the whole house, not each roof/wall/door/window separately.
    // The starter roof below is only a drawing check; it is not a centered complete house.
    return vertices;
}
inline std::vector<unsigned> houseIndices()
{
    // TODO A1.2: add triangle indices for all four house parts.
    // Hint: Add vertex indices to std::vector<unsigned>, with three indices per triangle.
    return
    {
        0, 1, 2,
        3,  4,  5,  3,  5,  6,
        7,  8,  9,  7,  9,  10,
        11, 12, 13, 11, 13, 14
    };
}
inline void createMesh(Mesh& mesh,const std::vector<Vertex>& vertices,const std::vector<unsigned>& indices)
{
    require(mesh.vao==0,"Create each immutable mesh only once");
    if(vertices.empty()||indices.empty())
        return;
    // TODO A1.3: glCreateBuffers + glNamedBufferStorage for VBO/EBO; byte sizes, flags=0.
    // Hint: Use glCreateBuffers(count, names) and glNamedBufferStorage(buffer, byteSize, data, flags). Use size() * sizeof(element type) and data().
    // Create names in mesh.vbo / mesh.ebo and upload the supplied arrays.
    
    // VBO, EBO 생성
    glCreateBuffers(1, &mesh.vbo);
    glCreateBuffers(1, &mesh.ebo);

    // VBO와 EBO의 GPU 메모리 할당, 데이터 복사
    glNamedBufferStorage(mesh.vbo, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), 0);
    glNamedBufferStorage(mesh.ebo, static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned)), indices.data(), 0);

    // TODO A1.4: create VAO, attach VBO to binding 0, attach EBO.
    // Hint: Use glCreateVertexArrays, glVertexArrayVertexBuffer(vao, binding, buffer, offset, stride), and glVertexArrayElementBuffer.
    // Binding index 0; offset 0; stride sizeof(Vertex).

    // VAO 생성
    glCreateVertexArrays(1, &mesh.vao);

    // VAO의 슬롯 0에 VBO를 바인딩
    glVertexArrayVertexBuffer(mesh.vao, 0, mesh.vbo, 0, sizeof(Vertex));

    // VAO에 EBO를 바인딩
    glVertexArrayElementBuffer(mesh.vao, mesh.ebo);

    // TODO A1.5: configure position (location 0, 2 floats) and color (location 1, 3 floats).
    // Hint: Use glVertexArrayAttribFormat, glVertexArrayAttribBinding, and glEnableVertexArrayAttrib. Use offsetof(Vertex, x) / offsetof(Vertex, r) for byte offsets.
    // Both attributes use VBO binding index 0. u/v are not enabled yet.

    // 정점의 위치를 의미하는 Vertex::x, Vertex::y를 attribute 0에 연결
    glEnableVertexArrayAttrib(mesh.vao, 0);     // VAO의 attribute 0을 활성화 (버텍스 셰이더의 layout(location = ...))
    glVertexArrayAttribBinding(mesh.vao, 0, 0); // VAO의 바인딩 슬롯 0과 attribute 0을 연결
    glVertexArrayAttribFormat(mesh.vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, x)); // attribute 0을 설정: 정점 하나가 갖는 성분의 개수는 2개(x, y), GL_FLOAT 타입으로, 정규화하지 않고, Vertex의 x부터 읽음

    // 정점의 색을 의미하는 Vertex::r, Vertex::g, Vertex::b를 attribute 1에 연결
    glEnableVertexArrayAttrib(mesh.vao, 1);
    glVertexArrayAttribBinding(mesh.vao, 1, 0);
    glVertexArrayAttribFormat(mesh.vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, r));

    mesh.indexCount=GLsizei(indices.size());
}
inline void drawMesh(const Mesh& mesh)
{
    if(!mesh.vao)
        return; // Keeps an unfinished starter runnable.
    // TODO A1.6: bind VAO and issue the indexed draw call.
    // Hint: Use glBindVertexArray followed by glDrawElements(mode, count, type, indices), with GL_TRIANGLES, GL_UNSIGNED_INT, and nullptr for the EBO start offset.
    // Count means number of indices, not number of triangles.

    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
}
