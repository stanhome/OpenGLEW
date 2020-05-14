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
layout (triangle_strip, max_vertices = 3) out;
/*
 out 可接受的类型：
points
line_strip
triangle_strip
*/

in VS_OUT {
    vec2 texCoords;
} gs_in[];

out vec2 texCoords;

uniform float time;

vec4 explode(vec4 position, vec3 normal) {
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) + 1.0) * 0.5) * magnitude;

    return position + vec4(direction, 0.0);
}

vec3 getNormal() {
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);

    return normalize(cross(a, b));
}


void main() {    
    vec3 normal = getNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    texCoords = gs_in[0].texCoords;
    EmitVertex();

    gl_Position = explode(gl_in[1].gl_Position, normal);
    texCoords = gs_in[1].texCoords;
    EmitVertex();

    gl_Position = explode(gl_in[2].gl_Position, normal);
    texCoords = gs_in[2].texCoords;
    EmitVertex();

    EndPrimitive();
}