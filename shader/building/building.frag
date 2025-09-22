#version 330 core

in vec4 fColor;

in vec3 fragPosition; // 用来计算 blinn-phong 
in vec3 fragNormal;
out vec4 FragColor;

uniform sampler2D leafTexture;

uniform vec4 LightDirPos; // 光的方向或位置
uniform vec3 LightColor; // 光的颜色
uniform vec3 ViewPos; // 视角位置
uniform vec3 ObjectColor;	// 物体颜色
void main(){

//	FragColor = vec4(1.0f, 0.7f, 0.34f, 1.0f);
	//vec4 color =  texture(leafTexture,texCoord);
	//if(color.a < 0.01f){
	//	discard;
	//}
	//FragColor = texture(leafTexture,texCoord);
	
	vec3 LightDir = normalize(LightDirPos.xyz);
	if(LightDirPos.a > 0)
	{
		LightDir = normalize(LightDirPos.xyz - fragPosition);
	}

	vec4 objectColor = vec4(ObjectColor, 1.0f);

	// 环境光
	vec3 ambient = 0.251 * LightColor;

	// 漫反射
	float diff = max(dot(normalize(fragNormal), normalize(LightDir)), 0.0f);
	vec3 diffuse = diff * LightColor;

	// 高光
	vec3 viewDir = normalize(ViewPos - fragPosition);
	vec3 H = normalize(viewDir + normalize(LightDir));
	float spec = pow(max(dot(fragNormal, H),0), 32);
	vec3 specular = spec * LightColor;

	
	vec3 totalEffect = ambient + diffuse  +specular;
	vec3 factor = vec3(min(1.0f, totalEffect.x), min(1.0f, totalEffect.y), min(1.0f, totalEffect.z));

	vec4 color = vec4(factor, 1.0f) * objectColor;

	FragColor = color;
}

