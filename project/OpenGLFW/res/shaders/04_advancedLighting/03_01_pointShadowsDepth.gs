#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowVP[6];

out vec4 fragPos; // fragPos from GS(output per emitvertex)

void main()
{
	for(int face = 0; face < 6; ++face) {
		gl_Layer = face; // built-in variable that specifies to which face to render.
		for(int i = 0; i < 3; ++i) {
			// for each triangle's vertices
			fragPos = gl_in[i].gl_Position;
			gl_Position = shadowVP[face] * fragPos;
			EmitVertex();
		}

		EndPrimitive();
	}
}