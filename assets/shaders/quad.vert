#version 430 core

void main()
{
    vec2 vertices[6] =
    {
        vec2(-0.5, 0.5),  // Top left corner
        vec2(-0.5, -0.5), // Bottom left corner
        vec2(0.5, 0.5),   // Top right corner

        vec2(0.5, 0.5),   // Top right corner
        vec2(0.5, -0.5),  // Bottom right corner
        vec2(-0.5, -0.5), // Bottom left corner
    };

    gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0);
}