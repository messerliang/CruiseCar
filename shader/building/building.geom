#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform mat4 view;
uniform mat4 projection;

in VS_OUT{
	vec3 Position;
	vec3 Normal;
}gs_in[];

out GS_OUT{
	vec3 FragPosition;
	vec3 Normal;
} gs_out;



void GenerateLine(int index)
{
	vec3 start = gs_in[index].Position;
	vec3 normal = gs_in[index].Normal;

}

void main(){
	

}

