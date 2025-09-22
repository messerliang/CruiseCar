#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform mat4 view;
uniform mat4 projection;

in VS_OUT{
	vec3 Position;
	vec3 Normal;
}gs_in[];





void GenerateLine(int index)
{
	vec3 start = gs_in[index].Position;
	vec3 normal = gs_in[index].Normal;

	gl_Position = projection * view * vec4(start, 1.0);
    EmitVertex();

	vec3 end = start + normal * 0.2;
	gl_Position = projection * view * vec4(end, 1.0);
	EmitVertex();
	EndPrimitive();
}

void main(){
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);

}

