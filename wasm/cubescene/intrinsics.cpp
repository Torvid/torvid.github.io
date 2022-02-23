

// TODO: Convert all of these to paltform-efficient versions and remove math.h

#if WASM
extern "C" float sqrtf(float n);
extern "C" float floorf(float n);
extern "C" float ceilf(float n);
extern "C" float sinf(float n);
extern "C" float cosf(float n);
extern "C" float atan2(float a, float b);
extern "C" float log10(float n);
extern "C" float fabsf(float n);
extern "C" float pow(float a, float b);
extern "C" float fminf(float a, float b);
extern "C" float fmaxf(float a, float b);
extern "C" float roundf(float a);
extern "C" float exp2f(float a);
#else
#include "math.h"
#endif
float sqrt(float x)
{
    return sqrtf(x);
}

float floor(float x)
{
    return floorf(x);
}

float ceil(float x)
{
    return ceilf(x);
}

float sin(float x)
{
    return sinf(x);
}

float cos(float x)
{
    return  cosf(x);
}
#undef min
#undef max
float min(float x, float y)
{
    return  fminf(x, y);
}
float min(float x, float y, float z)
{
    return  fminf(x, fminf(y, z));
}
float min(float x, float y, float z, float w)
{
    return  fminf(x, fminf(fminf(y, z), w));
}
float max(float x, float y)
{
    return fmaxf(x, y);
}
float max(float x, float y, float z)
{
    return fmaxf(x, fmaxf(y, z));
}
float max(float x, float y, float z, float w)
{
    return fmaxf(x, fmaxf(fmaxf(y, z), w));
}

//float atan2(float x, float y)
//{
//    return atan2(x, y);
//}

//float log10(float a)
//{
//    return log10(a);
//}

float abs(float x)
{
    return fabsf(x);
}

//float pow(float a, float b)
//{
//    return pow(a, b);
//}
float round(float x)
{
    return roundf(x);
}

float frac(float a)
{
    return a - floor(a);
}


int32 RoundToInt(float x)
{
    return (int32)roundf(x);
}
uint32 RoundToUInt(float x)
{
    return (uint32)roundf(x);
}

int32 TruncateToInt(float x)
{
    return (int32)x;
}

int32 FloorToInt(float x)
{
    return (int32)floorf(x);
}

int32 CeilToInt(float x)
{
    return (int32)ceilf(x);
}



inline float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

inline uint8 lerp(uint8 a, uint8 b, uint8 f)
{
    return (a * (256 - f) + (b * f)) >> 8;
    //return f;
    // (a * (256 - f) + (b * f)) >> 8;
}


float clamp(float a, float b, float c)
{
    return min(max(a, b), c);
}
float clamp01(float a)
{
    return min(max(a, 0.0f), 1.0f);
}

float MoveTowards(float current, float target, float distance)
{
    return current + clamp(target - current, -distance, distance);
}


//float MoveTowards(float current, float target, float maxDelta)
//{
//    if (abs(target - current) <= maxDelta)
//    {
//        return target;
//    }
//    return current + signn(target - current) * maxDelta;
//
//    //return current + clamp(target - current, -distance, distance);
//}

inline float remap(float value, float startMin, float startMax, float targetMin, float targetMax)
{
    float iDist = startMin - startMax;
    float jDist = targetMin - targetMax;

    value -= startMin; // first, move v down so that i1 is 0
    float p = (value / iDist); // percentage of the way our current number is to i2
    value = jDist * p; // upscaled number
    value += targetMin; // add j1 to offset it to where it's meant to be
    return value;
}

inline float remap01(float value, float startMin, float startMax)
{
    return remap(value, startMin, startMax, 0, 1);
}

typedef struct 
{
    float x;
    float y;
    float z;
    //void operator =(float b)
    //{
    //    x = b;
    //    y = b;
    //    z = b;
    //}
} float3;

inline float3 ctor_float3(float x, float y, float z)
{
    float3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}
#define float3(x, y, z) ctor_float3((x), (y), (z))

inline float3 operator +(float3 a, float3 b) { return float3(a.x + b.x, a.y + b.y, a.z + b.z); }
inline float3 operator -(float3 a, float3 b) { return float3(a.x - b.x, a.y - b.y, a.z - b.z); }
inline float3 operator *(float3 a, float3 b) { return float3(a.x * b.x, a.y * b.y, a.z * b.z); }
inline float3 operator /(float3 a, float3 b) { return float3(a.x / b.x, a.y / b.y, a.z / b.z); }
inline float3 operator +(float3 a, float b)  { return float3(a.x + b, a.y + b, a.z + b); }
inline float3 operator -(float3 a, float b)  { return float3(a.x - b, a.y - b, a.z - b); }
inline float3 operator -(float a, float3 b)  { return float3(a - b.x, a - b.y, a - b.z); }
inline float3 operator *(float3 a, float b)  { return float3(a.x * b, a.y * b, a.z * b); }
inline float3 operator *(float a, float3 b)  { return float3(a * b.x, a * b.y, a * b.z); }
inline float3 operator /(float3 a, float b)  { return float3(a.x / b, a.y / b, a.z / b); }
inline float3 operator /(float a, float3 b)  { return float3(a / b.x, a / b.y, a / b.z); }
inline float3 operator -(float3 a)
{
    return float3(-a.x, -a.y, -a.z);
}

inline float dot(float3 a, float3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float3 clamp01(float3 a)
{
    return float3(clamp01(a.x), clamp01(a.y), clamp01(a.z));
}

inline float3 cross(float3 a, float3 b)
{
    return float3(a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}
inline float length(float3 a)
{
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}
inline float3 normalize(float3 a)
{
    return a / length(a);
}

inline float3 RotateAroundAxis(float3 v, float3 k, float a)
{
    return v * cos(a) + cross(k, v) * sin(a) + k * dot(k, v) * (1 - cos(a));
}

//inline float3 add(float3 a, float3 b) { return ctor_float3(a.x + b.x, a.y + b.y, a.z + b.z); }
//inline float3 sub(float3 a, float3 b) { return ctor_float3(a.x - b.x, a.y - b.y, a.z - b.z); }
//inline float3 mul(float3 a, float3 b) { return ctor_float3(a.x * b.x, a.y * b.y, a.z * b.z); }
//inline float3 div(float3 a, float3 b) { return ctor_float3(a.x / b.x, a.y / b.y, a.z / b.z); }
//inline float3 add(float3 a, float b) { return ctor_float3(a.x + b, a.y + b, a.z + b); }
//inline float3 sub(float3 a, float b) { return ctor_float3(a.x - b, a.y - b, a.z - b); }
//inline float3 sub(float a, float3 b) { return ctor_float3(a - b.x, a - b.y, a - b.z); }
//inline float3 mul(float3 a, float b) { return ctor_float3(a.x * b, a.y * b, a.z * b); }
//inline float3 div(float3 a, float b) { return ctor_float3(a.x / b, a.y / b, a.z / b); }
//inline float3 div(float a, float3 b) { return ctor_float3(a / b.x, a / b.y, a / b.z); }

float3 lerp(float3 a, float3 b, float t)
{
    return float3(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t));
}