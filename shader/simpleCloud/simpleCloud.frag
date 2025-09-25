#version 410 core

//in vec2 TexCoord;
//
//
//out vec4 FragColor;
//
//void main()
//{
//	FragColor = vec4(1,1,0, 1);
//}

uniform float currentTime;

out vec4 FragColor;

in vec4 QuadPosition;

// 3d perlin noise

// Î±Ëæ»úÊý
float hash1(vec3 p)
{
	p = fract(p * vec3(0.3183099, 0.3678794, 0.7071067) + 0.1);
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

vec3 grad3(vec3 p)
{
	// 
	float h1 = hash1(p + vec3(0.0, 0.0, 0.0));
	float h2 = hash1(p + vec3(1.3, 7.1, 2.4));
	float h3 = hash1(p + vec3(8.2, 3.7, 5.6));

	vec3 g = normalize(vec3(h1 * 2.0 - 1.0, h2 * 2.0 - 1.0, h3 * 2.0 - 1.0));

	return g;
}

// classic fade function 6t^5 - 15t^4 + 10t^3
vec3 fade(vec3 t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

// perlin-like 3D noise returning in [-1,1]
float perlin3(vec3 p) {
    // lattice cell coordinates and local (0..1) position
    vec3 Pi = floor(p);
    vec3 Pf = fract(p);

    // compute fade weights
    vec3 w = fade(Pf);

    // gather dot products at 8 corners
    // corners: (0,0,0) .. (1,1,1)
    float n000 = dot(grad3(Pi + vec3(0.0,0.0,0.0)), Pf - vec3(0.0,0.0,0.0));
    float n100 = dot(grad3(Pi + vec3(1.0,0.0,0.0)), Pf - vec3(1.0,0.0,0.0));
    float n010 = dot(grad3(Pi + vec3(0.0,1.0,0.0)), Pf - vec3(0.0,1.0,0.0));
    float n110 = dot(grad3(Pi + vec3(1.0,1.0,0.0)), Pf - vec3(1.0,1.0,0.0));
    float n001 = dot(grad3(Pi + vec3(0.0,0.0,1.0)), Pf - vec3(0.0,0.0,1.0));
    float n101 = dot(grad3(Pi + vec3(1.0,0.0,1.0)), Pf - vec3(1.0,0.0,1.0));
    float n011 = dot(grad3(Pi + vec3(0.0,1.0,1.0)), Pf - vec3(0.0,1.0,1.0));
    float n111 = dot(grad3(Pi + vec3(1.0,1.0,1.0)), Pf - vec3(1.0,1.0,1.0));

    // trilinear interpolation using fade weights
    float nx00 = mix(n000, n100, w.x);
    float nx10 = mix(n010, n110, w.x);
    float nx01 = mix(n001, n101, w.x);
    float nx11 = mix(n011, n111, w.x);

    float nxy0 = mix(nx00, nx10, w.y);
    float nxy1 = mix(nx01, nx11, w.y);

    float nxyz = mix(nxy0, nxy1, w.z);

    // nxyz is roughly in [-sqrt(3)/2, sqrt(3)/2], scale to [-1,1] comfortably
    return clamp(nxyz * 1.6, 0.0, 1.0);
}

// --- FBM (fractal sum) convenience wrapper
float fbmPerlin3(vec3 p, int octaves, float lacunarity, float gain) {
    float amp = 1.0;
    float freq = 1.0;
    float sum = 0.0;
    float maxAmp = 0.0;
    for (int i = 0; i < octaves; ++i) {
        sum += amp * perlin3(p * freq);
        maxAmp += amp;
        amp *= gain;
        freq *= lacunarity;
    }
    return sum / maxAmp; // normalized to [-1,1]
}



void main(){
	//float h = (fbmPerlin3(0.1 * QuadPosition.xyz, 4, 2, 0.5) + 1) / 2.0f;
    vec3 pos = 0.1 * QuadPosition.xyz + vec3(0,0,currentTime);
	float h = (perlin3(pos) + 1.3) / 2.0f;
    vec3 skyBlue = vec3(0.698, 0.875, 1.0);
    vec3 cloudWhite = vec3(1);
    vec3 color = mix(skyBlue, cloudWhite, h);
	FragColor = vec4(color, 1.0f);
	
}

