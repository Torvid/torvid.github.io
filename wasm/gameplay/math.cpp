
#pragma once
// TODO: Convert all of these to paltform-efficient versions and remove math.h



#define PLATFORM_MATH 0
#define STATIC_LINK_MATH 1
#define DYNAMIC_LINK_MATH 2
#define C_MATH 3
#define WASM_MATH 4

#define MATH_TYPE C_MATH

#define USE_EXE_MATH 0


#if MATH_TYPE == PLATFORM_MATH
    #include "math.h"
    #include "float.h"
    float game_sqrtf(float n) { return sqrtf(n); }
    float game_floorf(float n) { return floorf(n); }
    float game_ceilf(float n) { return ceilf(n); }
    float game_sinf(float n) { return sinf(n); }
    float game_cosf(float n) { return cosf(n); }
    float game_atan2(float a, float b) { return atan2(a, b); }
    float game_log10(float n) { return log10(n); }
    float game_fabsf(float n) { return fabsf(n); }
    float game_pow(float a, float b) { return pow(a, b); }
    float game_fminf(float a, float b) { return fminf(a, b); }
    float game_fmaxf(float a, float b) { return fmaxf(a, b); }
    float game_roundf(float a) { return roundf(a); }
    float game_exp2f(float a) { return exp2f(a); }
    float game_acosf(float a) { return acosf(a); }
    bool  game_isfinite(float a) { return isfinite(a); }
#elif MATH_TYPE == STATIC_LINK_MATH
    float game_sqrtf(float n) { return exe_sqrtf(n); }
    float game_floorf(float n) { return exe_floorf(n); }
    float game_ceilf(float n) { return exe_ceilf(n); }
    float game_sinf(float n) { return exe_sinf(n); }
    float game_cosf(float n) { return exe_cosf(n); }
    float game_atan2(float a, float b) { return exe_atan2(a, b); }
    float game_log10(float n) { return exe_log10(n); }
    float game_fabsf(float n) { return exe_fabsf(n); }
    float game_pow(float a, float b) { return exe_pow(a, b); }
    float pow(float a, float b) { return game_pow(a, b); }
    float game_fminf(float a, float b) { return exe_fminf(a, b); }
    float game_fmaxf(float a, float b) { return exe_fmaxf(a, b); }
    float game_roundf(float a) { return exe_roundf(a); }
    float game_exp2f(float a) { return exe_exp2f(a); }
    float game_acosf(float a) { return exe_acosf(a); }
    bool  game_isfinite(float a) { return exe_isfinite(a); }
#elif MATH_TYPE == DYNAMIC_LINK_MATH
    float game_sqrtf(float n) { return GlobalGameMemory->sqrtf(n); }
    float game_floorf(float n) { return GlobalGameMemory->floorf(n); }
    float game_ceilf(float n) { return GlobalGameMemory->ceilf(n); }
    float game_sinf(float n) { return GlobalGameMemory->sinf(n); }
    float game_cosf(float n) { return GlobalGameMemory->cosf(n); }
    float game_atan2(float a, float b) { return GlobalGameMemory->atan2(a, b); }
    float game_log10(float n) { return GlobalGameMemory->log10(n); }
    float game_fabsf(float n) { return GlobalGameMemory->fabsf(n); }
    float game_pow(float a, float b) { return GlobalGameMemory->pow(a, b); }
    float pow(float a, float b) { return GlobalGameMemory->pow(a, b); }
    float game_fminf(float a, float b) { return GlobalGameMemory->fminf(a, b); }
    float game_fmaxf(float a, float b) { return GlobalGameMemory->fmaxf(a, b); }
    float game_roundf(float a) { return GlobalGameMemory->roundf(a); }
    float game_exp2f(float a) { return GlobalGameMemory->exp2f(a); }
    float game_acosf(float a) { return GlobalGameMemory->acosf(a); }
    bool  game_isfinite(float a) { return GlobalGameMemory->isfinite(a); }
#elif MATH_TYPE == C_MATH
    
    float pi = 3.14152128f;

    // Thanks doublescale for this idea!
    float SqrtDoublescale(float a)
    {
        if (a <= 0)
            return 0;

        // Square root asks "what number times itself makes this number?"
        // or, formulated a different way, what side-length of a perfect 
        // square makes a square that has the area of this number
        // 
        // If we say 9, the edge-length of the square is would intuitively be 3.
        // 
        // By knowing only the input number 9, we can trivially create a rectangle with that area. a 9x1 rectangle.
        // It has the right area, but to find out the "square" root, we need to somehow make this rectangle more and more
        // square-like, while perserving the area.
        // 
        // +------+------+------+------+------+------+------+------+------+ 0
        // |      |      |      |      |      |      |      |      |      |
        // |      |      |      |      |      |      |      |      |      |
        // +------+------+------+------+------+------+------+------+------+ 1
        // 0      1      2      3      4      5      6      7      8      9
        // To make this shape more square-like, we can get the total length (9 + 1) and divite it by 2. making 5.
        // 
        float x = a;
        float y = 1.0f;

        // +------+------+------+------+------+ 0
        // |      |      |      |      |      |
        // |      |      |      |      |      |
        // +------+------+------+------+------+ 1
        // 0      1      2      3      4      5
        // 
        // This no longer has an are of 9. to make it have an area of 9 again, we divide the desierd area (9) by the new side-length (5).
        // Because 5 * ? = 9, rearrange, 9 / 5 = ?
        // 9 / 5 =  1.8
        // 
        // +------+------+------+------+------+ 0
        // |      |      |      |      |      |
        // |      |      |      |      |      |
        // +------+------+------+------+------+ 1
        // |      |      |      |      |      |
        // +------+------+------+------+------+ 1.8
        // 0      1      2      3      4      5
        // 
        // 1 st itteration
        x = (x + y) * 0.5f;
        y = a / x;

        // We repeat these two last steps some number of times. Each time it gets closer and closer to the true square root.
        // 
        // x: (5.0 + 1.8) / 2.0 = 3.4
        // y: 9.0 / 3.4 = 2.6470588235...
        // 
        // +------+------+------+------+ 0
        // |      |      |      |      |
        // |      |      |      |      |
        // +------+------+------+------+ 1
        // |      |      |      |      |
        // |      |      |      |      |
        // +------+------+------+------+ 2
        // |      |      |      |      |
        // +------+------+------+------+ 2.6470588235...
        // 0      1      2      3     3.4
        // 
        // 2 nd itteration
        x = (x + y) * 0.5f;
        y = a / x;

        // x: (3.40 + 2.64) / 2.00 = 3.02352...
        // y: 9.00 / 3.02352... = 2.9766...
        // 
        // +------+------+------+-+ 0
        // |      |      |      | |
        // |      |      |      | |
        // +------+------+------+-+ 1
        // |      |      |      | |
        // |      |      |      | |
        // +------+------+------+-+ 2
        // |      |      |      | |
        // +------+------+------+-+ 2.9768235...
        // 0      1      2       3.02...
        // 3 rd itteration
        x = (x + y) * 0.5f;
        y = a / x;

        // x: (3.02 + 2.64) / 2.00 = 3.00017175...
        // y: 9.00 / 3.00... = 2.999...
        // 
        // +------+------+------+  0
        // |      |      |      |
        // |      |      |      |
        // +------+------+------+  1
        // |      |      |      |
        // |      |      |      |
        // +------+------+------+  2
        // |      |      |      |
        // |      |      |      |
        // +------+------+------+ 2.999...
        // 0      1      2     3.000...

        // 4 th itteration
        x = (x + y) * 0.5f;
        y = a / x;


        // 5 th itteration
        x = (x + y) * 0.5f;
        y = a / x;

        x = (x + y) * 0.5f;
        y = a / x;

        x = (x + y) * 0.5f;
        y = a / x;

        x = (x + y) * 0.5f;
        y = a / x;

        x = (x + y) * 0.5f;
        y = a / x;

        return x;
    }
    float game_sqrtf(float a)
    {
        return SqrtDoublescale(a);
    }
    float game_floorf(float n)
    {
        return (float)(int)(n < 0 ? (n - 1) : n);
    }
    float game_ceilf(float n) { return game_floorf(n + 1); }
    float game_frac(float n) { return n - game_floorf(n); }
    
    float game_fabsf(float n) { return n < 0 ? -n : n; }
    float game_atan2(float a, float b) // shitty implementaiton. really inaccurate.
    {
        float len = game_sqrtf(a * a + b * b);

        float value = 0;
        if (a > 0 && b > 0) // top right
        {
            value = game_fabsf(b / len);
        }
        else if (a < 0 && b > 0) // bottom right
        {
            value = game_fabsf(a / len) + 1;
        }
        else if (a < 0 && b < 0) // bottom left
        {
            value = game_fabsf(b / len) + 2;
        }
        else if (a > 0 && b < 0) // top left
        {
            value = game_fabsf(a / len) + 3;
        }
        return -value * 0.25f;
    }
    float game_log10(float n) { return 0; } // not implemented
    inline float lerppp(float a, float b, float f)
    {
        return a + f * (b - a);
    }
    float game_pow(float a, float b) // shitty implementaiton. really inaccurate.
    {
        float lowerPow = 0;
        float higherPow = a;
        for (int i = 0; i < game_floorf(b); i++)
        {
            lowerPow = higherPow;
            higherPow *= higherPow;
        }
            
        float fractionpart = game_frac(b);

        return lerppp(lowerPow, higherPow, fractionpart * fractionpart);
    }
    float pow(float a, float b) { return game_pow(a, b); }

    float game_fminf(float a, float b) { return a < b ? a : b; }
    float game_fmaxf(float a, float b) { return a > b ? a : b; }
    float game_roundf(float a) { return game_floorf(a + 0.5f); } 
    float game_exp2f(float a) { return a; } // not implemented
    float game_acosf(float a) { return a; } // not implemented
    bool  game_isfinite(float a) { return true; } // not implemented

    // good shit
    float game_fmod(float x, float y) {
        if(x < 0)
            return y - game_fabsf(x - (y * int(x / y)));
        else
            return game_fabsf(x - (y * int(x / y)));
    }

    // janky and inaccurate
    float game_sinf(float x)
    {
        //return GlobalGameMemory->sinf(x);
        //float y = game_fmod(x, pi * 2.0f);
        //
        //float a = (y - pi * 1.5f) / (pi * 0.5f); // right side
        //a *= a;
        //a = a - 1.0f;
        //
        //float b = (y - pi * 0.5f) / (pi * 0.5f); // left side
        //b *= b;
        //b = 1.0f - b;
        //
        //if (y > pi)
        //    return a;
        //else
        //    return b;

        // Wrapping
        float o = game_fabsf(game_fmod(x - pi * 0.5f, pi * 2.0f) - pi) - pi * 0.5f;

        // Taylor series
        float p = ((o) / 1.0f) -   
                  ((o * o * o) / 6.0f) + 
                  ((o * o * o * o * o) / 120.0f) -
                  ((o * o * o * o * o * o * o) / 5040.0f) +
                  ((o * o * o * o * o * o * o * o * o) / 362880.0f) -
                  ((o * o * o * o * o * o * o * o * o * o * o) / 39916800.0f);
        
        return p;
    }
    float game_cosf(float n)
    {
        return game_sinf(n + pi * 0.5f);
    }
#elif MATH_TYPE == WASM_MATH
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
    extern "C" bool  isfinite(float a);
    float game_sqrtf(float n) { return sqrtf(n); }
    float game_floorf(float n) { return floorf(n); }
    float game_ceilf(float n) { return ceilf(n); }
    float game_sinf(float n) { return sinf(n); }
    float game_cosf(float n) { return cosf(n); }
    float game_atan2(float a, float b) { return atan2(a, b); }
    float game_log10(float n) { return log10(n); }
    float game_fabsf(float n) { return fabsf(n); }
    float game_pow(float a, float b) { return pow(a, b); }
    float game_fminf(float a, float b) { return fminf(a, b); }
    float game_fmaxf(float a, float b) { return fmaxf(a, b); }
    float game_roundf(float a) { return roundf(a); }
    float game_exp2f(float a) { return exp2f(a); }
    float game_acosf(float a) { return acosf(a); }
    bool  game_isfinite(float a) { return isfinite(a); }
#endif

int asint(float x)
{
    return *((int*)(&x));
}
float asfloat(int x)
{
    return *((float*)(&x));
}
float sqrtFast(float x) // surprisingly accurate, BUT NOT ACCURATE ENOUGH
{
    int i = asint(x);
    i = 0x1FBD1DF5 + (i >> 1);
    return asfloat(i);
}


float sqrt(float x)
{
    return game_sqrtf(x);
}

float floor(float x)
{
    return game_floorf(x);
}

float ceil(float x)
{
    return game_ceilf(x);
}

float sin(float x)
{
    return game_sinf(x);
}

float cos(float x)
{
    return game_cosf(x);
}

float acos(float x)
{
    return game_acosf(x);
}
#undef min
#undef max
float min(float x, float y)
{
    return game_fminf(x, y);
}
float min(float x, float y, float z)
{
    return game_fminf(x, game_fminf(y, z));
}
float min(float x, float y, float z, float w)
{
    return game_fminf(x, game_fminf(game_fminf(y, z), w));
}
float max(float x, float y)
{
    return game_fmaxf(x, y);
}
float max(float x, float y, float z)
{
    return game_fmaxf(x, game_fmaxf(y, z));
}
float max(float x, float y, float z, float w)
{
    return game_fmaxf(x, game_fmaxf(game_fmaxf(y, z), w));
}


float abs(float x)
{
    return game_fabsf(x);
}

float round(float x)
{
    return game_roundf(x);
}

float frac(float a)
{
    return a - floor(a);
}

int32 RoundToInt(float x)
{
    return (int32)game_roundf(x);
}
uint32 RoundToUInt(float x)
{
    return (uint32)game_roundf(x);
}

int32 TruncateToInt(float x)
{
    return (int32)x;
}

int32 FloorToInt(float x)
{
    return (int32)game_floorf(x);
}

int32 CeilToInt(float x)
{
    return (int32)game_ceilf(x);
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


float mod(float a, float b)
{
    return frac(a / b) * b;
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


//INT2
typedef struct
{
    int x;
    int y;
    void operator =(int b)
    {
        x = b;
        y = b;
    }
} int2;
inline int2 ctor_int2(int x, int y)
{
    int2 result;
    result.x = x;
    result.y = y;
    return result;
}
#define int2(x, y) ctor_int2((x), (y))

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
float rand(float2 co)
{
    return frac(sin(dot(co, float2(12.9898, 78.233))) * 43758.5453);
}

float2 frac(float2 a)
{
    return float2(frac(a.x), frac(a.y));
}


//float hash(float i)
//{
//    return rand(float2(i, i*4));
//    //return frac(((float)(i * 2654435761 % 2000000000)) / 2000000);
//}
//enum bodytype
//{
//    bodytype_sphere,
//    bodytype_box
//};
//typedef struct
//{
//    bool initialized;
//
//    float2 position;
//    float2 right;
//    float2 up;
//    bool kinematic;
//    bodytype type;
//
//    float radius;
//    float2 size;
//
//    float mass;
//    float2 velocity;
//    float angularVelocity;
//
//} RigidBody2D;
//
//inline RigidBody2D ctor_RigidBody2DSphere(float2 position, float radius, float mass, bool kinematic)
//{
//    RigidBody2D result = {};
//    result.initialized = true;
//    result.position = position;
//    result.right = float2(1, 0);
//    result.up = float2(0, 1);
//    result.kinematic = kinematic;
//    result.type = bodytype_sphere;
//    result.mass = mass;
//    result.radius = radius;
//    //result.width = 0;
//    //result.height = 0;
//    //result.velocity = float2(0,0);
//    //result.angularVelocity = 0;
//    return result;
//}
//#define RigidBody2DSphere(position, radius, mass, kinematic) ctor_RigidBody2DSphere(position, radius, mass, kinematic)
//
//inline RigidBody2D ctor_RigidBody2DBox(float2 position, float2 size, float2 up, float mass, bool kinematic)
//{
//    RigidBody2D result = {};
//    result.initialized = true;
//    result.position = position;
//    result.right = float2(-up.y, up.x);
//    result.up = up;
//    result.mass = mass;
//    result.kinematic = kinematic;
//    result.type = bodytype_box;
//    result.size = size;
//    //result.radius = 0;
//    //result.velocity = float2(0,0);
//    //result.angularVelocity = 0;
//    return result;
//}
//#define RigidBody2DBox(position, size, up, mass, kinematic) ctor_RigidBody2DBox(position, size, up, mass, kinematic)



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

// takes the bit-pattern of an int and smashes it into a float
inline float3 float3_coerce(int x, int y, int z)
{
    float3 result;
    result.x = *(float*)&x;
    result.y = *(float*)&y;
    result.z = *(float*)&z;
    return result;
}

// takes the bit-pattern of an int and smashes it into a float
float float_coerce(int x)
{
    return *(float*)&x;
}

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
inline bool operator ==(float3 a, float3 b) { return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z)); }

inline float3 operator -(float3 a) // negate
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
    float l = a.x * a.x + a.y * a.y + a.z * a.z;
    if (l == 0)
        return 0;
    return sqrt(l);
}
inline float length(float3* a)
{
    float l = a->x * a->x + a->y * a->y + a->z * a->z;
    if (l == 0)
        return 0;
    return sqrt(l);
}
inline float distance(float3 Start, float3 End)
{
    return length(Start - End);
}
inline float3 normalize(float3 a)
{
    return a / sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}
inline float3 normalizeFast(float3 a)
{
    return a / sqrtFast(a.x * a.x + a.y * a.y + a.z * a.z);
}
inline float3 normalize_default(float3 a, float3 def)
{
    float d = a.x * a.x + a.y * a.y + a.z * a.z;
    if (d == 0)
        return def;
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
inline float3 lerp(float3 a, float3 b, float3 t)
{
    return float3(lerp(a.x, b.x, t.x), lerp(a.y, b.y, t.y), lerp(a.z, b.z, t.z));
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
    return max(a.x, max(a.y, a.z));
}
float min(float3 a)
{
    return min(a.x, min(a.y, a.z));
}
inline float3 sign(float3 a)
{
    return float3(sign(a.x), sign(a.y), sign(a.z));
}
float3 sin(float3 v)
{
    return float3(sin(v.x), sin(v.y), sin(v.z));
}

float3 cos(float3 v)
{
    return float3(cos(v.x), cos(v.y), cos(v.z));
}

float3 sqrt(float3 v)
{
    return float3(sqrt(v.x), sqrt(v.y), sqrt(v.z));
}
float3 floor(float3 v)
{
    return float3(floor(v.x), floor(v.y), floor(v.z));
}
float3 ceil(float3 v)
{
    return float3(ceil(v.x), ceil(v.y), ceil(v.z));
}
float3 round(float3 v)
{
    return float3(round(v.x), round(v.y), round(v.z));
}
float atan2(float2 a)
{
    return game_atan2(a.x, a.y);
}

float atan2(float x, float y)
{
    return game_atan2(x, y);
}

float AngleBetween(float2 A, float2 B)
{
    return atan2(B) - atan2(A);
    //return (float)Math.Acos(Vector3.Dot(A, B));
    //float x = B.Y - A.Y;
    //float y = B.X - A.X;
    //return (float)(Math.Atan2(y, x) * 57.2958f);
}
//bool isfinite(float3 a)
//{
//    return game_isfinite(a.x) && game_isfinite(a.y) && game_isfinite(a.z);
//}

float3 MoveTowards(float3 current, float3 target, float dist)
{
    return current + normalize(target - current) * min(dist, distance(target, current));
}
// TRANSFORM
// 60 bytes.
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
#define transform_pos(position) ctor_transform((position), (float3(0,1,0)), (float3(0,0,1)), (float3(1,1,1)))


float3 LocalToWorld(transform Trans, float3 position)
{
    Trans.right *= Trans.scale.x;
    Trans.forward *= Trans.scale.y;
    Trans.up *= Trans.scale.z;

    float3 BoxRightTransposed   = float3(Trans.right.x, Trans.forward.x, Trans.up.x);
    float3 BoxForwardTransposed = float3(Trans.right.y, Trans.forward.y, Trans.up.y);
    float3 BoxUpTransposed      = float3(Trans.right.z, Trans.forward.z, Trans.up.z);

    return (Trans.position + float3(
        dot(position, BoxRightTransposed),
        dot(position, BoxForwardTransposed),
        dot(position, BoxUpTransposed)));
}
float3 WorldToLocal(transform Trans, float3 position)
{
    return float3(
        dot(position - Trans.position, Trans.right / Trans.scale.x), 
        dot(position - Trans.position, Trans.forward / Trans.scale.y),
        dot(position - Trans.position, Trans.up / Trans.scale.z));
}

float3 LocalToWorldNoScale(transform Trans, float3 position)
{
    float3 BoxRightTransposed = float3(Trans.right.x, Trans.forward.x, Trans.up.x);
    float3 BoxForwardTransposed = float3(Trans.right.y, Trans.forward.y, Trans.up.y);
    float3 BoxUpTransposed = float3(Trans.right.z, Trans.forward.z, Trans.up.z);

    return (Trans.position + float3(
        dot(position, BoxRightTransposed),
        dot(position, BoxForwardTransposed),
        dot(position, BoxUpTransposed)));
}
float3 WorldToLocalNoScale(transform Trans, float3 position)
{
    return float3(
        dot(position - Trans.position, Trans.right),
        dot(position - Trans.position, Trans.forward),
        dot(position - Trans.position, Trans.up));
}

float3 LocalToWorldVector(transform Trans, float3 position)
{
    Trans.right *= Trans.scale.x;
    Trans.forward *= Trans.scale.y;
    Trans.up *= Trans.scale.z;

    float3 BoxRightTransposed = float3(Trans.right.x, Trans.forward.x, Trans.up.x);
    float3 BoxForwardTransposed = float3(Trans.right.y, Trans.forward.y, Trans.up.y);
    float3 BoxUpTransposed = float3(Trans.right.z, Trans.forward.z, Trans.up.z);

    return float3(
        dot(position, BoxRightTransposed),
        dot(position, BoxForwardTransposed),
        dot(position, BoxUpTransposed));
}
float3 WorldToLocalVector(transform Trans, float3 position)
{
    return float3(
        dot(position, Trans.right / Trans.scale.x),
        dot(position, Trans.forward / Trans.scale.y),
        dot(position, Trans.up / Trans.scale.z));
}

float3 LocalToWorldVectorNoScale(transform Trans, float3 position)
{
    float3 BoxRightTransposed = float3(Trans.right.x, Trans.forward.x, Trans.up.x);
    float3 BoxForwardTransposed = float3(Trans.right.y, Trans.forward.y, Trans.up.y);
    float3 BoxUpTransposed = float3(Trans.right.z, Trans.forward.z, Trans.up.z);

    return float3(
        dot(position, BoxRightTransposed),
        dot(position, BoxForwardTransposed),
        dot(position, BoxUpTransposed));
}
float3 WorldToLocalVectorNoScale(transform Trans, float3 position)
{
    return float3(dot(position, Trans.right),
                  dot(position, Trans.forward),
                  dot(position, Trans.up));
}
float ClosestRayDistance(float3 l1Pos, float3 l1Dir, float3 l2Pos, float3 l2dir)
{
    float3 rightVector = normalize(cross(l1Dir, cross(l1Dir, l2dir)));

    float alignment = dot(rightVector, l2dir);
    alignment = 1.0f / alignment;

    float3 s1 = l1Pos - l2Pos;
    float leftRightDistance = dot(rightVector, s1);
    return alignment * leftRightDistance;
}

float3 ClosestRayApproach(float3 l1Pos, float3 l1Dir, float3 l2Pos, float3 l2dir)
{
    return l2Pos + (l2dir * ClosestRayDistance(l1Pos, l1Dir, l2Pos, l2dir));
}

uint32 ToColor(float R, float G, float B, float A)
{
    return  (uint8)(clamp01(A) * 255) << 24 |
        (uint8)(clamp01(R) * 255) << 16 |
        (uint8)(clamp01(G) * 255) << 8 |
        (uint8)(clamp01(B) * 255) << 0;
}
uint32 ToColor(float3 Color, float A)
{
    return ToColor(Color.x, Color.y, Color.z, A);
}
uint32 ToColor(float3 Color)
{
    return ToColor(Color.x, Color.y, Color.z, 1);
}
uint32 ToColor(float R)
{
    return ToColor(R, R, R, 1);
}
float3 BlendColor(float3 a, float3 b, float t)
{
    // This is meant to be pow 2.2 and the sqrt is meant to be pow 0.454545, but it's too slow.
    a = a * a;
    b = b * b;

    b *= (1.0 - t);
    b += a * t;

    return sqrt(b);
}


float3 palette(float t, float3 a, float3 b, float3 c, float3 d)
{
    return a + b * cos(6.28318 * (c * t + d));
}



/*
public static float ClosestRayDistance(Vector3 l1Pos, Vector3 l1Dir, Vector3 l2Pos, Vector3 l2dir)
{
    Vector3 rightVector = Vector3.Cross(l1Dir, Vector3.Cross(l1Dir, l2dir));
    rightVector.Normalize();

    float alignment = Vector3.Dot(rightVector, l2dir);
    alignment = 1 / alignment;

    Vector3 s1 = l1Pos - l2Pos;
    float leftRightDistance = Vector3.Dot(rightVector, s1);
    return alignment * leftRightDistance;
}
public static Vector3 ClosestRayApproach(Vector3 l1Pos, Vector3 l1Dir, Vector3 l2Pos, Vector3 l2dir)
{
    return l2Pos + (l2dir * ClosestRayDistance(l1Pos, l1Dir, l2Pos, l2dir));
}
*/


//transform ctor_transform(float3 position, float3 scale)
//{
//    transform result;
//    result.position = position;
//    result.right = float3(1,0,0);
//    result.forward = float3(0,1,0);
//    result.up = float3(0,0,1);
//    result.scale = scale;
//    return result;
//}
//#define transform(position, scale) ctor_transform((position), (scale))


transform rotate(transform t, float3 Axis, float angle)
{
    t.forward = RotateAroundAxis(t.forward, Axis, angle);
    t.up = RotateAroundAxis(t.up, Axis, angle);
    t.right = cross(t.forward, t.up);

    t.forward = normalize(t.forward);
    t.up = normalize(t.up);
    t.right = normalize(t.right);
    return t;
}
transform rotate(transform t, float X, float Y, float Z)
{
    t = rotate(t, t.right, X);
    t = rotate(t, t.forward, Y);
    t = rotate(t, t.up, Z);
    return t;
}

transform LookRotation(transform t, float3 forward, float3 up)
{
    t.forward = normalize(forward);
    t.right = normalize(cross(forward, up));
    t.up = normalize(cross(t.right, forward));
    return t;
}


//typedef struct
//{
//    bool initialized;
//
//    transform t;
//
//    bool kinematic;
//    bodytype type;
//
//    float mass;
//    float3 velocity; // Linear velocity through space
//    float3 angularVelocityAxis; // Axis around which we are rotating
//    float angularVelocity; // Speed of that rotation
//} RigidBody3D;
//
//inline RigidBody3D ctor_RigidBody3DSphere(transform t, float mass, bool kinematic)
//{
//    RigidBody3D result = {};
//    result.initialized = true;
//    result.type = bodytype_sphere;
//    result.t = t;
//    result.kinematic = kinematic;
//    result.mass = mass;
//    return result;
//}
//#define RigidBody3DSphere(t, mass, kinematic) ctor_RigidBody3DSphere(t, mass, kinematic)
//
//inline RigidBody3D ctor_RigidBody3DBox(transform t, float mass, bool kinematic)
//{
//    RigidBody3D result = {};
//    result.initialized = true;
//    result.type = bodytype_box;
//    result.t = t;
//    result.kinematic = kinematic;
//    result.mass = mass;
//    return result;
//}
//#define RigidBody3DBox(t, mass, kinematic) ctor_RigidBody3DBox(t, mass, kinematic)
//
