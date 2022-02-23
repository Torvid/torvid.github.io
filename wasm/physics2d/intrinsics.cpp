

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
extern "C" float acosf(float a);
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

float acos(float x)
{
    return acosf(x);
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



inline float sign(float a)
{
    return a > 0 ? 1 : -1;
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


//FLOAT2

typedef struct
{
    float x;
    float y;
    void operator =(float b)
    {
        x = b;
        y = b;
    }
} float2;
inline float2 ctor_float2(float x, float y)
{
    float2 result;
    result.x = x;
    result.y = y;
    return result;
}
#define float2(x, y) ctor_float2((x), (y))
inline void operator +=(float2& a, float2 b) { a.x += b.x; a.y += b.y;}
inline void operator -=(float2& a, float2 b) { a.x -= b.x; a.y -= b.y;}
inline void operator *=(float2& a, float2 b) { a.x *= b.x, a.y *= b.y;}
inline void operator /=(float2& a, float2 b) { a.x /= b.x, a.y /= b.y;}
inline void operator +=(float2& a, float  b) { a.x += b, a.y += b;}
inline void operator -=(float2& a, float  b) { a.x -= b, a.y -= b;}
inline void operator *=(float2& a, float  b) { a.x *= b, a.y *= b;}
inline void operator /=(float2& a, float  b) { a.x /= b, a.y /= b;}
inline float2 operator +(float2 a, float2 b) { return float2(a.x + b.x, a.y + b.y); }
inline float2 operator +(float2 a, float b) { return float2(a.x + b, a.y + b); }
inline float2 operator +(float a, float2 b) { return float2(a + b.x, a + b.y); }
inline float2 operator -(float2 a, float2 b) { return float2(a.x - b.x, a.y - b.y); }
inline float2 operator -(float2 a, float b) { return float2(a.x - b, a.y); }
inline float2 operator -(float a, float2 b) { return float2(a - b.x, a - b.y); }
inline float2 operator -(float2 b) { return float2(-b.x, -b.y); }
inline float2 operator *(float2 a, float2 b) { return float2(a.x * b.x, a.y * b.y); }
inline float2 operator *(float2 a, float b) { return float2(a.x * b, a.y * b); }
inline float2 operator *(float a, float2 b) { return float2(a * b.x, a * b.y); }
inline float2 operator /(float2 a, float2 b) { return float2(a.x / b.x, a.y / b.y); }
inline float2 operator /(float2 a, float b) { return float2(a.x / b, a.y / b); }
inline float2 operator /(float a, float2 b) { return float2(a / b.x, a / b.y); }

float dot(float2 a, float2 b)
{
    return a.x * b.x + a.y * b.y;
}
inline float length(float2 a)
{
    float d = dot(a, a);
    if (d == 0)
        return 0;
    return sqrt(d);
}
inline float distance(float2 Start, float2 End)
{
    return length(Start - End);
}
inline float2 normalize(float2 a)
{
    return a / length(a);
}

inline float2 reflect(float2 a, float2 normal)
{
    // How tf can a matrix be its own inverse.
    // ??????????????????
    float2 up = -float2(-normal.y, normal.x);
    float2 right = float2(normal.x, normal.y);
    float2 local_space_vector = float2(-dot(a, right), dot(a, up));
    return float2(dot(local_space_vector, right), dot(local_space_vector, up));
}

inline float2 lerp(float2 a, float2 b, float t)
{
    return float2(lerp(a.x, b.x, t), lerp(a.y, b.y, t));
}
inline float2 abs(float2 a)
{
    return float2(abs(a.x), abs(a.y));
}
inline float2 sign(float2 a)
{
    return float2(sign(a.x), sign(a.y));
}
float2 rotate(float2 v, float angle)
{
    float x = v.x * cos(angle) - v.y * sin(angle);
    float y = v.x * sin(angle) + v.y * cos(angle);
    return float2(x, y);
}

// FLOAT3
typedef struct 
{
    float x;
    float y;
    float z;
    void operator =(float b)
    {
        x = b;
        y = b;
        z = b;
    }
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

inline void operator +=(float3& a, float3 b) { a.x += b.x; a.y += b.y; a.z += b.z; }
inline void operator -=(float3& a, float3 b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; }
inline void operator *=(float3& a, float3 b) { a.x *= b.x, a.y *= b.y, a.z *= b.z; }
inline void operator /=(float3& a, float3 b) { a.x /= b.x, a.y /= b.y, a.z /= b.z; }
inline void operator +=(float3& a, float  b) { a.x += b, a.y += b, a.z += b; }
inline void operator -=(float3& a, float  b) { a.x -= b, a.y -= b, a.z -= b; }
inline void operator *=(float3& a, float  b) { a.x *= b, a.y *= b, a.z *= b; }
inline void operator /=(float3& a, float  b) { a.x /= b, a.y /= b, a.z /= b; }
inline float3 operator +(float3 a, float3 b) { return float3(a.x + b.x, a.y + b.y, a.z + b.z); }
inline float3 operator +(float3 a, float b) { return float3(a.x + b, a.y + b, a.z + b); }
inline float3 operator +(float a, float3 b) { return float3(a + b.x, a + b.y, a + b.z); }
inline float3 operator -(float3 a, float3 b) { return float3(a.x - b.x, a.y - b.y, a.z - b.z); }
inline float3 operator -(float3 a, float b) { return float3(a.x - b, a.y - b, a.z - b); }
inline float3 operator -(float a, float3 b) { return float3(a - b.x, a - b.y, a - b.z); }
inline float3 operator *(float3 a, float3 b) { return float3(a.x * b.x, a.y * b.y, a.z * b.z); }
inline float3 operator *(float3 a, float b) { return float3(a.x * b, a.y * b, a.z * b); }
inline float3 operator *(float a, float3 b) { return float3(a * b.x, a * b.y, a * b.z); }
inline float3 operator /(float3 a, float3 b) { return float3(a.x / b.x, a.y / b.y, a.z / b.z); }
inline float3 operator /(float3 a, float b) { return float3(a.x / b, a.y / b, a.z / b); }
inline float3 operator /(float a, float3 b) { return float3(a / b.x, a / b.y, a / b.z); }

inline float3 operator -(float3 a)
{
    return float3(-a.x, -a.y, -a.z);
}

inline float dot(float3 a, float3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float3 frac(float3 a)
{
    return float3(frac(a.x), frac(a.y), frac(a.z));
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
    return a / sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}
inline float3 RotateAroundAxis(float3 v, float3 k, float a)
{
    return v * cos(a) + cross(k, v) * sin(a) + k * dot(k, v) * (1 - cos(a));
}
inline float3 lerp(float3 a, float3 b, float t)
{
    return float3(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t));
}
inline float3 abs(float3 a)
{
    return float3(abs(a.x), abs(a.y), abs(a.z));
}
inline float3 reflect(float3 v, float3 normal)
{
    return v - 2.0 * normal * dot(v, normal);
}
float max(float3 a)
{
    return fmaxf(a.x, fmaxf(a.y, a.z));
}
inline float3 sign(float3 a)
{
    return float3(sign(a.x), sign(a.y), sign(a.z));
}


// TRANSFORM

typedef struct
{
    float3 position;
    float3 right; // x+
    float3 forward; // y+
    float3 up; // z+
    float3 scale;
} transform;

transform ctor_transform(float3 position, float3 forward, float3 up, float3 scale)
{
    transform result;
    result.position = position;
    result.right = cross(forward, up);
    result.forward = forward;
    result.up = up;
    result.scale = scale;
    return result;
}
#define transform(position, forward, up, scale) ctor_transform((position), (forward), (up), (scale))

void rotate(transform* t, float3 Axis, float angle)
{
    t->forward = RotateAroundAxis(t->forward, Axis, angle);
    t->up = RotateAroundAxis(t->up, Axis, angle);
    t->right = cross(t->forward, t->up);
}
