#pragma once
//#include <immintrin.h>

// Two-component vector, float2. Used to represent positions, velocities, sizes, etc in 2D space.
typedef struct
{
    float x;
    float y;
} float2;
float2 float2Ctor(float x, float y){return (float2){x,y};}
float2 float2Default(){return (float2){0,0};}
#define float2(x, y) float2Ctor((x), (y))

typedef struct 
{
    union
    {
        float2 xy;
        struct 
        {
            float x;
            float y;
        };
    };
    float z;
} float3;
float3 float3Ctor(float x, float y, float z){return (float3){x,y,z};}
float3 float3Default(){return (float3){0,0,0};}
#define float3(x, y, z) float3Ctor((x), (y), (z))

const float3 float3Right    = { 1,  0,  0 };
const float3 float3Left     = { -1, 0,  0 };
const float3 float3Forward  = { 0,  1,  0 };
const float3 float3Back     = { 0, -1,  0 };
const float3 float3Up       = { 0,  0,  1 };
const float3 float3Down     = { 0,  0, -1 };

typedef struct 
{
    union
    {
        struct 
        {
            float2 xy;
            float2 zw;
        };
        struct 
        {
            float x;
            float y;
            float z;
            float w;
        };
        struct
        {
            float3 xyz;
            float q;
        };
    };
} float4;
float4 float4Ctor(float x, float y, float z, float w){return (float4){x,y,z,w};}
float4 float4Default(){return (float4){0,0,0,0};}
#define float4(x, y, z, w) float4Ctor((x), (y), (z), (w))

float2 add2(float2 a, float2 b) { return (float2){a.x + b.x, a.y + b.y}; }
float2 sub2(float2 a, float2 b) { return (float2){a.x - b.x, a.y - b.y}; }
float2 mul2(float2 a, float2 b) { return (float2){a.x * b.x, a.y * b.y}; }
float2 div2(float2 a, float2 b) { return (float2){a.x / b.x, a.y / b.y}; }

float3 add3(float3 a, float3 b) { return float3(a.x + b.x, a.y + b.y, a.z + b.z); }
float3 sub3(float3 a, float3 b) { return float3(a.x - b.x, a.y - b.y, a.z - b.z); }
float3 mul3(float3 a, float3 b) { return float3(a.x * b.x, a.y * b.y, a.z * b.z); }
float3 div3(float3 a, float3 b) { return float3(a.x / b.x, a.y / b.y, a.z / b.z); }

float floor(float n) { return (float)(int)(n < 0 ? (n - 1) : n); }
float round(float a) { return floor(a + 0.5f); }
float ceil(float n) { return floor(n + 1); }
#define abs(n) (((n) < 0) ? (-(n)) : (n)) 
float mod(float x, float y) {
    float q = abs(x - (y * (int)(x / y)));
    if(x < 0) return y - q;
    else return q;
}
float tri(float x)
{
    return abs(mod(x, 2.0f)-1.0f);
}
static const float pi = 3.14159265358979323926264338327;
static const float tau = 6.28318530718 ;
float atan2(float x, float y)
{
    float coeff_1 = 3.1415212832 / 4.0f;
    float coeff_2 = 3.0f * coeff_1;
    float abs_y = abs(y);
    float angle;
    if (x >= 0.0f) {
        float r = (x - abs_y) / (x + abs_y);
        angle = coeff_1 - coeff_1 * r;
    }
    else {
        float r = (x + abs_y) / (abs_y - x);
        angle = coeff_2 - coeff_1 * r;
    }
    return (y < 0.0f) ? -angle : angle;
}
float pow(float a, float b)
{
    union {
        double d;
        int x[2];
    } u = { a };
    u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
    u.x[0] = 0;
    return u.d;
}
// counts the number of '1' bits in an int.
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
char nthbit(char c, char n)
{
    unsigned char tmp = 1 << n;
    return (c & tmp) >> n;
}
int asint(float x) { return *((int*)(&x)); }
float asfloat(int x) { return *((float*)(&x)); }
int32 RoundToInt(float x){return (int32)round(x);}
uint32 RoundToUInt(float x){return (uint32)round(x);}
int32 TruncateToInt(float x){return (int32)x;}
int32 FloorToInt(float x){return (int32)floor(x);}
int32 CeilToInt(float x){return (int32)ceil(x);}
int RoundToNearestPowerOfTwo(int v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}
float sin(float x)
{
    // Wrapping
    float o = abs(mod(x - pi * 0.5f, pi * 2.0f) - pi) - pi * 0.5f;

    // Taylor series
    float p = ((o) / 1.0f) -   
                ((o * o * o) / 6.0f) + 
                ((o * o * o * o * o) / 120.0f) -
                ((o * o * o * o * o * o * o) / 5040.0f) +
                ((o * o * o * o * o * o * o * o * o) / 362880.0f) -
                ((o * o * o * o * o * o * o * o * o * o * o) / 39916800.0f);
    
    return p;
}
float cos(float n)
{
    return sin(n + pi * 0.5f);
}
float sqrt(float a)
{
    if (a <= 0)
        return 0;

    float x = a;
    float y = 1.0f;

    for (int i = 0; i < 12; i++)
    {
        x = (x + y) * 0.5f;
        y = a / x;
    }
    
    return x;
}
// surprisingly accurate, BUT NOT ACCURATE ENOUGH
float sqrtfast(float x)
{
    int i = asint(x);
    i = 0x1FBD1DF5 + (i >> 1);
    return asfloat(i);
}
int mini(int x, int y) { if (y < x) { return y; } else { return x; } };
float min(float x, float y) { if (y < x) { return y; } else { return x; } }
int maxi(int x, int y) { if (y > x) { return y; } else { return x; } };
float max(float x, float y) { if (y > x) { return y; } else { return x; } }
float clamp01(float a) { return min(max((a), 0.0f), 1.0f); }
float sign(float a) { return a > 0 ? 1 : -1; }
float smoothstep(float x) { return x * x * (3 - 2 * x); }
int clampi(int a, int minimum, int maximim) { return min(max(a, minimum), maximim); }
float clamp(float a, float minimum, float maximim) { return min(max(a, minimum), maximim); }
uint8 lerp8(uint8 a, uint8 b, uint8 f) { return (a * (256 - f) + (b * f)) >> 8; }
float lerp(float a, float  b, float t) { return a + t * (b - a); }
float inverselerp(float a, float b, float t) { return (t - a) / (b - a); }
float remap(float t, float iMin, float iMax, float oMin, float oMax){t = inverselerp(iMin, iMax, t);return lerp(oMin, oMax, t);}

float dot2(float2 a, float2 b) { return a.x * b.x + a.y * b.y; }
float dot3(float3 a, float3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

float length2(float2 a)
{
    float d = dot2(a, a);
    if (d <= 0)
        return 0;
    return sqrt(d);
}

float length3(float3 a)
{
    float d = dot3(a, a);
    if (d <= 0)
        return 0;
    return sqrt(d);
}
float distancesquared2(float2 a, float2 b) { return dot2(float2(a.x-b.x, a.y-b.y), float2(a.x-b.x, a.y-b.y)); }
float distancesquared3(float3 a, float3 b) { return dot3(float3(a.x-b.x, a.y-b.y, a.z-b.z), float3(a.x-b.x, a.y-b.y, a.z-b.z)); }

float distance2(float2 a, float2 b) { return length2(float2(a.x - b.x, a.y - b.y)); }
float distance3(float3 a, float3 b) { return length3(float3(a.x - b.x, a.y - b.y, a.z - b.z)); }

float2 normalize2(float2 a) { return float2(a.x / length2(a), a.y / length2(a)); }
float3 normalize3(float3 a) { return float3(a.x / length3(a), a.y / length3(a), a.z / length3(a)); }

float2 reflect2(float2 a, float2 normal)
{
    float2 up = float2(-normal.y, normal.x);
    float2 right = float2(normal.x, normal.y);
    float2 local_space_vector = float2(dot2(a, right), dot2(a, up));
    return float2(dot2(local_space_vector, right), dot2(local_space_vector, up));
}
//float3 reflect3(float3 v, float3 normal)
//{
//    return sub(v, mul(mul(2.0, normal), dot(v, normal)));
//}
//
//float rand(float2 uv)
//{
//    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
//}

float2 rotate(float2 v, float angle)
{
    float x = v.x * cos(angle) - v.y * sin(angle);
    float y = v.x * sin(angle) + v.y * cos(angle);
    return float2(x, y);
}

float2 rotate90(float2 v)
{
    return float2(v.y, -v.x);
}
float2 rotate90CCW(float2 v)
{
    return float2(-v.y, v.x);
}

float3 cross(float3 a, float3 b) { return float3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

//float3 RotateAroundAxis(float3 v, float3 k, float a)
//{
//    return add(mul(mul(v, add(cos(a), cross(k, v))), sin(a)), mul(mul(k, dot(k, v)), (1.0f - cos(a))));
//}

//float movetowards(float from, float to, float distance)		{ return from + clamp(to - from, -distance, distance); }
//float2 movetowards2(float2 from, float2 to, float distance)    { return from + ((to - from) / length(to - from)) * min(distance, length(to - from)); }
//float3 movetowards3(float3 from, float3 to, float distance)    { return from + ((to - from) / length(to - from)) * min(distance, length(to - from)); }

typedef struct 
{
    float xMin;
    float yMin;
    float xMax;
    float yMax;
} extents2;
extents2 extents2Ctor(float minX, float minY, float maxX, float maxY){return (extents2){minX, minY, maxX, maxY};}
#define extents2(minX, minY, maxX, maxY) extents2Ctor((minX), (minY), (maxX), (maxY))

typedef struct 
{
    float2 pos;
    float2 size;
} box2;
box2 box2Ctor(float2 pos, float2 size){return (box2){pos, size};}
#define box2(pos, size) box2Ctor((pos), (size))


extents2 BoxToExtents(float2 pos, float2 size)
{
    return extents2(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
}
box2 ExtentsToBox(float2 posMin, float2 posMax)
{
    return box2(posMin, sub2(posMax, posMin));
}

box2 FitBoxInBox(box2 box0, box2 box1)
{
    extents2 extents0 = BoxToExtents(box0.pos, box0.size);
    extents2 extents1 = BoxToExtents(box1.pos, box1.size);
    
    if(extents0.xMin < extents1.xMin) extents0.xMin = extents1.xMin;
    if(extents0.yMin < extents1.yMin) extents0.yMin = extents1.yMin;
    if(extents0.xMax < extents1.xMin) extents0.xMax = extents1.xMin;
    if(extents0.yMax < extents1.yMin) extents0.yMax = extents1.yMin;

    if(extents0.xMax > extents1.xMax) extents0.xMax = extents1.xMax;
    if(extents0.yMax > extents1.yMax) extents0.yMax = extents1.yMax;
    if(extents0.xMin > extents1.xMax) extents0.xMin = extents1.xMax;
    if(extents0.yMin > extents1.yMax) extents0.yMin = extents1.yMax;

    return ExtentsToBox(float2(extents0.xMin, extents0.yMin), float2(extents0.xMax, extents0.yMax));
}

bool IntersectCirclePoint(float2 point, float2 circlePos, float circleRadius)
{
    return distance2(point, circlePos) < circleRadius;
}
bool IntersectCircleCircle(float2 pos0, float radius0, float2 pos1, float radius1)
{
    return distance2(pos0, pos1) < radius0 + radius1;
}
bool IntersectCircleBox(float2 circlePos, float2 circleRadius, float2 boxPos, float2 boxSize)
{
    float2 largeBoxSize = float2(boxSize.x + circleRadius.x * 2.0,
                                 boxSize.y + circleRadius.y * 2.0);
    float p1 = (boxPos.x - largeBoxSize.x * 0.5f);
    float p2 = (boxPos.x + largeBoxSize.x * 0.5f);
    float p3 = (boxPos.y - largeBoxSize.y * 0.5f);
    float p4 = (boxPos.y + largeBoxSize.y * 0.5f);

    return circlePos.x > p1 &&
           circlePos.x < p2 &&
           circlePos.y > p3 &&
           circlePos.y < p4;
}
bool IntersectPointBox(float2 point, float2 boxPos, float2 boxSize)
{
    extents2 extents = BoxToExtents(boxPos, boxSize);
    return (point.x > extents.xMin && 
            point.x < extents.xMax &&
            point.y > extents.yMin && 
            point.y < extents.yMax);

}
bool IntersectBoxBox(float2 box0Pos, float2 box0Size, float2 box1Pos, float2 box1Size)
{
    extents2 extents0 = BoxToExtents(box0Pos, box0Size);
    extents2 extents1 = BoxToExtents(box1Pos, box1Size);
    return (extents0.xMin < extents1.xMax && 
            extents1.xMin < extents0.xMax && 
            extents0.yMin < extents1.yMax && 
            extents1.yMin < extents0.yMax);
}

bool IntersectRayCircle(float2 p, float2 d, float2 circlePos, float r, float2* hit)
{
    d = normalize2(d);
    float q = dot2(sub2(p, circlePos), float2(-d.y, d.x));
    float o = -sqrt(r * r - q * q);
    
    if(o == 0)
        return false;
        
    float x = -d.y * q + d.x * o;
    float y = d.x * q + d.y * o;
    *hit = add2(float2(x, y), circlePos);
    
    return true;
}
bool IntersectRayBoxDistance(float2 rayStart, float2 rayDirection, float2 boxPos, float2 boxSize, float* hitClose, float* hitFar)
{
    rayDirection = normalize2(rayDirection);
    float rx = rayStart.x;
    float ry = rayStart.y;
    float dx = rayDirection.x;
    float dy = rayDirection.y;
    float x = boxPos.x + boxSize.x/2;
    float y = boxPos.y + boxSize.y/2;
    float width = boxSize.x;
    float height = boxSize.y;

    float hwidth = width / 2;
    float hheight = height / 2;
    float px = rx - x;
    float py = ry - y;
    float LeftDistance = (-hwidth - px) / dx;
    float BottomDistance = (-hheight - py) / dy;
    float RightDistance = (hwidth - px) / dx;
    float TopDistance = (hheight - py) / dy;

    float xmax = LeftDistance > RightDistance ? LeftDistance : RightDistance;
    float xmin = LeftDistance < RightDistance ? LeftDistance : RightDistance;
    float ymax = TopDistance > BottomDistance ? TopDistance : BottomDistance;
    float ymin = TopDistance < BottomDistance ? TopDistance : BottomDistance;
    if (xmin > ymax || ymin > xmax)
    {
        return false;
    }
    *hitClose = max(xmin, ymin);
    *hitFar = min(xmax, ymax);
    return true;

}
bool IntersectRayBox(float2 rayStart, float2 rayDirection, float2 boxPos, float2 boxSize, float2* hitClose, float2* hitFar)
{
    float ClosestDistance;
    float FurthestDistance;
    bool hit = IntersectRayBoxDistance(rayStart, rayDirection, boxPos, boxSize, &ClosestDistance, &FurthestDistance);
    if(!hit)
        return false;
    *hitClose = float2(rayStart.x + rayDirection.x * ClosestDistance,
                       rayStart.y + rayDirection.y * ClosestDistance);
    *hitFar   = float2(rayStart.x + rayDirection.x * FurthestDistance,
                       rayStart.y + rayDirection.y * FurthestDistance);
}

bool ConstrainLine(float2 start, float2 end, float2 boxPos, float2 boxSize, float2* hitStart, float2* hitEnd)
{
    float hitClose2;
    float hitFar2;
    float2 rayDirection = sub2(end, start);
    bool hit = IntersectRayBoxDistance(start, rayDirection, boxPos, boxSize, &hitClose2, &hitFar2);
    
    bool bothOut = (hitClose2 > 0) && ((hitFar2 - length2(rayDirection)) > 0);

	if(hit && hitFar2 > 0 && hitClose2 - length2(rayDirection)<0)
	{
		if(hitClose2 > 0)
		{
            *hitStart = add2(start, mul2(normalize2(rayDirection), float2(hitClose2, hitClose2)));
		}
		else
		{
            *hitStart = start;
		}

		if((hitFar2 - length2(rayDirection)) < 0)
		{
            *hitEnd = add2(start, mul2(normalize2(rayDirection), float2(hitFar2, hitFar2)));
		}
		else
		{
            *hitEnd = end;
		}
        return true;
	}
    return false;
}


uint32 randHash(uint32 x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}
static uint32 RandomNumberIndex2;
uint32 Random()
{
    uint32 result;
#if __WASM__
    RandomNumberIndex2++;
    return randHash(RandomNumberIndex2);
#elif __clang__ // TODO: make sure we are on x86
    __builtin_ia32_rdrand32_step(&result);
    return result;
#else
    RandomNumberIndex2++;
    return randHash(RandomNumberIndex2);
#endif

    //uint32 index = RandomNumberIndex2++;
    //index = (index << 13) ^ index;
    //return (index * (index * index * 15731 + 789221) + 1376312589);
}
