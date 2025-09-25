#version 410 core

// 转发顶点数据，以及控制 tessellation 密度，固定为 16 了

layout(vertices = 4) out;

in vec2 TexCoord[];

uniform mat4 model;
uniform mat4 view;
uniform vec3 ViewPosition;

out vec2 TextureCoord[];



void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

//	if (gl_InvocationID == 0)
//	{
//		gl_TessLevelOuter[0] = 16;
//		gl_TessLevelOuter[1] = 16;
//		gl_TessLevelOuter[2] = 16;
//		gl_TessLevelOuter[3] = 16;
//
//		gl_TessLevelInner[0] = 16;
//		gl_TessLevelInner[1] = 16;
//	}
	
	if (gl_InvocationID == 0)
	{
		const int MIN_TESS_LEVEL = 4;
		const int MAX_TESS_LEVEL = 128;
		const float MIN_DISTANCE = 20;
		const float MAX_DISTANCE = 800;

		// Step 2: transform each vertex into eye space，这样就只需要看z轴坐标就可以大致估算位置了
		vec4 eyeSpacePos00 = view * model * gl_in[0].gl_Position;
		vec4 eyeSpacePos01 = view * model * gl_in[1].gl_Position;
		vec4 eyeSpacePos10 = view * model * gl_in[2].gl_Position;
		vec4 eyeSpacePos11 = view * model * gl_in[3].gl_Position;

        float d00 = abs(eyeSpacePos00.z);
        float d01 = abs(eyeSpacePos01.z);
        float d10 = abs(eyeSpacePos10.z);
        float d11 = abs(eyeSpacePos11.z);

        // ----------------------------------------------------------------------
        // Step 3: "distance" from camera scaled between 0 and 1
        float distance00 = clamp((d00 - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
        float distance01 = clamp((d01 - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
        float distance10 = clamp((d10 - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
        float distance11 = clamp((d11 - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);

        // ----------------------------------------------------------------------
        // Step 4: interpolate edge tessellation level based on closer vertex
        float tessLevel0 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00));
        float tessLevel1 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01));
        float tessLevel2 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11));
        float tessLevel3 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10));

        // ----------------------------------------------------------------------
        // Step 5: set the corresponding outer edge tessellation levels
        gl_TessLevelOuter[0] = tessLevel0;
        gl_TessLevelOuter[1] = tessLevel1;
        gl_TessLevelOuter[2] = tessLevel2;
        gl_TessLevelOuter[3] = tessLevel3;

        // ----------------------------------------------------------------------
        // Step 6: set the inner tessellation levels to the max of the two parallel edges
        gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
        gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);

	}

}