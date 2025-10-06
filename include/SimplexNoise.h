/**
 * @file    SimplexNoise.h
 * @brief   A Perlin Simplex Noise C++ Implementation (1D, 2D, 3D).
 *
 * Copyright (c) 2014-2018 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once
#include <glm/glm.hpp>
#include <cstddef>  // size_t
#include <vector>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


 /**
  * @brief A Perlin Simplex Noise C++ Implementation (1D, 2D, 3D, 4D).
  */
class SimplexNoise {
public:
    // 1D Perlin simplex noise
    static float noise(float x);
    // 2D Perlin simplex noise
    static float noise(float x, float y);
    // 3D Perlin simplex noise
    static float noise(float x, float y, float z);

    // Fractal/Fractional Brownian Motion (fBm) noise summation
    float fractal(size_t octaves, float x) const;
    float fractal(size_t octaves, float x, float y) const;
    float fractal(size_t octaves, float x, float y, float z) const;

    /**
     * Constructor of to initialize a fractal noise summation
     *
     * @param[in] frequency    Frequency ("width") of the first octave of noise (default to 1.0)
     * @param[in] amplitude    Amplitude ("height") of the first octave of noise (default to 1.0)
     * @param[in] lacunarity   Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
     * @param[in] persistence  Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)
     */
    explicit SimplexNoise(float frequency = 1.0f,
        float amplitude = 1.0f,
        float lacunarity = 2.0f,
        float persistence = 0.5f) :
        mFrequency(frequency),
        mAmplitude(amplitude),
        mLacunarity(lacunarity),
        mPersistence(persistence) {
    }

private:
    // Parameters of Fractional Brownian Motion (fBm) : sum of N "octaves" of noise
    float mFrequency;   ///< Frequency ("width") of the first octave of noise (default to 1.0)
    float mAmplitude;   ///< Amplitude ("height") of the first octave of noise (default to 1.0)
    float mLacunarity;  ///< Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
    float mPersistence; ///< Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)
};


class FbmPerlin3D {

private:
    // Parameters of Fractional Brownian Motion (fBm) : sum of N "octaves" of noise
    float mFrequency;   ///< Frequency ("width") of the first octave of noise (default to 1.0)
    float mAmplitude;   ///< Amplitude ("height") of the first octave of noise (default to 1.0)
    float mLacunarity;  ///< Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
    float mPersistence; ///< Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)
public:
    /**
     * Constructor of to initialize a fractal noise summation
     *
     * @param[in] frequency    Frequency ("width") of the first octave of noise (default to 1.0)
     * @param[in] amplitude    Amplitude ("height") of the first octave of noise (default to 1.0)
     * @param[in] lacunarity   Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
     * @param[in] persistence  Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)
     */
    explicit FbmPerlin3D(float frequency = 1.0f,
        float amplitude = 1.0f,
        float lacunarity = 2.0f,
        float persistence = 0.5f) :
        mFrequency(frequency),
        mAmplitude(amplitude),
        mLacunarity(lacunarity),
        mPersistence(persistence) {
    }

    // perlin-like 3D noise returning in [-1,1]
    float perlin3(glm::vec3 p);

    // --- FBM (fractal sum) convenience wrapper
    float fbmPerlin3(glm::vec3 p, int octaves, float lacunarity = 2.0f, float gain = 0.5f);
private:

    // Î±Ëæ»úÊý
    float hash1(glm::vec3 p);

    glm::vec3 grad3(glm::vec3 p);

    // classic fade function 6t^5 - 15t^4 + 10t^3
    glm::vec3 fade(glm::vec3 t);


};


class WorleyNoise3D {

public:
    WorleyNoise3D() {}

public:

    // Hash by David_Hoskins
    unsigned int UI0 = 1597334673U;
    unsigned int UI1 = 3812015801U;
    glm::u32vec2 UI2 = glm::u32vec2(UI0, UI1);
    glm::u32vec3 UI3 = glm::u32vec3(UI0, UI1, 2798796415U);
    double UIF = (1.0 / float(0xffffffffU));

public:
    glm::vec3 hash33(glm::vec3 p)
    {
        glm::u32vec3 q = glm::u32vec3(glm::ivec3(p)) * UI3;
        q = (q.x ^ q.y ^ q.z) * UI3;
        return -1.f + 2.f * glm::vec3(q) * (float)UIF;
    }
    float remap(float x, float a, float b, float c, float d)
    {
        return (((x - a) / (b - a)) * (d - c)) + c;
    }


    // Gradient noise by iq (modified to be tileable)
    float gradientNoise(glm::vec3 x, float freq)
    {
        // grid
        glm::vec3 p = glm::floor(x);
        glm::vec3 w = glm::fract(x);

        // quintic interpolant
        glm::vec3 u = w * w * w * (w * (w * 6.f - 15.f) + 10.f);


        // gradients
        glm::vec3 ga = hash33(glm::mod(p + glm::vec3(0., 0., 0.), freq));
        glm::vec3 gb = hash33(glm::mod(p + glm::vec3(1., 0., 0.), freq));
        glm::vec3 gc = hash33(glm::mod(p + glm::vec3(0., 1., 0.), freq));
        glm::vec3 gd = hash33(glm::mod(p + glm::vec3(1., 1., 0.), freq));
        glm::vec3 ge = hash33(glm::mod(p + glm::vec3(0., 0., 1.), freq));
        glm::vec3 gf = hash33(glm::mod(p + glm::vec3(1., 0., 1.), freq));
        glm::vec3 gg = hash33(glm::mod(p + glm::vec3(0., 1., 1.), freq));
        glm::vec3 gh = hash33(glm::mod(p + glm::vec3(1., 1., 1.), freq));

        // projections
        float va = glm::dot(ga, w - glm::vec3(0., 0., 0.));
        float vb = glm::dot(gb, w - glm::vec3(1., 0., 0.));
        float vc = glm::dot(gc, w - glm::vec3(0., 1., 0.));
        float vd = glm::dot(gd, w - glm::vec3(1., 1., 0.));
        float ve = glm::dot(ge, w - glm::vec3(0., 0., 1.));
        float vf = glm::dot(gf, w - glm::vec3(1., 0., 1.));
        float vg = glm::dot(gg, w - glm::vec3(0., 1., 1.));
        float vh = glm::dot(gh, w - glm::vec3(1., 1., 1.));

        // interpolation
        return va +
            u.x * (vb - va) +
            u.y * (vc - va) +
            u.z * (ve - va) +
            u.x * u.y * (va - vb - vc + vd) +
            u.y * u.z * (va - vc - ve + vg) +
            u.z * u.x * (va - vb - ve + vf) +
            u.x * u.y * u.z * (-va + vb + vc - vd + ve - vf - vg + vh);
    }


    // Tileable 3D worley noise
    float worleyNoise(glm::vec3 uv, float freq)
    {
        glm::vec3 id = glm::floor(uv);
        glm::vec3 p = glm::fract(uv);

        float minDist = 10000.;
        for (float x = -1.; x <= 1.; ++x)
        {
            for (float y = -1.; y <= 1.; ++y)
            {
                for (float z = -1.; z <= 1.; ++z)
                {
                    glm::vec3 offset = glm::vec3(x, y, z);
                    glm::vec3 h = hash33(glm::mod(id + offset, glm::vec3(freq))) * .5f + .5f;
                    h += offset;
                    glm::vec3 d = p - h;
                    float tmp = glm::dot(d, d);
                    minDist = minDist < tmp ? minDist : tmp;
                }
            }
        }

        // inverted worley noise
        return 1. - minDist;
    }

    // Fbm for Perlin noise based on iq's blog
    float perlinfbm(glm::vec3 p, float freq, int octaves)
    {
        float G = exp2(-.85);
        float amp = 1.;
        float noise = 0.;
        for (int i = 0; i < octaves; ++i)
        {
            noise += amp * gradientNoise(p * freq, freq);
            freq *= 2.;
            amp *= G;
        }

        return noise;
    }

    // Tileable Worley fbm inspired by Andrew Schneider's Real-Time Volumetric Cloudscapes
    // chapter in GPU Pro 7.
    float worleyFbm(glm::vec3 p, float freq=1.0f)
    {
        return worleyNoise(p * freq, freq) * .625 +
            worleyNoise(p * freq * 2.f, freq * 2.f) * .25f+
            worleyNoise(p * freq * 4.f, freq * 4.f) * .125f;
    }

    std::vector<glm::vec3> GenNoise(int XDim, int YDim, int ZDim, float scale=0.5f)
    {
        std::vector<glm::vec3> rgbNoise;
        float maxVal = -1e9;
        float minVal = 1e9;
        
        for (float x = 0; x < XDim; x++)
        {
            for (float y = 0; y < YDim; y++)
            {
                for (float z = 0; z < ZDim; z++)
                {
                    float val = perlinfbm(glm::vec3(scale * x, scale * y, scale * z), 1.0f,4);
                    maxVal = max(val, maxVal);
                    minVal = min(val, minVal);
                    rgbNoise.emplace_back((val + 1.0f) / 2.f);
                }
            }
        }
        float range = maxVal - minVal;
        for (auto& val : rgbNoise)
        {
            val /= range;
        }
        return rgbNoise;
    }
};