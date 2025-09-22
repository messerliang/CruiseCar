#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices=5) out;

in vec4 vertColor[];

out vec4 fColor;

void build_house(vec4 position){
	fColor = vertColor[0];
	gl_Position = position + vec4(-0.2f, -0.2f, 0.0f, 0.0f) * 5.0f;
	EmitVertex();
	gl_Position = position + vec4( 0.2f, -0.2f, 0.0f, 0.0f) * 5.0f;
	EmitVertex();
	gl_Position = position + vec4(-0.2f,  0.2f, 0.0f, 0.0f) * 5.0f;
	EmitVertex();
	gl_Position = position + vec4( 0.2f,  0.2f, 0.0f, 0.0f) * 5.0f;
	EmitVertex();
	gl_Position = position + vec4( 0.0f,  0.4f, 0.0f, 0.0f) * 5.0f;
	EmitVertex();

	EndPrimitive();
	
}

void main(){
	build_house(gl_in[0].gl_Position);

}

