#version 330 core
/*
in 接受的参数如下：表明从顶点着色器接收下列任何一种图元
points: when drawing GL_POINTS primitives (1).
lines: when drawing GL_LINES or GL_LINE_STRIP (2).
lines_adjacency: GL_LINES_ADJACENCY or GL_LINE_STRIP_ADJACENCY (4).
triangles: GL_TRIANGLES, GL_TRIANGLE_STRIP or GL_TRIANGLE_FAN (3).
triangles_adjacency : GL_TRIANGLES_ADJACENCY or GL_TRIANGLE_STRIP_ADJACENCY (6).
*/

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;
/*
 out 可接受的类型：
points
line_strip
triangle_strip
*/

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.2;

void generateLine(int index) {
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE;
    EmitVertex();

    EndPrimitive();
}

void main() {    
    generateLine(0); // first vertex normal;
    generateLine(1); // second vertex normal;
    generateLine(2); // third vertex normal;
}