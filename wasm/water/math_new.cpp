#pragma once

#include "haven_platform.h"

static const float pi = 3.14159265358979323926264338327;
static const float tau = 6.283185307179586;

#if PLATFORM == WIN32
    // Windows.
#elif PLATFORM == WASM
    // Webassembly.
#elif PLATFORM == ESP32
    // ESP32 Microcontroller.
#else
    // Unknown platform.
#endif

#define PLATFORM_MATH 0
#define STATIC_LINK_MATH 1
#define DYNAMIC_LINK_MATH 2
#define C_MATH 3
#define WASM_MATH 4

#define MATH_TYPE PLATFORM_MATH

#define USE_EXE_MATH 0

#define Win32Math 0

// Core math functions which get called elsewhere
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
    //float game_pow(float a, float b) { return pow(a, b); }

    //float game_frac(float n) { return n - game_floorf(n); }
    //float lerppp(float a, float b, float f)
    //{
    //    return a + f * (b - a);
    //}
    //float game_pow(float a, float b) // shitty implementaiton. really inaccurate.
    //{
    //    float lowerPow = 0;
    //    float higherPow = a;
    //    for (int i = 0; i < game_floorf(b); i++)
    //    {
    //        lowerPow = higherPow;
    //        higherPow *= higherPow;
    //    }
    //
    //    float fractionpart = game_frac(b);
    //
    //    return lerppp(lowerPow, higherPow, fractionpart * fractionpart);
    //}
    //float pow(float a, float b) { return game_pow(a, b); }
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
#elif MATH_TYPE == C_MATH // Pure C implementation of math functions.
    
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
    float lerppp(float a, float b, float f)
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
    //float pow(float a, float b) { return game_pow(a, b); }

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

    float game_sinf(float x)
    {
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

// counts the number of set bits in an int.
int popcount(int value)
{
#if __clang__
    return __builtin_popcount(value);
#elif _MSC_VER 
    return __popcnt(value);
#else
    int count = 0;
    int test = value;
    for (int i = 0; i < 32; i++)
    {
        if (test & 1)
            count++;
        test >>= 1;
    }
    return test;
#endif
}

char GetNthBit(char c, char n)
{
    unsigned char tmp = 1 << n;
    return (c & tmp) >> n;
}


int32 RoundToInt(float x){return (int32)game_roundf(x);}
uint32 RoundToUInt(float x){return (uint32)game_roundf(x);}
int32 TruncateToInt(float x){return (int32)x;}
int32 FloorToInt(float x){return (int32)game_floorf(x);}
int32 CeilToInt(float x){return (int32)game_ceilf(x);}

// Intiger square root
int64 sqrt_i64(int64 a)
{
    if (a <= 0)
        return 0;
    int64 x = a;
    int64 y = 1;
    x = (x + y) / 2;
    y = a / x;
    x = (x + y) / 2;
    y = a / x;
    x = (x + y) / 2;
    y = a / x;
    x = (x + y) / 2;
    y = a / x;
    x = (x + y) / 2;
    y = a / x;
    x = (x + y) / 2;
    y = a / x;
    x = (x + y) / 2;
    y = a / x;
    x = (x + y) / 2;
    y = a / x;
    x = (x + y) / 2;
    y = a / x;
    return x;
}



// Two-component int vector. Used to represnt positions in grids and other fixed spaces.
//struct int2
//{
//    int x;
//    int y;
//};
int2 ctor_int2(int x, int y)
{
    int2 result;
    result.x = x;
    result.y = y;
    return result;
}
#define int2(x, y) ctor_int2((x), (y))
void operator +=(int2& a, int2 b) { a.x += b.x; a.y += b.y; }
void operator -=(int2& a, int2 b) { a.x -= b.x; a.y -= b.y; }
void operator *=(int2& a, int2 b) { a.x *= b.x, a.y *= b.y; }
void operator /=(int2& a, int2 b) { a.x /= b.x, a.y /= b.y; }
void operator +=(int2& a, int  b) { a.x += b, a.y += b; }
void operator -=(int2& a, int  b) { a.x -= b, a.y -= b; }
void operator *=(int2& a, int  b) { a.x *= b, a.y *= b; }
void operator /=(int2& a, int  b) { a.x /= b, a.y /= b; }
int2 operator +(int2 a, int2 b) { return int2(a.x + b.x, a.y + b.y); }
int2 operator +(int2 a, int b) { return int2(a.x + b, a.y + b); }
int2 operator +(int a, int2 b) { return int2(a + b.x, a + b.y); }
int2 operator -(int2 a, int2 b) { return int2(a.x - b.x, a.y - b.y); }
int2 operator -(int2 a, int b) { return int2(a.x - b, a.y - b); }
int2 operator -(int a, int2 b) { return int2(a - b.x, a - b.y); }
int2 operator -(int2 b) { return int2(-b.x, -b.y); }
int2 operator *(int2 a, int2 b) { return int2(a.x * b.x, a.y * b.y); }
int2 operator *(int2 a, int b) { return int2(a.x * b, a.y * b); }
int2 operator *(int a, int2 b) { return int2(a * b.x, a * b.y); }
int2 operator /(int2 a, int2 b) { return int2(a.x / b.x, a.y / b.y); }
int2 operator /(int2 a, int b) { return int2(a.x / b, a.y / b); }
int2 operator /(int a, int2 b) { return int2(a / b.x, a / b.y); }


// Two-component vector, float2. Used to represent positions, velocities, sizes, etc in 2D space.
//struct float2
//{
//    float x;
//    float y;
//    void operator =(float b)
//    {
//        x = b;
//        y = b;
//    }
//};
float2 ctor_float2(float x, float y)
{
    float2 result;
    result.x = x;
    result.y = y;
    return result;
}
#define float2(x, y) ctor_float2((x), (y))
void operator +=(float2& a, float2 b) { a.x += b.x; a.y += b.y;}
void operator -=(float2& a, float2 b) { a.x -= b.x; a.y -= b.y;}
void operator *=(float2& a, float2 b) { a.x *= b.x, a.y *= b.y;}
void operator /=(float2& a, float2 b) { a.x /= b.x, a.y /= b.y;}
void operator +=(float2& a, float  b) { a.x += b, a.y += b;}
void operator -=(float2& a, float  b) { a.x -= b, a.y -= b;}
void operator *=(float2& a, float  b) { a.x *= b, a.y *= b;}
void operator /=(float2& a, float  b) { a.x /= b, a.y /= b;}
float2 operator +(float2 a, float2 b) { return float2(a.x + b.x, a.y + b.y); }
float2 operator +(float2 a, float b) { return float2(a.x + b, a.y + b); }
float2 operator +(float a, float2 b) { return float2(a + b.x, a + b.y); }
float2 operator -(float2 a, float2 b) { return float2(a.x - b.x, a.y - b.y); }
float2 operator -(float2 a, float b) { return float2(a.x - b, a.y - b); }
float2 operator -(float a, float2 b) { return float2(a - b.x, a - b.y); }
float2 operator -(float2 b) { return float2(-b.x, -b.y); }
float2 operator *(float2 a, float2 b) { return float2(a.x * b.x, a.y * b.y); }
float2 operator *(float2 a, float b) { return float2(a.x * b, a.y * b); }
float2 operator *(float a, float2 b) { return float2(a * b.x, a * b.y); }
float2 operator /(float2 a, float2 b) { return float2(a.x / b.x, a.y / b.y); }
float2 operator /(float2 a, float b) { return float2(a.x / b, a.y / b); }
float2 operator /(float a, float2 b) { return float2(a / b.x, a / b.y); }


// FLOAT3
// Three-dimensional vector, used to represnt things like positions, colors, velocities, bounding sizes, etc in 3D.
//struct float3
//{
//    float x;
//    float y;
//    float z;
//    void operator =(float b)
//    {
//        x = b;
//        y = b;
//        z = b;
//    }
//};

float3 ctor_float3(float x, float y, float z)
{
    float3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}
#define float3(x, y, z) ctor_float3((x), (y), (z))

// takes the bit-pattern of an int and smashes it into a float
float3 float3_coerce(int x, int y, int z)
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

void operator +=(float3& a, float3 b) { a.x += b.x; a.y += b.y; a.z += b.z; }
void operator -=(float3& a, float3 b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; }
void operator *=(float3& a, float3 b) { a.x *= b.x, a.y *= b.y, a.z *= b.z; }
void operator /=(float3& a, float3 b) { a.x /= b.x, a.y /= b.y, a.z /= b.z; }
void operator +=(float3& a, float  b) { a.x += b, a.y += b, a.z += b; }
void operator -=(float3& a, float  b) { a.x -= b, a.y -= b, a.z -= b; }
void operator *=(float3& a, float  b) { a.x *= b, a.y *= b, a.z *= b; }
void operator /=(float3& a, float  b) { a.x /= b, a.y /= b, a.z /= b; }
float3 operator +(float3 a, float3 b) { return float3(a.x + b.x, a.y + b.y, a.z + b.z); }
float3 operator +(float3 a, float b) { return float3(a.x + b, a.y + b, a.z + b); }
float3 operator +(float a, float3 b) { return float3(a + b.x, a + b.y, a + b.z); }
float3 operator -(float3 a, float3 b) { return float3(a.x - b.x, a.y - b.y, a.z - b.z); }
float3 operator -(float3 a, float b) { return float3(a.x - b, a.y - b, a.z - b); }
float3 operator -(float a, float3 b) { return float3(a - b.x, a - b.y, a - b.z); }
float3 operator *(float3 a, float3 b) { return float3(a.x * b.x, a.y * b.y, a.z * b.z); }
float3 operator *(float3 a, float b) { return float3(a.x * b, a.y * b, a.z * b); }
float3 operator *(float a, float3 b) { return float3(a * b.x, a * b.y, a * b.z); }
float3 operator /(float3 a, float3 b) { return float3(a.x / b.x, a.y / b.y, a.z / b.z); }
float3 operator /(float3 a, float b) { return float3(a.x / b, a.y / b, a.z / b); }
float3 operator /(float a, float3 b) { return float3(a / b.x, a / b.y, a / b.z); }
bool operator ==(float3 a, float3 b) { return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z)); }
float3 operator -(float3 a) {return float3(-a.x, -a.y, -a.z);} // Negate


// fixed point math based on libfixmath
struct fixed
{
    int32 value;

    fixed() {}

    fixed(double a) { value = (int32)(a * (1 << 16)); }
    fixed(float a) { value = (int32)(a * (1 << 16)); }
    fixed(int32 a) { value = (a * (1 << 16)); } //int_to_fixed

    void operator =(double a) { value = (int32)(a * (1 << 16)); }
    void operator =(int32 a) { value = (a * (1 << 16)); }
};
// bitpattern-cast an int to a fixed-point value.
fixed fixed_literal(int32 a)
{
    fixed result = fixed();
    result.value = a;
    return result;
}
int fixed_to_int(fixed a)
{
    return a.value / (1 << 16);
}
fixed fixed_mul(fixed a, fixed b)
{
    // Convert to int64 to stop it from overflowing. 
    // also divide instead of just shifting by 16 bits because bit-shifting negative numbers is undefined.
    return fixed_literal((int32)(((int64)a.value * (int64)b.value) / (1 << 16)));
}
fixed fixed_div(fixed a, fixed b)
{
    return fixed_literal((int32)(((int64)a.value * (1 << 16)) / b.value));
}
void operator +=(fixed& a, fixed b) { a = fixed_literal(a.value + b.value); }
void operator -=(fixed& a, fixed b) { a = fixed_literal(a.value - b.value); }
void operator *=(fixed& a, fixed b) { a = fixed_mul(a, b); }
void operator /=(fixed& a, fixed b) { a = fixed_div(a, b); }
fixed operator +(fixed a, fixed b)  { return fixed_literal(a.value + b.value); }
fixed operator -(fixed a, fixed b)  { return fixed_literal(a.value - b.value); }
fixed operator -(fixed b)           { return fixed_literal(-b.value); }
fixed operator *(fixed a, fixed b)  { return fixed_mul(a, b); }
fixed operator /(fixed a, fixed b)  { return fixed_div(a, b); }
bool operator ==(fixed a, fixed b)  { return a.value == b.value; }
bool operator <(fixed a, fixed b)   { return a.value < b.value; }
bool operator >(fixed a, fixed b)   { return a.value > b.value; }
bool operator <=(fixed a, fixed b)  { return a.value <= b.value; }
bool operator >=(fixed a, fixed b)  { return a.value >= b.value; }
double fixed_to_double(fixed a)
{
    return ((double)a.value / (double)(1 << 16));
}
float fixed_to_float(fixed a)
{
    return ((float)a.value / (float)(1 << 16));
}
fixed sqrt(fixed a)
{
    if (a <= fixed(0))
        return 0;
    
    fixed val = 2;

    fixed x = a;
    fixed y = 1;
    x = (x + y) / val;
    y = a / x;
    x = (x + y) / val;
    y = a / x;
    x = (x + y) / val;
    y = a / x;
    x = (x + y) / val;
    y = a / x;
    x = (x + y) / val;
    y = a / x;
    x = (x + y) / val;
    y = a / x;
    x = (x + y) / val;
    y = a / x;
    x = (x + y) / val;
    y = a / x;
    x = (x + y) / val;
    y = a / x;
    return x;
}
fixed abs(fixed a) { return (a < 0) ? -a : a; }
fixed fixed_fmod(fixed x, fixed y) {
    if (x < 0)
        return y - abs(x - (y * fixed_to_int(x / y)));
    else
        return abs(x - (y * fixed_to_int(x / y)));
}
fixed sin(fixed x)
{
    fixed fixedpi = fixed(3.14152128);
    // Wrapping
    fixed o = abs(fixed_fmod(x - fixedpi * fixed(0.5), fixedpi * fixed(2.0)) - fixedpi) - fixedpi * fixed(0.5);

    // Taylor series
    fixed p = ((o) / fixed(1.0)) -
        ((o * o * o) / fixed(6.0)) +
        ((o * o * o * o * o) / fixed(120.0)) -
        ((o * o * o * o * o * o * o) / fixed(5040.0)) +
        ((o * o * o * o * o * o * o * o * o) / fixed(362880.0)) -
        ((o * o * o * o * o * o * o * o * o * o * o) / fixed(39916800.0));

    return p;
}
fixed cos(fixed n)
{
    fixed fixedpi = fixed(3.14152128);
    return sin(n + fixedpi * fixed(0.5));
}
// Two-component vector, fixed2. Used to represent positions, velocities, sizes, etc in 2D space.
struct fixed2
{
    fixed x;
    fixed y;
    void operator =(fixed b)
    {
        x = b;
        y = b;
    }
};
fixed2 ctor_fixed2(fixed x, fixed y)
{
    fixed2 result;
    result.x = x;
    result.y = y;
    return result;
}
#define fixed2(x, y) ctor_fixed2((x), (y))
void operator +=(fixed2& a, fixed2 b) { a.x += b.x; a.y += b.y; }
void operator -=(fixed2& a, fixed2 b) { a.x -= b.x; a.y -= b.y; }
void operator *=(fixed2& a, fixed2 b) { a.x *= b.x, a.y *= b.y; }
void operator /=(fixed2& a, fixed2 b) { a.x /= b.x, a.y /= b.y; }
void operator +=(fixed2& a, fixed  b) { a.x += b, a.y += b; }
void operator -=(fixed2& a, fixed  b) { a.x -= b, a.y -= b; }
void operator *=(fixed2& a, fixed  b) { a.x *= b, a.y *= b; }
void operator /=(fixed2& a, fixed  b) { a.x /= b, a.y /= b; }
fixed2 operator +(fixed2 a, fixed2 b) { return fixed2(a.x + b.x, a.y + b.y); }
fixed2 operator +(fixed2 a, fixed b) { return fixed2(a.x + b, a.y + b); }
fixed2 operator +(fixed a, fixed2 b) { return fixed2(a + b.x, a + b.y); }
fixed2 operator -(fixed2 a, fixed2 b) { return fixed2(a.x - b.x, a.y - b.y); }
fixed2 operator -(fixed2 a, fixed b) { return fixed2(a.x - b, a.y - b); }
fixed2 operator -(fixed a, fixed2 b) { return fixed2(a - b.x, a - b.y); }
fixed2 operator -(fixed2 b) { return fixed2(-b.x, -b.y); }
fixed2 operator *(fixed2 a, fixed2 b) { return fixed2(a.x * b.x, a.y * b.y); }
fixed2 operator *(fixed2 a, fixed b) { return fixed2(a.x * b, a.y * b); }
fixed2 operator *(fixed a, fixed2 b) { return fixed2(a * b.x, a * b.y); }
fixed2 operator /(fixed2 a, fixed2 b) { return fixed2(a.x / b.x, a.y / b.y); }
fixed2 operator /(fixed2 a, fixed b) { return fixed2(a.x / b, a.y / b); }
fixed2 operator /(fixed a, fixed2 b) { return fixed2(a / b.x, a / b.y); }
bool operator ==(fixed2 a, fixed2 b) { return ((a.x == b.x) && (a.y == b.y)); }

float2 fixed2_to_float2(fixed2 a)
{
    return float2(fixed_to_float(a.x), fixed_to_float(a.y));
}


// FIXED3
// fixed point three-component vector
struct fixed3
{
    fixed x;
    fixed y;
    fixed z;
    void operator =(fixed b)
    {
        x = b;
        y = b;
        z = b;
    }
};

fixed3 ctor_fixed3(fixed x, fixed y, fixed z)
{
    fixed3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}
#define fixed3(x, y, z) ctor_fixed3(fixed(x), fixed(y), fixed(z))
// takes the bit-pattern of an int and smashes it into a fixed
fixed3 fixed3_coerce(int x, int y, int z)
{
    fixed3 result;
    result.x = *(fixed*)&x;
    result.y = *(fixed*)&y;
    result.z = *(fixed*)&z;
    return result;
}
void operator +=(fixed3& a, fixed3 b) { a.x += b.x; a.y += b.y; a.z += b.z; }
void operator -=(fixed3& a, fixed3 b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; }
void operator *=(fixed3& a, fixed3 b) { a.x *= b.x, a.y *= b.y, a.z *= b.z; }
void operator /=(fixed3& a, fixed3 b) { a.x /= b.x, a.y /= b.y, a.z /= b.z; }
void operator +=(fixed3& a, fixed  b) { a.x += b, a.y += b, a.z += b; }
void operator -=(fixed3& a, fixed  b) { a.x -= b, a.y -= b, a.z -= b; }
void operator *=(fixed3& a, fixed  b) { a.x *= b, a.y *= b, a.z *= b; }
void operator /=(fixed3& a, fixed  b) { a.x /= b, a.y /= b, a.z /= b; }
fixed3 operator +(fixed3 a, fixed3 b) { return fixed3(a.x + b.x, a.y + b.y, a.z + b.z); }
fixed3 operator +(fixed3 a, fixed b) { return fixed3(a.x + b, a.y + b, a.z + b); }
fixed3 operator +(fixed a, fixed3 b) { return fixed3(a + b.x, a + b.y, a + b.z); }
fixed3 operator -(fixed3 a, fixed3 b) { return fixed3(a.x - b.x, a.y - b.y, a.z - b.z); }
fixed3 operator -(fixed3 a, fixed b) { return fixed3(a.x - b, a.y - b, a.z - b); }
fixed3 operator -(fixed a, fixed3 b) { return fixed3(a - b.x, a - b.y, a - b.z); }
fixed3 operator *(fixed3 a, fixed3 b) { return fixed3(a.x * b.x, a.y * b.y, a.z * b.z); }
fixed3 operator *(fixed3 a, fixed b) { return fixed3(a.x * b, a.y * b, a.z * b); }
fixed3 operator *(fixed a, fixed3 b) { return fixed3(a * b.x, a * b.y, a * b.z); }
fixed3 operator /(fixed3 a, fixed3 b) { return fixed3(a.x / b.x, a.y / b.y, a.z / b.z); }
fixed3 operator /(fixed3 a, fixed b) { return fixed3(a.x / b, a.y / b, a.z / b); }
fixed3 operator /(fixed a, fixed3 b) { return fixed3(a / b.x, a / b.y, a / b.z); }
bool operator ==(fixed3 a, fixed3 b) { return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z)); }
fixed3 operator -(fixed3 a) { return fixed3(-a.x, -a.y, -a.z); } // Negate

float3 fixed3_to_float3(fixed3 a)
{
    return float3(fixed_to_float(a.x), fixed_to_float(a.y), fixed_to_float(a.z));
}


//#undef min
//#undef max
//int min(int x, int y) { if (y < x) { return y; } else { return x; } };
float min(float x, float y) { return game_fminf(x, y); }
float min(float x, float y, float z) { return min(x, min(y, z)); }
fixed min(fixed x, fixed y) { return (x < y) ? x : y; }
float2 min(float2 a, float2 b) { return float2(min(a.x, b.x), min(a.y, b.y)); }
float3 min(float3 a, float3 b) { return float3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)); }
float min(float2 a) { return min(a.x, a.y); }
float min(float3 a) { return min(a.x, min(a.y, a.z)); }

//int max(int x, int y) { if (y > x) { return y; } else { return x; } };
float max(float x, float y) { return game_fmaxf(x, y); }
float max(float x, float y, float z) { return max(x, max(y, z)); }
fixed max(fixed x, fixed y) { return (x > y) ? x : y; }
float2 max(float2 a, float2 b) { return float2(max(a.x, b.x), max(a.y, b.y)); }
float2 max(float2 a, float b) { return float2(max(a.x, b), max(a.y, b)); }
float2 max(float a, float2 b) { return float2(max(a, b.x), max(a, b.y)); }
float3 max(float3 a, float3 b) { return float3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)); }
float max(float2 a) { return max(a.x, a.y); }
float max(float3 a) { return max(a.x, max(a.y, a.z)); }

//int     clamp(int a,    int    minimum, int    maximim) { return min(max(a, minimum), maximim); }
float   clamp(float a, float  minimum, float  maximim) { return min(max(a, minimum), maximim); }
fixed   clamp(fixed a, fixed  minimum, fixed  maximim) { return min(max(a, minimum), maximim); }
float2  clamp(float2 a, float  minimum, float  maximim) { return min(max(a, float2(minimum, minimum)), float2(maximim, maximim)); }
float2  clamp(float2 a, float2 minimum, float2 maximum) { return min(max(a, minimum), maximum); }
float3  clamp(float3 a, float3 minimum, float3 maximum) { return min(max(a, minimum), maximum); }

float lerp(float a, float b, float f) { return a + f * (b - a); }
float2 lerp(float2 a, float2 b, float t) { return float2(lerp(a.x, b.x, t), lerp(a.y, b.y, t)); }
float3 lerp(float3 a, float3 b, float t) { return float3(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t)); }
float3 lerp(float3 a, float b, float t) { return float3(lerp(a.x, b, t), lerp(a.y, b, t), lerp(a.z, b, t)); }
float3 lerp(float a, float3 b, float t) { return float3(lerp(a, b.x, t), lerp(a, b.y, t), lerp(a, b.z, t)); }
float3 lerp(float3 a, float3 b, float3 t) { return float3(lerp(a.x, b.x, t.x), lerp(a.y, b.y, t.y), lerp(a.z, b.z, t.z)); }

fixed lerp(fixed a, fixed b, fixed f) { return a + f * (b - a); }
fixed2 lerp(fixed2 a, fixed2 b, fixed t) { return fixed2(lerp(a.x, b.x, t), lerp(a.y, b.y, t)); }
fixed3 lerp(fixed3 a, fixed3 b, fixed t) { return fixed3(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t)); }
fixed3 lerp(fixed3 a, fixed b, fixed t) { return fixed3(lerp(a.x, b, t), lerp(a.y, b, t), lerp(a.z, b, t)); }
fixed3 lerp(fixed a, fixed3 b, fixed t) { return fixed3(lerp(a, b.x, t), lerp(a, b.y, t), lerp(a, b.z, t)); }
fixed3 lerp(fixed3 a, fixed3 b, fixed3 t) { return fixed3(lerp(a.x, b.x, t.x), lerp(a.y, b.y, t.y), lerp(a.z, b.z, t.z)); }

fixed inverselerp(fixed a, fixed b, fixed f) { return (f - a) / (b - a); }
float inverselerp(float a, float b, float f) { return (f - a) / (b - a); }

// intiger lerp, fast and good for blending color values.
uint8 lerp(uint8 a, uint8 b, uint8 f) { return (a * (256 - f) + (b * f)) >> 8; }

float smoothstep(float x) { return x * x * (3 - 2 * x); }
fixed smoothstep(fixed x) { return x * x * (3 - 2 * x); }
float2 smoothstep(float2 a) { return float2(smoothstep(a.x), smoothstep(a.y)); }
float3 smoothstep(float3 a) { return float3(smoothstep(a.x), smoothstep(a.y), smoothstep(a.z)); }
fixed3 smoothstep(fixed3 a) { return fixed3(smoothstep(a.x), smoothstep(a.y), smoothstep(a.z)); }

float clamp01(float a) { return min(max(a, 0.0f), 1.0f); }
float2 clamp01(float2 a) { return float2(clamp01(a.x), clamp01(a.y)); }
float3 clamp01(float3 a) { return float3(clamp01(a.x), clamp01(a.y), clamp01(a.z)); }

inline float step(float a, float x)
{
    return (x >= a) ? 1 : 0;
}
inline float3 step(float3 a, float3 x)
{
    return float3(step(a.x, x.x), step(a.y, x.y), step(a.z, x.z));
}
inline float2 step(float2 a, float2 x)
{
    return float2(step(a.x, x.x), step(a.y, x.y));
}

//float abs(float x) noexcept { return game_fabsf(x); }
// These functions are defined in math.h on the ESP32/Andoir, so we don't need to define them here.
#ifndef XR_USE_PLATFORM_ANDROID
float abs(float x) { return game_fabsf(x); }
float floor(float x) { return game_floorf(x); }
float ceil(float x) { return game_ceilf(x); }
float round(float x) { return game_roundf(x); }
float acos(float x) { return game_acosf(x); }
float sin(float x) { return game_sinf(x); }
float cos(float x) { return game_cosf(x); }
float sinTurns(float x) { return game_sinf(x * 3.14152128f * 2.0f); }
float cosTurns(float x) { return game_cosf(x * 3.14152128f * 2.0f); }
float sqrt(float x) { return game_sqrtf(x); }
float atan2(float x, float y)
{
    return game_atan2(x, y);
}
#endif


float2 abs(float2 a) { return float2(game_fabsf(a.x), game_fabsf(a.y)); }
float3 abs(float3 a){return float3(game_fabsf(a.x), game_fabsf(a.y), game_fabsf(a.z));}

float sign(float a) { return a > 0 ? 1 : -1; }
fixed sign(fixed a) { return a > 0 ? fixed(1) : fixed(-1); }
float2 sign(float2 a) { return float2(sign(a.x), sign(a.y)); }
float3 sign(float3 a){return float3(sign(a.x), sign(a.y), sign(a.z));}


fixed floor(fixed v) { return (v.value < 0) ? fixed_to_int(v - fixed(1)) : fixed_to_int(v); }
float2 floor(float2 v) { return float2(floor(v.x), floor(v.y)); }
float3 floor(float3 v) { return float3(floor(v.x), floor(v.y), floor(v.z)); }

fixed ceil(fixed v) { return (v.value < 0) ? fixed_to_int(v) : fixed_to_int(v + fixed(1)); }
float2 ceil(float2 v) { return float2(ceil(v.x), ceil(v.y)); }
float3 ceil(float3 v) { return float3(ceil(v.x), ceil(v.y), ceil(v.z)); }


float2 round(float2 v) { return float2(round(v.x), round(v.y)); }
float3 round(float3 v) { return float3(round(v.x), round(v.y), round(v.z)); }

fixed frac(fixed a) { return a - floor(a); }
fixed2 frac(fixed2 a) { return fixed2(frac(a.x), frac(a.y)); }
fixed3 frac(fixed3 a) { return fixed3(frac(a.x), frac(a.y), frac(a.z)); }
float frac(float a) { return a - floor(a); }
float2 frac(float2 a) { return float2(frac(a.x), frac(a.y)); }
float3 frac(float3 a) { return float3(frac(a.x), frac(a.y), frac(a.z)); }

float2 acos(float2 v) { return float2(game_acosf(v.x), game_acosf(v.y)); }
float3 acos(float3 v) { return float3(game_acosf(v.x), game_acosf(v.y), game_acosf(v.z)); }

float2 sin(float2 v) { return float2(game_sinf(v.x), game_sinf(v.y)); }
float3 sin(float3 v) { return float3(game_sinf(v.x), game_sinf(v.y), game_sinf(v.z)); }

float2 cos(float2 v) { return float2(game_cosf(v.x), game_cosf(v.y)); }
float3 cos(float3 v) { return float3(game_cosf(v.x), game_cosf(v.y), game_cosf(v.z)); }

float2 sqrt(float2 v) { return float2(sqrt(v.x), sqrt(v.y)); }
float3 sqrt(float3 v) { return float3(sqrt(v.x), sqrt(v.y), sqrt(v.z)); }

float mod(float a, float b) { return frac(a / b) * b; }

int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

float dot(float2 a, float2 b) { return a.x * b.x + a.y * b.y; }
float dot(float3 a, float3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
fixed dot(fixed2 a, fixed2 b) { return a.x * b.x + a.y * b.y; }
fixed dot(fixed3 a, fixed3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

float length(float2 a)
{
    float d = dot(a, a);
    if (d == 0)
        return 0;
    return sqrt(d);
}

float length(float3 a)
{
    float d = dot(a, a);
    if (d == 0)
        return 0;
    return sqrt(d);
}

fixed length(fixed2 a)
{
    fixed d = dot(a, a);
    if (d == 0)
        return 0;
    return sqrt(d);
}

fixed length(fixed3 a)
{
    fixed d = dot(a, a);
    if (d == 0)
        return 0;
    return sqrt(d);
}

float distanceSquared(float2 Start, float2 End) { return dot(Start - End, Start - End); }
float distance(float2 Start, float2 End) { return length(Start - End); }
float distance(float3 Start, float3 End) { return length(Start - End); }
fixed distance(fixed3 Start, fixed3 End) { return length(Start - End); }
fixed distance(fixed2 Start, fixed2 End) { return length(Start - End); }

float2 normalize(float2 a) { return a / length(a); }
float3 normalize(float3 a) { return a / length(a); }
fixed2 normalize(fixed2 a) { 
    if (length(a) == 0)
        return fixed2(1, 0);
    return a / length(a); 
}
fixed3 normalize(fixed3 a) {
    if (length(a) == 0)
        return fixed3(1, 0, 0); 
    return a / length(a); }
float3 normalizeFast(float3 a) { return a / sqrtFast(dot(a, a)); }
float remap(float t, float iMin, float iMax, float oMin, float oMax)
{
    t = inverselerp(iMin, iMax, t);
    return lerp(oMin, oMax, t);
}
float remap01(float t, float iMin, float iMax)
{
    return inverselerp(iMin, iMax, t);
}

// Gaussian / bell-curve function.
float gaussian(float x, float Width, float Height)
{
    return pow(2, -pow(x / Width, 2)) * Height;
}


float2 reflect(float2 a, float2 normal)
{
    // How tf can a matrix be its own inverse.
    // ??????????????????
    float2 up = -float2(-normal.y, normal.x);
    float2 right = float2(normal.x, normal.y);
    float2 local_space_vector = float2(-dot(a, right), dot(a, up));
    return float2(dot(local_space_vector, right), dot(local_space_vector, up));
}
float rand(float2 co)
{
    return frac(sin(dot(co, float2(12.9898, 78.233))) * 43758.5453);
}

float2 rotate(float2 v, float angle)
{
    float x = v.x * cos(angle * tau) - v.y * sin(angle * tau);
    float y = v.x * sin(angle * tau) + v.y * cos(angle * tau);
    return float2(x, y);
}

float3 cross(float3 a, float3 b) { return float3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
fixed3 cross(fixed3 a, fixed3 b) { return fixed3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

float3 RotateAroundAxis(float3 v, float3 k, float a)
{
    return v * cos(a * tau) + cross(k, v) * sin(a * tau) + k * dot(k, v) * (1 - cos(a * tau));
}
float3 reflect(float3 v, float3 normal)
{
    return v - 2.0 * normal * dot(v, normal);
}

// TODO: test these
//float MoveTowards(float current, float target, float distance)
//{
//    return current + clamp(target - current, -distance, distance);
//}
//fixed MoveTowards(fixed current, fixed target, fixed distance)
//{
//    return current + clamp(target - current, -distance, distance);
//}
//float2 MoveTowards(float2 current, float2 target, float dist)
//{
//    if (current.x == target.x && current.y == target.y)
//        return current;
//    return current + normalize(target - current) * min(dist, distance(target, current));
//}
//float3 MoveTowards(float3 current, float3 target, float dist)
//{
//    if (current.x == target.x && current.y == target.y && current.z == target.z)
//        return current;
//    return current + normalize(target - current) * min(dist, distance(target, current));
//}
//fixed3 MoveTowards(fixed3 current, fixed3 target, fixed dist)
//{
//    if (current.x == target.x && current.y == target.y && current.z == target.z)
//        return current;
//    return current + normalize(target - current) * min(dist, distance(target, current));
//}
float MoveTowards(float from, float to, float distance) { return from + clamp(to - from, -distance, distance); }
float2 MoveTowards(float2 from, float2 to, float distance) { return from + ((to - from) / length(to - from)) * min(distance, length(to - from)); }
float3 MoveTowards(float3 from, float3 to, float distance) { return from + ((to - from) / length(to - from)) * min(distance, length(to - from)); }

float ToSRGB(float Lin) { return Lin <= .04045 ? Lin / 12.92 : pow((Lin + 0.055) / 1.055, 2.4); }
float2 ToSRGB(float2 Lin) { return float2(ToSRGB(Lin.x), ToSRGB(Lin.y)); }
float3 ToSRGB(float3 Lin) { return float3(ToSRGB(Lin.x), ToSRGB(Lin.y), ToSRGB(Lin.z)); }

float ToGamma(float Lin) { return Lin <= .0031308 ? Lin * 12.92 : 1.055 * pow(Lin, (1.0f / 2.4)) - 0.055; }
float2 ToGamma(float2 Lin) { return float2(ToGamma(Lin.x), ToGamma(Lin.y)); }
float3 ToGamma(float3 Lin) { return float3(ToGamma(Lin.x), ToGamma(Lin.y), ToGamma(Lin.z)); }


float atan2(float2 a)
{
    return game_atan2(a.x, a.y);
}

float AngleBetween(float2 A, float2 B)
{
    return atan2(B) - atan2(A);
}
// atan2 with a 0 - 1 range instead of -pi to pi range
float atan2Turns(float x, float y)
{
    return atan2(float2(x, y)) / 3.14152128 / 2.0 + 0.5f;
}
// atan2 with a 0 - 1 range instead of -pi to pi range
float atan2Turns(float2 a)
{
    return atan2(a) / 3.14152128 / 2.0 + 0.5f;
}

float AngleBetween(float2 A, float2 B)
{
    return atan2(B) - atan2(A);
}

float3 palette(float t, float3 a, float3 b, float3 c, float3 d)
{
    return a + b * cos(6.28318 * (c * frac(t) + d));
}
// Same thing but normalized
float3 paletteN(float t, float3 a, float3 b, float3 c, float3 d)
{
    float3 result = palette(t, a, b, c, d);
    return result / length(result);
}

float smin(float a, float b, float k)
{
    float h = max(k - abs(a - b), 0.0) / k;
    return min(a, b) - h * h * k * (1.0 / 4.0);
}

// TRANSFORM
// 60 bytes.
//struct Transform
//{
//    float3 position;
//    float3 right; // x+
//    float3 forward; // y+
//    float3 up; // z+
//    float3 scale;
//};

const float3 vectorRight    = { 1,  0,  0 };
const float3 vectorLeft     = { -1, 0,  0 };
const float3 vectorForward  = { 0,  1,  0 };
const float3 vectorBack     = { 0, -1,  0 };
const float3 vectorUp       = { 0,  0,  1 };
const float3 vectorDown     = { 0,  0, -1 };
const float3 vectorOne      = { 1,  1,  1 };
const float3 vectorZero     = { 0,  0,  0 };

const Transform transformIdentity = { {0, 0, 0}, { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 1, 1, 1 } };

Transform ctor_transform(float3 position, float3 forward, float3 up, float3 scale)
{
    forward = normalize(forward);
    up = normalize(up);
    Transform result;
    result.position = position;
    result.right = cross(forward, up);
    result.forward = forward;
    result.up = cross(result.right, forward);
    result.scale = scale;
    return result;
}


float3 LocalToWorld(Transform Trans, float3 position)
{
    Trans.right *= Trans.scale.x;
    Trans.forward *= Trans.scale.y;
    Trans.up *= Trans.scale.z;

    float3 BoxRightTransposed = float3(Trans.right.x, Trans.forward.x, Trans.up.x);
    float3 BoxForwardTransposed = float3(Trans.right.y, Trans.forward.y, Trans.up.y);
    float3 BoxUpTransposed = float3(Trans.right.z, Trans.forward.z, Trans.up.z);

    return (Trans.position + float3(
        dot(position, BoxRightTransposed),
        dot(position, BoxForwardTransposed),
        dot(position, BoxUpTransposed)));
}
float3 WorldToLocal(Transform Trans, float3 position)
{
    return float3(
        dot(position - Trans.position, Trans.right / Trans.scale.x),
        dot(position - Trans.position, Trans.forward / Trans.scale.y),
        dot(position - Trans.position, Trans.up / Trans.scale.z));
}

float3 LocalToWorldNoScale(Transform Trans, float3 position)
{
    float3 BoxRightTransposed = float3(Trans.right.x, Trans.forward.x, Trans.up.x);
    float3 BoxForwardTransposed = float3(Trans.right.y, Trans.forward.y, Trans.up.y);
    float3 BoxUpTransposed = float3(Trans.right.z, Trans.forward.z, Trans.up.z);

    return (Trans.position + float3(
        dot(position, BoxRightTransposed),
        dot(position, BoxForwardTransposed),
        dot(position, BoxUpTransposed)));
}
float3 WorldToLocalNoScale(Transform Trans, float3 position)
{
    return float3(
        dot(position - Trans.position, Trans.right),
        dot(position - Trans.position, Trans.forward),
        dot(position - Trans.position, Trans.up));
}

float3 LocalToWorldVector(Transform Trans, float3 position)
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
float3 WorldToLocalVector(Transform Trans, float3 position)
{
    return float3(
        dot(position, Trans.right / Trans.scale.x),
        dot(position, Trans.forward / Trans.scale.y),
        dot(position, Trans.up / Trans.scale.z));
}

float3 LocalToWorldVectorNoScale(Transform Trans, float3 position)
{
    float3 BoxRightTransposed = float3(Trans.right.x, Trans.forward.x, Trans.up.x);
    float3 BoxForwardTransposed = float3(Trans.right.y, Trans.forward.y, Trans.up.y);
    float3 BoxUpTransposed = float3(Trans.right.z, Trans.forward.z, Trans.up.z);

    return float3(
        dot(position, BoxRightTransposed),
        dot(position, BoxForwardTransposed),
        dot(position, BoxUpTransposed));
}
float3 WorldToLocalVectorNoScale(Transform Trans, float3 position)
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

Transform rotate(Transform t, float3 Axis, float angle)
{
    t.forward = RotateAroundAxis(t.forward, Axis, angle);
    t.up = RotateAroundAxis(t.up, Axis, angle);
    t.right = cross(t.forward, t.up);

    t.forward = normalize(t.forward);
    t.up = normalize(t.up);
    t.right = normalize(t.right);
    return t;
}
Transform rotate(Transform t, float X, float Y, float Z)
{
    t = rotate(t, t.right, X);
    t = rotate(t, t.forward, Y);
    t = rotate(t, t.up, Z);
    return t;
}
Transform transform(float3 position)
{
    return ctor_transform(position, float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1));
}
Transform transform(float3 position, float3 scale)
{
    return ctor_transform(position, float3(0, 1, 0), float3(0, 0, 1), scale);
}
Transform transform(float3 position, float x, float y, float z)
{
    return rotate(ctor_transform(position, float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1)), x, y, z);
}
Transform transform(float3 position, float x, float y, float z, float3 scale)
{
    return rotate(ctor_transform(position, float3(0, 1, 0), float3(0, 0, 1), scale), x, y, z);
}
#define Transform(position, forward, up, scale) ctor_transform((position), (forward), (up), (scale))


Transform LookRotation(Transform t, float3 forward, float3 up)
{
    t.forward = normalize(forward);
    t.right = normalize(cross(forward, up));
    t.up = normalize(cross(t.right, forward));
    return t;
}

float2 rotate90CW(float2 a)
{
    return float2(-a.y, a.x);
}
float2 rotate90CXW(float2 a)
{
    return float2(a.y, -a.x);
}


float DragBlend(float x, float d)
{
    return (1.0f - (1.0f / ((x * d) + 1.0f)));
}

float3 analyticConstantVelocity(float3 a, float x, float d)
{
    d = max(d, 0.00001f);
    return x * x * a - x * x * a * DragBlend(x, d);
}
float3 analyticConstantVelocity(float3 a, float x)
{
    return a * x * x;
}
float3 analyticInstantVelocity(float3 a, float x, float d)
{
    d = max(d, 0.00001f);
    return (a / d) * DragBlend(x, d);
}
float3 analyticInstantVelocity(float3 a, float x)
{
    return a * x;
}



float3 VectorToPolar(float3 Normal)
{
    float u = atan2(Normal.x, Normal.y) / (3.141532f * 2.0f);
    float3 p = normalize(float3(Normal.x, Normal.y, 0));
    float a = dot(Normal, p);
    float v = acos(a);
    if (Normal.z > 0) v *= -1;
    v /= (3.14152128 * 0.5);
    v = v * 0.5 + 0.5;

    return float3(frac(u), frac(v), 0);
}
// X and Y are UVs, Z is face index
float3 RayCubemap(float3 v)
{
#if 1
    float3 vAbs = abs(v);
    float3 habs = 0.5f / vAbs;
    if (vAbs.z >= vAbs.x && vAbs.z >= vAbs.y)
    {
        return float3(
            (v.z < 0.0f ? -v.x : v.x) * (0.5f / vAbs.z) + 0.5f,
            (-v.y) * (0.5f / vAbs.z) + 0.5f,
            v.z < 0.0f ? 5.0f : 4.0f);
    }
    else if (vAbs.y >= vAbs.x)
    {
        return float3(
            (v.y < 0.0f ? v.x : -v.x) * (0.5f / vAbs.y) + 0.5f,
            (-v.z) * (0.5f / vAbs.y) + 0.5f,
            v.y < 0.0f ? 3.0f : 2.0f);
    }
    else
    {
        return float3(
            (v.x < 0.0f ? -v.y : v.y) * (0.5f / vAbs.x) + 0.5f,
            (-v.z) * (0.5f / vAbs.x) + 0.5f,
            v.x < 0.0f ? 1.0f : 0.0f);
    }
#else
    float3 vAbs = abs(v);
    float3 habs = 0.5 / vAbs;
    if (vAbs.z >= vAbs.x && vAbs.z >= vAbs.y)
    {
        return float3(
            (v.z < 0.0 ? -v.x : v.x) * (0.5 / vAbs.z) + 0.5,
            (-v.y) * (0.5 / vAbs.z) + 0.5,
            v.z < 0.0 ? 5.0 : 4.0);
    }
    else if (vAbs.y >= vAbs.x)
    {
        return float3(
            (v.y < 0.0 ? v.x : -v.x) * (0.5 / vAbs.y) + 0.5,
            (-v.z) * (0.5 / vAbs.y) + 0.5,
            v.y < 0.0 ? 3.0 : 2.0);
    }
    else
    {
        return float3(
            (v.x < 0.0 ? -v.y : v.y) * (0.5 / vAbs.x) + 0.5,
            (-v.z) * (0.5 / vAbs.x) + 0.5,
            v.x < 0.0 ? 1.0 : 0.0);
    }
#endif
}


float3 RaySphereUV(float3 Normal)
{
    return RayCubemap(Normal);
}



float GetT(float t, float alpha, float3 p0, float3 p1)
{
    auto d = p1 - p0;
    float b = pow(dot(d, d), alpha * 0.5f);
    return (b + t);
}

float3 CatmullRom(float3 p0, float3 p1, float3 p2, float3 p3, float t, float alpha = .5f)
{
    float t0 = 0.0f;
    float t1 = GetT(t0, alpha, p0, p1);
    float t2 = GetT(t1, alpha, p1, p2);
    float t3 = GetT(t2, alpha, p2, p3);

    t = lerp(t1, t2, t);

    float3 A1 = (t1 - t) / (t1 - t0) * p0 + (t - t0) / (t1 - t0) * p1;
    float3 A2 = (t2 - t) / (t2 - t1) * p1 + (t - t1) / (t2 - t1) * p2;
    float3 A3 = (t3 - t) / (t3 - t2) * p2 + (t - t2) / (t3 - t2) * p3;

    float3 B1 = (t2 - t) / (t2 - t0) * A1 + (t - t0) / (t2 - t0) * A2;
    float3 B2 = (t3 - t) / (t3 - t1) * A2 + (t - t1) / (t3 - t1) * A3;

    return (t2 - t) / (t2 - t1) * B1 + (t - t1) / (t2 - t1) * B2;
}



float GetSmoothedValue(float data[100], int* index, float newTime, int count)
{
    (*index)++;
    (*index) %= count;
    data[*index] = newTime;
    float accumulatedDelta = 0;
    for (int i = 0; i < count; i++)
    {
        accumulatedDelta += data[i];
    }
    accumulatedDelta /= count;
    return accumulatedDelta;
}

// Spiral Blur
// https://www.youtube.com/watch?v=lOIP_Z_-0Hs
// Thanks vihart :>
float2 GoldenSpiral(float step)
{
    float PhiRadians = 2.3998277;
    return float2(sin(step * PhiRadians) * step, cos(step * PhiRadians) * step);
}

// Call this in a loop to get UV offsets
float2 GetSpiralBlurUVOffset(float Radius, float Samples, float i)
{
    return GoldenSpiral(i) / Samples * Radius;
}

float GetSpiralBlurWeight(float Samples, float i)
{
    Samples += 0.01f;
    return (1.0f - (i / Samples)) / Samples * 2.0;
}
