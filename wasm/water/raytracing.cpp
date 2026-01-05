#pragma once
#include "quote.h"

float RayCircleIntersect(VideoBuffer* videoBuffer, float2 p, float2 d, float r)
{
    float q = dot(p, float2(-d.y, d.x));
    float o = -sqrt(r * r - q * q);

    float x = -d.y * q + d.x * o;
    float y = d.x * q + d.y * o;

    //DrawLine(videoBuffer, p, float2(0, 0));
    //DrawLine(videoBuffer, d * 100, float2(0, 0));
    //DrawCircle(videoBuffer, float2(x, y), 3);
    //DrawCircle(videoBuffer, float2(0, 0), r);

    return 0;
}


float RayBoxIntersect2D(VideoBuffer* videoBuffer, float rx, float ry, float dx, float dy, float x, float y, float width, float height)
{
    float hwidth = width / 2;
    float hheight = height / 2;
    float px = rx - x;
    float py = ry - y;
    float LeftDistance = (-hwidth - px) / dx;
    float BottomDistance = (-hheight - py) / dy;
    float RightDistance = (hwidth - px) / dx;
    float TopDistance = (hheight - py) / dy;

    float maxx = LeftDistance > RightDistance ? LeftDistance : RightDistance;
    float minx = LeftDistance < RightDistance ? LeftDistance : RightDistance;
    float maxy = TopDistance > BottomDistance ? TopDistance : BottomDistance;
    float miny = TopDistance < BottomDistance ? TopDistance : BottomDistance;
    float ClosestDistance = max(minx, miny);
    float FurthestDistance = min(maxx, maxy);

    if (minx > maxy || miny > maxx)
    {
        return -1;
    }
    return ClosestDistance;
}

float3 RayPlaneIntersectWorldSpace(float3 RayPos, float3 RayDir, float3 planeP, float3 planeN)
{
    float d = dot(planeP, -planeN);
    float t = -(d + RayPos.z * planeN.z + RayPos.y * planeN.y + RayPos.x * planeN.x) / (RayDir.z * planeN.z + RayDir.y * planeN.y + RayDir.x * planeN.x);
    return RayPos + t * RayDir;
}
//#if 1
float RayBoxIntersect(float3 RayPos, float3 RayDir, float3* Normal)
{
    float3 m = 1.0f / RayDir;
    float3 n = -m * RayPos;
    float3 k = abs(m) * 0.5f;

    float3 t1 = n - k;
    float3 t2 = n + k;

    float tN = max(t1.x, t1.y, t1.z);
    float tF = min(t2.x, t2.y, t2.z);

    if (tN > tF || tF < 0.0)// 
        return -1.0;

    *Normal = -sign(RayDir) * step(float3(t1.y, t1.z, t1.x), float3(t1.x, t1.y, t1.z)) * step(float3(t1.z, t1.x, t1.y), float3(t1.x, t1.y, t1.z));

    return tN;
}
//#else
//__m128 sign_ps(__m128 a) {
//    return _mm_blendv_ps(_mm_set1_ps(-1.0f), _mm_set1_ps(1.0f), _mm_cmpgt_ps(a, _mm_setzero_ps()));
//}
//__m128 abs_ps(__m128 a) {
//    return _mm_andnot_ps(_mm_set1_ps(-0.0f), a);
//}
//float RayBoxIntersect(float3 RayPos, float3 RayDir, float3* Normal)
//{
//    __m128 rayPos = _mm_set_ps(0, RayPos.z, RayPos.y, RayPos.x);
//    __m128 rayDir = _mm_set_ps(0, RayDir.z, RayDir.y, RayDir.x);
//
//    __m128 m = _mm_div_ps(_mm_set1_ps(1.0f), rayDir);
//    __m128 n = _mm_mul_ps(_mm_set1_ps(-1.0f), _mm_mul_ps(m, rayPos));
//    __m128 k = _mm_mul_ps(abs_ps(m), _mm_set1_ps(0.5f));
//
//    __m128 t1 = _mm_sub_ps(n, k);
//    __m128 t2 = _mm_add_ps(n, k);
//
//    __m128 tN = _mm_max_ps(_mm_max_ps(_mm_shuffle_ps(t1, t1, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(t1, t1, _MM_SHUFFLE(3, 1, 0, 2))), _mm_shuffle_ps(t1, t1, _MM_SHUFFLE(3, 2, 1, 0)));
//    __m128 tF = _mm_min_ps(_mm_min_ps(_mm_shuffle_ps(t2, t2, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(3, 1, 0, 2))), _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(3, 2, 1, 0)));
//
//    if (_mm_comigt_ss(tN, tF) || _mm_comilt_ss(tF, _mm_setzero_ps()))
//        return -1.0f;
//
//    __m128 normal = _mm_mul_ps(_mm_mul_ps(_mm_set1_ps(-1.0f), sign_ps(rayDir)), _mm_cmpge_ps(_mm_shuffle_ps(t1, t1, _MM_SHUFFLE(3, 2, 0, 1)), t1));
//    normal = _mm_mul_ps(normal, _mm_cmpge_ps(_mm_shuffle_ps(t1, t1, _MM_SHUFFLE(3, 0, 2, 1)), t1));
//
//    _mm_storeu_ps((float*)Normal, normal);
//
//    return _mm_cvtss_f32(tN);
//}
//#endif

float RayBoxIntersect_old(float3 RayPos, float3 RayDir, float3* Normal)
{
    float3 hBoxSize = float3(0.5f, 0.5f, 0.5f);

    float XpDistance = (hBoxSize.x - RayPos.x) / RayDir.x;
    float XnDistance = (-hBoxSize.x - RayPos.x) / RayDir.x;
    float YpDistance = (hBoxSize.y - RayPos.y) / RayDir.y;
    float YnDistance = (-hBoxSize.y - RayPos.y) / RayDir.y;
    float ZpDistance = (hBoxSize.z - RayPos.z) / RayDir.z;
    float ZnDistance = (-hBoxSize.z - RayPos.z) / RayDir.z;
    float maxx = XnDistance > XpDistance ? XnDistance : XpDistance;
    float minx = XnDistance < XpDistance ? XnDistance : XpDistance;
    float maxy = YnDistance > YpDistance ? YnDistance : YpDistance;
    float miny = YnDistance < YpDistance ? YnDistance : YpDistance;
    float maxz = ZnDistance > ZpDistance ? ZnDistance : ZpDistance;
    float minz = ZnDistance < ZpDistance ? ZnDistance : ZpDistance;

    if (minx > maxy || miny > maxx || minz > maxz)
    {
        return -1;
    }

    float ClosestDistance = max(minx, miny, minz);
    float FurthestDistance = min(maxx, maxy, maxz);

    if (ClosestDistance > FurthestDistance)
    {
        return -1;
    }

    *Normal = {};
    if (ClosestDistance == XpDistance)
        (*Normal).x = 1;
    else if (ClosestDistance == XnDistance)
        (*Normal).x = -1;
    else if (ClosestDistance == YpDistance)
        (*Normal).y = 1;
    else if (ClosestDistance == YnDistance)
        (*Normal).y = -1;
    else if (ClosestDistance == ZpDistance)
        (*Normal).z = 1;
    else if (ClosestDistance == ZnDistance)
        (*Normal).z = -1;

    return ClosestDistance;
}

#if MATH_TYPE == PLATFORM_MATH
__m128 _mm_cross_ps(__m128 a, __m128 b)
{
    return _mm_sub_ps(
        _mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2))),
        _mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1))));
}
float RaySphereIntersect(float3 RayPos, float3 RayDir, float r)
{
    __m128 RayPosVec = _mm_set_ps(0.0f, RayPos.z, RayPos.y, RayPos.x);
    __m128 RayDirVec = _mm_set_ps(0.0f, RayDir.z, RayDir.y, RayDir.x);
    __m128 rVec = _mm_set1_ps(r);
    __m128 rSquaredVec = _mm_mul_ps(rVec, rVec);

    float dotRayPos = _mm_cvtss_f32(_mm_dp_ps(RayPosVec, RayPosVec, 0x71));

    if (dotRayPos < (r * r))
        return -1;

    __m128 qVec = _mm_cross_ps(RayDirVec, RayPosVec);
    float dotQ = _mm_cvtss_f32(_mm_dp_ps(qVec, qVec, 0x71));

    if (dotQ > (r * r))
        return -1;

    float dist = _mm_cvtss_f32(_mm_dp_ps(_mm_mul_ps(RayPosVec, _mm_set1_ps(-1.0f)), RayDirVec, 0x71)) - sqrt((r * r) - dotQ);

    return dist;
}

#else
float RaySphereIntersect(float3 RayPos, float3 RayDir, float r)
{
    if (dot(RayPos, RayPos) < (r * r))
        return -1;

    float3 q = cross(RayDir, RayPos);
    if (dot(q, q) > (r * r))
        return -1;
    float dist = dot(-RayPos, RayDir) - sqrtFast((r * r) - dot(q, q));

    return dist;
}
#endif