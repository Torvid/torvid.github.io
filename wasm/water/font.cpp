// time to read ttf files

#include "quote.h"

struct tableRecord
{
    uint32 tag;
    //char tag[4];
    uint32 checkSum;
    uint32 offset;
    uint32 length;
};

struct TTF_HEADER
{
    // Offset subtable
    uint32 scalerType;
    uint16 numTables;
    uint16 searchRange;
    uint16 entrySelector;
    uint16 rangeShift;

    // Table directory
    tableRecord tableDirectory[100];
};

uint32 EndianSwap(uint32 num)
{
    return  ((num >> 24) & 0xff) | // move byte 3 to byte 0
            ((num << 8) & 0xff0000) | // move byte 1 to byte 2
            ((num >> 8) & 0xff00) | // move byte 2 to byte 1
            ((num << 24) & 0xff000000); // byte 0 to byte 3
}
uint16 EndianSwap(uint16 num)
{
    return (num >> 8) | (num << 8);

}
//int32 EndianSwap(int32 num)
//{
//    return  ((num >> 24) & 0xff) | // move byte 3 to byte 0
//        ((num << 8) & 0xff0000) | // move byte 1 to byte 2
//        ((num >> 8) & 0xff00) | // move byte 2 to byte 1
//        ((num << 24) & 0xff000000); // byte 0 to byte 3
//}
int16 EndianSwap(int16 num)
{
    return (int16)((num << 8) + ((uint16)num >> 8));

}
// Glyph header
// https://learn.microsoft.com/en-us/typography/opentype/spec/glyf
struct ttf_glyf
{
    // MSDN - If the number of contours is greater than or equal to zero, this is a simple glyph. 
    // If negative, this is a composite glyph — the value -1 should be used for composite glyphs.
    int16 numberOfContours;
    int16 xMin;
    int16 yMin;
    int16 xMax;
    int16 yMax;
};

typedef uint32 Fixed; // close enough for now
typedef int64 LONGDATETIME;
typedef uint16 Offset16;
//typedef uint24 Offset24;
typedef uint32 Offset32;
typedef uint32 Version16Dot16;

#define ON_CURVE_POINT                              0x01
#define X_SHORT_VECTOR                              0x02
#define Y_SHORT_VECTOR                              0x04
#define REPEAT_FLAG                                 0x08
#define X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR        0x10
#define Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR        0x20
#define OVERLAP_SIMPLE                              0x40
#define Reserved                                    0x80

#define PLATFORMID_UNICODE 0
#define PLATFORMID_MACINTOSH 1
#define PLATFORMID_ISO 2 // Deprecated, apparently
#define PLATFORMID_WINDOWS 3
#define PLATFORMID_CUSTOM 4

#define ENCODINGID_UNICODE_1_0 0 // deprecated
#define ENCODINGID_UNICODE_1_1 1 // deprecated
#define ENCODINGID_UNICODE_ISO 2 // deprecated
#define ENCODINGID_UNICODE_2_0_BMP 3
#define ENCODINGID_UNICODE_2_0_FULL 4
#define ENCODINGID_UNICODE_VARIATION_14 5
#define ENCODINGID_UNICODE_FULL_13 6


struct head
{
    uint16 majorVersion;
    uint16 minorVersion;
    Fixed fontRevision;
    uint32 checksumAdjustment;
    uint32 magicNumber;
    uint16 flags;
    uint16 unitsPerEm;
    LONGDATETIME created;
    LONGDATETIME modified;
    int16 xMin;
    int16 yMin;
    int16 xMax;
    int16 yMax;
    uint16 macStyle;
    uint16 lowestRecPPEM;
    int16 fontDirectionHint;
    int16 indexToLocFormat;
    int16 glyphDataFormat;
};

// https://learn.microsoft.com/en-us/typography/opentype/spec/maxp
struct maxp
{
    Version16Dot16 version;
    uint16 numGlyphs;
};

//https://learn.microsoft.com/en-us/typography/opentype/spec/loca
struct loca
{
    union
    {
        Offset16 offsets16[400];
        Offset32 offsets32[400];
    };
};

int ParseCoordinate(uint8** data, int coord, bool xShort, bool xIsSame)
{
    int result = 0;
    if (xShort)
    {
        result = **data;
        *data += sizeof(uint8);

        if (xIsSame)
            result = coord + result;
        else
            result = coord - result;
    }
    else
    {
        if (xIsSame)
        {
            result = coord; // no change
        }
        else
        {
            int16 wat = *(int16*)*data;
            result = coord + EndianSwap(wat);

            *data += sizeof(uint16);
        }
    }
    return result;
}

struct GlyphSegment
{
    bool isLine;
    bool goingUpwards;
    float2 pos0;
    float2 pos1;
    float2 pos2;
};
struct GlyphContour
{
    ArrayCreate(GlyphSegment, glyphSegments, 100);
};
struct Glyph
{
    float minX; // aka x bearing
    float minY; // aka descent
    float maxX; 
    float maxY; // aka bearing y

    //int sizeX;
    //int sizeY;
    //int centerX;
    //int centerY;

    float advance; // how far forward to move the cursor when this glyph his printed.

    ArrayCreate(GlyphContour, contours, 10);
};

struct Font
{
    int glyphCount;
    int ascent; // default line height, how far to go "down" after each new line.
    ArrayCreate(Glyph, glyphs, 500);
    //ArrayCreate(int, charmap, 128);
};

//Glyph* getGlyph(Font* font, char character)
//{
//
//}

const char* validchars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ$!?#&_%\'\",.;:^|`~=<>+-*/\\(){}[]@";

int GetGlyphIndex(char characterAscii, bool itallic, bool bolditallic)
{
    //if (itallic)
    //{
    //}
    //else if (bolditallic)
    //{
    //}
    //else
    //{
        if (characterAscii >= 65 && characterAscii <= 90) // Capital letters
            return (characterAscii - 65) + 14;

        if (characterAscii >= 97 && characterAscii <= 122) // Lowercase letters
            return (characterAscii - 97) + 40;

        if (characterAscii >= 48 && characterAscii <= 57) // Numbers
            return (characterAscii - 48) + 4;

        switch (characterAscii)
        {
            case '£':  return 141;
            case '$':  return 123;
            case '€':  return 0;
            case '©':  return 90;
            case '®':  return 147;
            case '¥':  return 143;
            case '!':  return 120;
            case '?':  return 110;
            case '#':  return 122;
            case '&':  return 125;
            case '_':  return 118;
            case '%':  return 124;
            case '\'': return 126;
            case '"':  return 121;
            case ',':  return 131;
            case '.':  return 133;
            case ';':  return 113;
            case ':':  return 112;
            case '^':  return 66;
            case '|':  return 136;
            case '§':  return 145;
            case '¼':  return 104;
            case '½':  return 105;
            case '¾':  return 106;
            case '`':  return 119;
            case '´':  return 96;
            case '~':  return 138;
            case '=':  return 108;
            case '<':  return 107;
            case '>':  return 109;
            case '+':  return 130;
            case '-':  return 132;
            case '*':  return 129;
            case '/':  return 134;
            case '\\': return 115;
            case '(':  return 127;
            case ')':  return 128;
            case '{':  return 135;
            case '}':  return 137;
            case '[':  return 114;
            case ']':  return 116;
            case '@':  return 111;
            case 'µ':  return 97;
            case ' ':  return -2;
            case '\n': return -1;
        }
    //}
    return 78; // empty
}

struct FontData
{
    uint8 spritefont[Megabytes(12)];
    Font font;
    //uint8 padding[Megabytes(15)];
    uint8 filedata;
};
int ReadUint16BBigEndian(uint8** data)
{
    int result = EndianSwap(*(uint16*)*data);
    *data += sizeof(uint16);
    return result;
}
int ReadInt16BBigEndian(uint8** data)
{
    int result = EndianSwap(*(int16*)*data);
    *data += sizeof(int16);
    return result;
}

// 2d analogy of cross product
float cross(float2 a, float2 b) { return a.x * b.y - a.y * b.x; }

float distanceToLineSegment(float2 p, float2 a, float2 b)
{
    float2 pa = p - a;
    float2 ba = b - a;
    float h = clamp01(dot(pa, ba) / dot(ba, ba));
    return length(pa - ba * h);
}

bool pointInBounds(float2 point, float2 boxMin, float2 boxMax)
{
    return point.x > boxMin.x && point.x < boxMax.x&& point.y > boxMin.y && point.y < boxMax.y;
}

void lineBounds(float2 pos0, float2 pos2, float2* boxMin, float2* boxMax)
{
    boxMin->x = min(pos0.x, pos2.x);
    boxMin->y = min(pos0.y, pos2.y);
    boxMax->x = max(pos0.x, pos2.x);
    boxMax->y = max(pos0.y, pos2.y);
}
bool PointInLineBounds(float2 point, float2 pos0, float2 pos1)
{
    float2 boxMin;
    float2 boxMax;
    lineBounds(pos0, pos1, &boxMin, &boxMax);
    return pointInBounds(point, boxMin, boxMax);
}

void bezierBounds(float2 pos0, float2 pos1, float2 pos2, float2* boxMin, float2* boxMax)
{
    boxMin->x = min(pos0.x, min(pos1.x, pos2.x));
    boxMin->y = min(pos0.y, min(pos1.y, pos2.y));
    boxMax->x = max(pos0.x, max(pos1.x, pos2.x));
    boxMax->y = max(pos0.y, max(pos1.y, pos2.y));
}

bool PointInBezierBounds(float2 point, float2 pos0, float2 pos1, float2 pos2)
{
    float2 boxMin;
    float2 boxMax;
    bezierBounds(pos0, pos1, pos2, &boxMin, &boxMax);
    return pointInBounds(point, boxMin, boxMax);
}

float cro(float2 a, float2 b) { return a.x * b.y - a.y * b.x; }
float dot2(float2 v) { return dot(v, v); }
// approximate solution, found here https://www.shadertoy.com/view/MlKcDD
// and here http://research.microsoft.com/en-us/um/people/hoppe/ravg.pdf
//float distanceToBezier(float2 p, float2 v0, float2 v1, float2 v2)
float distanceToBezier(float2 pos, float2 A, float2 B, float2 C)
{
    //float2 i = v0 - v2;
    //float2 j = v2 - v1;
    //float2 k = v1 - v0;
    //float2 w = j - k;
    //
    //v0 -= p;
    //v1 -= p;
    //v2 -= p;
    //
    //float x = cross(v0, v2);
    //float y = cross(v1, v0);
    //float z = cross(v2, v1);
    //
    //float2 s = 2.0 * (y * j + z * k) - x * i;
    //
    //float r = (y * z - x * x * 0.25) / dot(s, s);
    //float t = clamp01((0.5 * x + y + r * dot(s, w)) / (x + y + z));
    //
    //return length(v0 + t * (k + k + t * w));

    float2 a = B - A;
    float2 b = A - 2.0 * B + C;
    float2 c = a * 2.0;
    float2 d = A - pos;

    float kk = 1.0 / dot(b, b);
    float kx = kk * dot(a, b);
    float ky = kk * (2.0 * dot(a, a) + dot(d, b)) / 3.0;
    float kz = kk * dot(d, a);

    float res = 0.0;
    float sgn = 0.0;

    float p = ky - kx * kx;
    float q = kx * (2.0 * kx * kx - 3.0 * ky) + kz;
    float p3 = p * p * p;
    float q2 = q * q;
    float h = q2 + 4.0 * p3;

    if (h >= 0.0)
    {   // 1 root
        h = sqrt(h);
        float2 x = (float2(h, -h) - q) / 2.0;

        float2 uv = sign(x) * pow(abs(x), float2(1.0 / 3.0, 1.0 / 3.0));
        float t = clamp(uv.x + uv.y - kx, 0.0, 1.0);
        float2  q = d + (c + b * t) * t;
        res = dot2(q);
        sgn = cro(c + 2.0 * b * t, q);
    }
    else
    {   // 3 roots
        float z = sqrt(-p);
        float v = acos(q / (p * z * 2.0)) / 3.0;
        float m = cos(v);
        float n = sin(v) * 1.732050808;
        float3  t = clamp(float3(m + m, -n - m, n - m) * z - kx, 0.0f, 1.0f);
        float2  qx = d + (c + b * t.x) * t.x; float dx = dot2(qx), sx = cro(c + 2.0 * b * t.x, qx);
        float2  qy = d + (c + b * t.y) * t.y; float dy = dot2(qy), sy = cro(c + 2.0 * b * t.y, qy);
        if (dx < dy) { res = dx; sgn = sx; }
        else { res = dy; sgn = sy; }
    }

    return sqrt(res);// *sign(sgn);
}

// solve "a * t + b * (1 - t)" (lerp) for t
float intersect(float a, float b)
{
    return 1 - (-b / (a - b));
}
// find where a 2D line intersects y 0
bool lineIntersect(float2 a, float2 b, float height, float2* out)
{
    a -= float2(0, height);
    b -= float2(0, height);
    float hit = intersect(a.y, b.y);
    
    if (hit < 0.0f || hit >= 1.0f)
        return false;
    
    *out = lerp(a, b, hit) + float2(0, height);

    return true;
}

// solve lerp(lerp(a,b,t), lerp(b,c,t), t) for t.
// there are two solutions, since it's a parabola.
bool bezierIntersectQuadratic0(double a, double b, double c, float* out)
{
    double test = b * b - a * c;
    if (test <= 0)
        return false; // illegal

    double hit = 1 - ((sqrt(test) - b + c) / (c + a - 2.0 * b));
    *out = hit;

    if (hit <= 0.0 || hit > 1.0)
        return false;

    return true;
}


bool bezierIntersectQuadratic1(double a, double b, double c, float* out)
{
    double test = b * b - a * c;
    if (test <= 0)
        return false; // illegal

    double hit = 1-(-((sqrt(test) + b - c) / (c + a - 2.0 * b)));
    *out = hit;

    if (hit <= 0.0 || hit > 1.0)
        return false;

    return true;
}

float bezier(float a, float b, float c, float t)
{
    return lerp(lerp(a, b, t), lerp(b, c, t), t);
}
float2 bezier(float2 a, float2 b, float2 c, float t)
{
    return lerp(lerp(a, b, t), lerp(b, c, t), t);
}

bool bezierUpsideDown(float a, float b, float c)
{
    return (a - 2 * b + c) < 0;
}

// find the points where this curve intersects y0
void bezierIntersect(float2 a, float2 b, float2 c, float height, float2* hit0Pos, bool* hit0, float2* hit1Pos, bool* hit1)
{
    a -= float2(0, height);
    b -= float2(0, height);
    c -= float2(0, height);

    float test = (c.y + a.y - 2.0 * b.y);
    if (test == 0) // if test is 0, the intersect function will fail, so we nudge it a tiny tiny bit to prevent that.
        b.y += 0.000001;

    float dist0 = 0;
    *hit0 = bezierIntersectQuadratic0(a.y, b.y, c.y, &dist0);
    *hit0Pos = bezier(a, b, c, dist0) + float2(0, height);

    float dist1 = 0;
    *hit1 = bezierIntersectQuadratic1(a.y, b.y, c.y, &dist1);
    *hit1Pos = bezier(a, b, c, dist1) + float2(0, height);
}

int GetLineWinding(float2 start, float2 pos0, float2 pos1, float2 pos2, bool isLine, float2* hit0Pos, float2* hit1Pos, bool* hit0, bool* hit1)
{
    if (isLine)
    {
        *hit0 = lineIntersect(pos0, pos2, start.y, hit0Pos);
        if (*hit0 && start.x < (*hit0Pos).x)
        {
            return (pos0.y < pos2.y) ? 1 : -1;
            // hit
        }
    }
    else
    {
        int result = 0;
        bezierIntersect(pos0, pos1, pos2, start.y, hit0Pos, hit0, hit1Pos, hit1);
        if (*hit0 && start.x < (*hit0Pos).x)
        {
            result--;
            // hit
        }
        if (*hit1 && start.x < (*hit1Pos).x)
        {
            result++;
            // hit
        }
        return result;
    }

    return 0;
}
int GetLineWinding(float2 start, float2 pos0, float2 pos1, float2 pos2, bool isLine)
{
    bool hit0 = false;
    bool hit1 = false;
    float2 hit0Pos = float2(0, 0);
    float2 hit1Pos = float2(0, 0);
    return GetLineWinding(start, pos0, pos1, pos2, isLine, &hit0Pos, &hit1Pos, &hit0, &hit1);
}
bool ContourIsClockwise(GlyphContour* glyphContour)
{
    float sum = 0;
    for (int i = 0; i < ArrayCount(glyphContour->glyphSegments); i++)
    {
        float d0 = glyphContour->glyphSegments[i].pos0.x * 0.01f;
        float d1 = glyphContour->glyphSegments[i].pos0.y * 0.01f;
        float d2 = glyphContour->glyphSegments[i].pos2.x * 0.01f;
        float d3 = glyphContour->glyphSegments[i].pos2.y * 0.01f;
        sum += (d2 - d0) * (d3 + d1);
    }
    return sum > 0;
}

#pragma pack(push, 1)
struct FontImageGlyphMetrics
{
    uint16 minX;
    uint16 minY;
    uint16 maxX;
    uint16 maxY;
    uint16 pivotX;
    uint16 pivotY;
    uint16 advanceX;
    uint16 advanceY;
};
struct FontImageMetrics
{
    char cookie[16];
    FontImageGlyphMetrics glyphs[128];
};
#pragma pack(pop)

void DrawGlyphSimple(VideoBuffer* videoBuffer, Glyph* glyph, float2 offset, float size, float ssss, FontImageGlyphMetrics* metrics)
{
    float2 scale = float2(ssss, ssss);

    float2 boxMin = float2(glyph->minX, glyph->minY);
    float2 boxMax = float2(glyph->maxX, glyph->maxY);
    float2 boxCenter = (boxMin + boxMax) / 2.0f;
    float2 boxSize = abs(boxMin - boxMax);
    float2 sizeOffset = float2(0, boxSize.y) - boxMin;

    DrawBox(videoBuffer, offset + boxSize * 0.5 * size, boxSize * size);

    float2 smallest = float2(9999, 9999);
    float2 biggest = -float2(9999, 9999);

    float2 origin = (float2(0.0f, 0.0f) + sizeOffset) * size * scale + offset + boxSize * size * (1.0 - scale) * 0.5;
    metrics->pivotX = origin.x;
    metrics->pivotY = origin.y;
    DrawCircle(videoBuffer, origin, 5);
    float2 advance = (float2(glyph->advance, 0.0f) + sizeOffset) * size * scale + offset + boxSize * size * (1.0 - scale) * 0.5;
    metrics->advanceX = advance.x;
    metrics->advanceY = advance.y;
    DrawCircle(videoBuffer, advance, 5, float3(1, 0, 0));

    float2 center2 = offset + boxSize * 0.5 * size;
    float2 size2 = boxSize * size;

    float2 boxMin2 = center2 - size2 * 0.5;
    float2 boxMax2 = center2 + size2 * 0.5;

    metrics->maxX = boxMax2.x;//glyph->maxX;
    metrics->minX = boxMin2.x;//glyph->minX;
    metrics->maxY = boxMax2.y;//glyph->maxY;
    metrics->minY = boxMin2.y;//glyph->minY;

    for (int j = 0; j < ArrayCount(glyph->contours); j++)
    {
        float3 actualColor = float3(frac((float)randHash(j) / 1000000.0f), frac((float)randHash(j + 20) / 1000000.0f), frac((float)randHash(j + 40) / 1000000.0f));
        actualColor = float3(1, 1, 1);
        GlyphContour* contour = &glyph->contours[j];

        for (int k = 0; k < ArrayCount(contour->glyphSegments); k++)
        {
            GlyphSegment* segment = &contour->glyphSegments[k];

            float2 pos0 = (segment->pos0 + sizeOffset) * size * scale + offset + boxSize * size * (1.0 - scale) * 0.5;
            float2 pos1 = (segment->pos1 + sizeOffset) * size * scale + offset + boxSize * size * (1.0 - scale) * 0.5;
            float2 pos2 = (segment->pos2 + sizeOffset) * size * scale + offset + boxSize * size * (1.0 - scale) * 0.5;

            if (segment->isLine)
            {
                DrawLine(videoBuffer, pos0, pos2, actualColor);
                smallest = min(smallest, pos0);
                smallest = min(smallest, pos2);
                biggest = max(biggest, pos0);
                biggest = max(biggest, pos2);
            }
            else
            {
                DrawLineBezier(videoBuffer, pos0, pos1, pos2, actualColor);
                smallest = min(smallest, pos0);
                smallest = min(smallest, pos1);
                smallest = min(smallest, pos2);
                biggest = max(biggest, pos0);
                biggest = max(biggest, pos1);
                biggest = max(biggest, pos2);
            }
        }
    }
    float2 boxCenterFitted = (smallest + biggest) / 2.0f;
    float2 boxSizeFitted = abs(smallest - biggest);
    DrawBox(videoBuffer, boxCenterFitted, boxSizeFitted);
    
}

void DrawGlyphSDF(VideoBuffer* videoBuffer, Glyph* glyph, float2 offset, float size, float scale, FontImageGlyphMetrics* metrics)
{
    float2 boxMin = float2(glyph->minX, glyph->minY);
    float2 boxMax = float2(glyph->maxX, glyph->maxY);
    float2 boxCenter = (boxMin + boxMax) / 2.0f;
    float2 boxSize = abs(boxMin - boxMax);
    float2 sizeOffset = float2(0, boxSize.y) - boxMin;

    float2 center2 = offset + boxSize * 0.5 * size;
    float2 size2 = boxSize * size;

    float2 boxMin2 = center2 - size2 * 0.5;
    float2 boxMax2 = center2 + size2 * 0.5;
    int expand = 0;
    for (int x = (boxMin2.x - expand); x < (boxMax2.x + expand); x++)
    {
        for (int y = (boxMin2.y - expand); y < (boxMax2.y + expand); y++)
        {
            float dist = 999999;
            int winding = 0;
            for (int j = 0; j < ArrayCount(glyph->contours); j++)
            {
                float3 actualColor = float3(1, 1, 1);
                GlyphContour* contour = &glyph->contours[j];
                for (int k = 0; k < ArrayCount(contour->glyphSegments); k++)
                {
                    GlyphSegment* segment = &contour->glyphSegments[k];
                    float2 pos0 = (segment->pos0 + sizeOffset) * size * scale + offset + boxSize * size * (1.0 - scale) * 0.5;
                    float2 pos1 = (segment->pos1 + sizeOffset) * size * scale + offset + boxSize * size * (1.0 - scale) * 0.5;
                    float2 pos2 = (segment->pos2 + sizeOffset) * size * scale + offset + boxSize * size * (1.0 - scale) * 0.5;

                    if (segment->isLine)
                        dist = min(dist, distanceToLineSegment(float2(x, y), pos0, pos2));
                    else
                        dist = min(dist, distanceToBezier(float2(x, y), pos0, pos1, pos2));

                    winding += GetLineWinding(float2(x, y) + 0.001f, pos0, pos1, pos2, segment->isLine);

                }
            }
            float3 color = float3(dist, dist, dist) / 20.0f;
            if (winding != 0)
                SetPixel(videoBuffer, x, y, (color)+0.5);
            else
                SetPixel(videoBuffer, x, y, (-color) + 0.5);
        }
    }


    DrawBox(videoBuffer, center2, size2, float3(0, 0, 0));
    DrawBox(videoBuffer, center2, size2-2, float3(0, 0, 0));

    return;




    DrawBox(videoBuffer, offset + boxSize * 0.5 * size, boxSize * size);

    float2 smallest = float2(9999, 9999);
    float2 biggest = -float2(9999, 9999);

    float2 origin = (float2(0.0f, 0.0f) + sizeOffset) * size * scale + offset + boxSize * size * (1.0 - scale) * 0.5;
    metrics->pivotX = origin.x;
    metrics->pivotY = origin.y;
    DrawCircle(videoBuffer, origin, 5);
    float2 advance = (float2(glyph->advance, 0.0f) + sizeOffset) * size * scale + offset + boxSize * size * (1.0 - scale) * 0.5;
    metrics->advanceX = advance.x;
    metrics->advanceY = advance.y;
    DrawCircle(videoBuffer, advance, 5, float3(1, 0, 0));

    metrics->maxX = glyph->maxX;
    metrics->minX = glyph->minX;
    metrics->maxY = glyph->maxY;
    metrics->minY = glyph->minY;

    for (int j = 0; j < ArrayCount(glyph->contours); j++)
    {
        float3 actualColor = float3(frac((float)randHash(j) / 1000000.0f), frac((float)randHash(j + 20) / 1000000.0f), frac((float)randHash(j + 40) / 1000000.0f));
        actualColor = float3(1, 1, 1);
        GlyphContour* contour = &glyph->contours[j];

        for (int k = 0; k < ArrayCount(contour->glyphSegments); k++)
        {
            GlyphSegment* segment = &contour->glyphSegments[k];

            float2 pos0 = (segment->pos0 + sizeOffset) * size * scale + offset + boxSize * size * (1.0 - scale) * 0.5;
            float2 pos1 = (segment->pos1 + sizeOffset) * size * scale + offset + boxSize * size * (1.0 - scale) * 0.5;
            float2 pos2 = (segment->pos2 + sizeOffset) * size * scale + offset + boxSize * size * (1.0 - scale) * 0.5;

            if (segment->isLine)
            {
                DrawLine(videoBuffer, pos0, pos2, actualColor);
                smallest = min(smallest, pos0);
                smallest = min(smallest, pos2);
                biggest = max(biggest, pos0);
                biggest = max(biggest, pos2);
            }
            else
            {
                DrawLineBezier(videoBuffer, pos0, pos1, pos2, actualColor);
                smallest = min(smallest, pos0);
                smallest = min(smallest, pos1);
                smallest = min(smallest, pos2);
                biggest = max(biggest, pos0);
                biggest = max(biggest, pos1);
                biggest = max(biggest, pos2);
            }
        }
    }



    float2 boxCenterFitted = (smallest + biggest) / 2.0f;
    float2 boxSizeFitted = abs(smallest - biggest);

    DrawBox(videoBuffer, boxCenterFitted, boxSizeFitted);
}

void FontReader(GameMemory* memory, EngineState* engineState, GameInput* input, VideoBuffer* videoBuffer)
{

#if 0 // debugging intersection functions
    DrawClear(videoBuffer, float3(0, 0, 0));
    float2 pos0 = float2(100, 50);
    float2 pos1 = float2(200, 300);
    float2 pos2 = float2(500, 100);
    float2 test = float2(input->MouseX, input->MouseY);

    //float dist = distanceToLineSegment(test, pos0, pos1);
    //DrawCircle(videoBuffer, test, dist);

    float dist2 = distanceToBezier(test, pos0, pos1, pos2);
    DrawCircle(videoBuffer, test, dist2);
    //if (PointInBezierBounds(test, pos0, pos1, pos2))
    //{
    //    DrawCircle(videoBuffer, float2(0, 0), 100);
    //}
    if (PointInLineBounds(test, pos0, pos1))
    {
        DrawCircle(videoBuffer, float2(0, 0), 100);
    }
    //int winding = 0;
    //winding += GetLineWinding(start, pos0, pos1, pos1, true);
    //winding += GetLineWinding(start, pos0, pos1, pos2, false);

    //float distanceToBezier(test, float2 v0, float2 v1, float2 v2)
    //float distanceToLineSegment(test, float2 a, float2 b)
    DrawLineBezier(videoBuffer, pos0, pos1, pos2, float3(1, 1, 1));

    DrawCircle(videoBuffer, pos0, 5.0f, float3(1, 0, 0));
    DrawCircle(videoBuffer, pos1, 5.0f, float3(0, 1, 0));
    DrawCircle(videoBuffer, pos2, 5.0f, float3(0, 0, 1));

    //int winding = 0;
    //GetLineHit(float2 start, pos0, pos1, pos2, &winding)
    float h = 150;

    bool hit0;
    bool hit1;
    float2 hit0Pos;
    float2 hit1Pos;
    DrawLine(videoBuffer, float2(0, h), float2(2000, h));
    bezierIntersect(pos0, pos1, pos2, h, &hit0Pos, &hit0, &hit1Pos, &hit1);
    if (hit0)
        DrawCircle(videoBuffer, hit0Pos, 5.0f, float3(1.0, 0.5, 0.5));
    if (hit1)
        DrawCircle(videoBuffer, hit1Pos, 5.0f, float3(0.5, 1.0, 0.5));


    DrawLine(videoBuffer, pos0, pos1);
    if (lineIntersect(pos0, pos1, h, &hit0Pos))
    {
        DrawCircle(videoBuffer, hit0Pos, 5.0f, float3(0.5, 0.5, 1.0));
    }

    return;
#endif

    FontData* fontData = (FontData*)(((uint8*)(engineState + 2)));
    Font* font = &fontData->font;

    if (input->SpaceDown)
        memory->initialized = false;

    if (!memory->initialized)
    {
        engineState->profilingData.zoom = 1;
        engineState->platformReadImageIntoMemory = memory->platformReadImageIntoMemory;
        engineState->platformReadSoundIntoMemory = memory->platformReadSoundIntoMemory;
        engineState->platformPrint = memory->platformPrint;
        engineState->platformSetForcefeedbackEffect = memory->platformSetForcefeedbackEffect;
        InitializeArena(&engineState->staticAssetsArena, Megabytes(12), (uint8*)(engineState + 1));
        engineState->fontSpritesheet = LoadImage(engineState, &engineState->staticAssetsArena, "font.tga", 192, 52);

        //Clear(fontData, Megabytes(2));
        memory->initialized = true;
    }
    Clear(fontData, sizeof(FontData));

    const char* ttfFile = "Atkinson-Hyperlegible-Bold-102.ttf";
    const char* outputFile = "FontDataBold.font";
    memory->platformReadFile(ttfFile, &fontData->filedata);
    bool italic = false;
    bool bolditallic = false;

    ArrayClear(font->glyphs);

    DrawClear(videoBuffer, float3(0, 0, 0));

    TTF_HEADER* header = (TTF_HEADER*)(&fontData->filedata);
    header->numTables = EndianSwap(header->numTables);
    TTF_HEADER a;

    head* fontHead = 0;
    maxp* maximumProfile = 0;
    loca* indexToLocation = 0;
    uint8* glyphBase = 0;

    // loop over tables and fix up the endianness
    for (int i = 0; i < header->numTables; i++)
    {
        tableRecord* directory = &header->tableDirectory[i];
        directory->tag = EndianSwap(directory->tag);
        directory->length = EndianSwap(directory->length);
        directory->offset = EndianSwap(directory->offset);
        directory->checkSum = EndianSwap(directory->checkSum);
    }

    // loop over tables and get pointers to them.
    for (int i = 0; i < header->numTables; i++)
    {
        tableRecord* directory = &header->tableDirectory[i];

        if (directory->tag == 'head')
            fontHead = (head*)(&fontData->filedata + directory->offset);
        if (directory->tag == 'maxp')
            maximumProfile = (maxp*)(&fontData->filedata + directory->offset);
        if (directory->tag == 'loca')
            indexToLocation = (loca*)(&fontData->filedata + directory->offset);
        if (directory->tag == 'glyf')
            glyphBase = (&fontData->filedata + directory->offset);
    }

    // loop over tables and fix up endianness
    for (int i = 0; i < header->numTables; i++)
    {
        tableRecord* directory = &header->tableDirectory[i];
        if (directory->tag == 'head')
        {
            //fontHead->majorVersion = EndianSwap(fontHead->majorVersion);
            //fontHead->minorVersion = EndianSwap(fontHead->minorVersion);
            //fontHead->fontRevision = EndianSwap(fontHead->fontRevision);
            //fontHead->checksumAdjustment = EndianSwap(fontHead->checksumAdjustment);
            //fontHead->magicNumber = EndianSwap(fontHead->magicNumber);
            //fontHead->flags = EndianSwap(fontHead->flags);
            //fontHead->unitsPerEm = EndianSwap(fontHead->unitsPerEm);
            //fontHead->created                = EndianSwap(fontHead->created);
            //fontHead->modified               = EndianSwap(fontHead->modified);
            //fontHead->xMin = EndianSwap(fontHead->xMin);
            //fontHead->yMin = EndianSwap(fontHead->yMin);
            //fontHead->xMax = EndianSwap(fontHead->xMax);
            //fontHead->yMax = EndianSwap(fontHead->yMax);
            //fontHead->macStyle = EndianSwap(fontHead->macStyle);
            //fontHead->lowestRecPPEM = EndianSwap(fontHead->lowestRecPPEM);
            //fontHead->fontDirectionHint = EndianSwap(fontHead->fontDirectionHint);
            fontHead->indexToLocFormat = EndianSwap(fontHead->indexToLocFormat);
            //fontHead->glyphDataFormat = EndianSwap(fontHead->glyphDataFormat);

        }
        if (directory->tag == 'maxp')
        {
            maximumProfile->numGlyphs = EndianSwap(maximumProfile->numGlyphs);
            font->glyphCount = maximumProfile->numGlyphs;
        }
    }

    // fix up more endianness
    for (int i = 0; i < header->numTables; i++)
    {
        tableRecord* directory = &header->tableDirectory[i];
        if (directory->tag == 'loca')
        {
            if (fontHead->indexToLocFormat) // long offsets (Offset32)
            {
                Assert(false); // TODO
            }
            else // short offsets (Offset16)
            {
                for (int j = 0; j < maximumProfile->numGlyphs; j++)
                {
                    indexToLocation->offsets16[j] = EndianSwap(indexToLocation->offsets16[j]);
                }
            }
        }
    }

    // read glyphs
    for (int i = 0; i < header->numTables; i++)
    {
        tableRecord* directory = &header->tableDirectory[i];
        if (directory->tag == 'glyf')
        {
            for (int glyphIndex = 0; glyphIndex < maximumProfile->numGlyphs; glyphIndex++)
            {
                ArrayCreate(uint8, instructions, 1000);

                Glyph* glyph = ArrayAddIndex(font->glyphs);

                int a = 32;
                if (glyphIndex == 151)
                {
                    a = 2;
                }
                ArrayCreate(int, xCoords, 500);
                ArrayCreate(int, yCoords, 500);
                ArrayCreate(uint8, flags, 500);
                ArrayCreate(uint16, endPointIndexes, 500);

                uint8* glyphPtr = (glyphBase)+indexToLocation->offsets16[glyphIndex] * 2;
                uint8* data = glyphPtr;
                int numberOfContours = EndianSwap(*(int16*)data);
                data += sizeof(int16);

                glyph->minX = ReadInt16BBigEndian(&data);
                glyph->minY = -ReadInt16BBigEndian(&data);
                glyph->maxX = ReadInt16BBigEndian(&data);
                glyph->maxY = -ReadInt16BBigEndian(&data);
                //glyph->yMax = (glyph->yMin - glyph->yMax);

                if (numberOfContours <= 0)
                {
                    break;
                }
                // get countour end-points
                for (int j = 0; j < numberOfContours; j++)
                {
                    ArrayAdd(endPointIndexes, EndianSwap(*(uint16*)data));
                    data += sizeof(uint16);
                }

                // skip instructions
                uint16 instructionLength = EndianSwap(*(uint16*)data);
                data += sizeof(uint16);
                data += instructionLength;

                uint16 numberOfCoordinates = endPointIndexes[numberOfContours - 1] + 1;

                for (int j = 0; j < numberOfCoordinates; j++)
                {
                    uint8 flag = *data;
                    data += sizeof(uint8);
                    ArrayAdd(flags, flag);

                    if (flag & REPEAT_FLAG)
                    {
                        // skip an extra byte because it contains the repeat count.
                        uint8 repeatCount = *data;
                        Assert(repeatCount > 0);
                        j += repeatCount;
                        data += sizeof(uint8);
                        while (repeatCount--)
                        {
                            ArrayAdd(flags, flag);
                        }
                    }
                }

                // Check that flags were grabbed correctly
                if (ArrayCount(flags) != numberOfCoordinates)
                    break;

                int coord = 0;
                for (int j = 0; j < numberOfCoordinates; j++)
                {
                    bool xShort = flags[j] & X_SHORT_VECTOR;
                    bool xIsSame = flags[j] & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR;

                    coord = ParseCoordinate(&data, coord, xShort, xIsSame);
                    ArrayAdd(xCoords, coord);
                }

                coord = 0;
                for (int j = 0; j < numberOfCoordinates; j++)
                {
                    bool yShort = flags[j] & Y_SHORT_VECTOR;
                    bool yIsSame = flags[j] & Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR;

                    coord = ParseCoordinate(&data, coord, yShort, yIsSame);
                    ArrayAdd(yCoords, coord);
                }

                // expand/unpack glyph curves in memory even further :)
                float2 pos = float2(0, 0);
                float2 last = pos;
                int lastI = 0;
                for (int j = 0; j < ArrayCount(endPointIndexes); j++)
                {
                    ArrayAddIndex(glyph->contours);
                    float2 start = pos;
                    for (int k = lastI; k < endPointIndexes[j] + 1; k++)
                    {
                        bool first = k == lastI;
                        last = pos;

                        bool onCurve0 = flags[k - 1] & ON_CURVE_POINT;
                        bool onCurve1 = flags[k] & ON_CURVE_POINT;
                        bool onCurve2 = flags[k + 1] & ON_CURVE_POINT;

                        float2 pos0 = float2(xCoords[k - 1], -yCoords[k - 1]);
                        float2 pos1 = float2(xCoords[k], -yCoords[k]);
                        float2 pos2 = float2(xCoords[k + 1], -yCoords[k + 1]);
                        pos0 = onCurve0 ? pos0 : lerp(pos0, pos1, 0.5f);
                        pos2 = onCurve2 ? pos2 : lerp(pos1, pos2, 0.5f);

                        pos = float2(xCoords[k], -yCoords[k]);

                        if (first)
                            start = pos;
                        if (first)
                            continue;

                        GlyphSegment segment;
                        segment.isLine = false;
                        segment.pos0 = pos0;
                        segment.pos1 = pos1;
                        segment.pos2 = pos2;
                        if (onCurve0 && onCurve1)
                        {
                            segment.isLine = true;
                            segment.pos0 = pos0;
                            segment.pos1 = float2(0, 0);
                            segment.pos2 = pos1;
                            ArrayAdd(glyph->contours[j].glyphSegments, segment);
                        }
                        else if (!onCurve1)
                        {
                            ArrayAdd(glyph->contours[j].glyphSegments, segment);
                        }
                    }
                    GlyphSegment segment;
                    segment.isLine = true;
                    segment.pos0 = pos;
                    segment.pos1 = float2(0, 0);
                    segment.pos2 = start;
                    ArrayAdd(glyph->contours[j].glyphSegments, segment);
                    lastI = endPointIndexes[j] + 1;
                }
            }
        }
    }

    for (int i = 0; i < header->numTables; i++)
    {
        tableRecord* directory = &header->tableDirectory[i];
        uint8* table = (uint8*)(&fontData->filedata + directory->offset);
        uint8* data = (uint8*)(&fontData->filedata + directory->offset);

        // read advance widths, aka how far forward the cursor should be moved when this glyph is printed.
        if (directory->tag == 'hmtx')
        {
            for (int j = 0; j < ArrayCount(font->glyphs); j++)
            {
                font->glyphs[j].advance = EndianSwap(*(uint16*)(data + j * 4));
                //font->glyphs[j].advance /= abs(font->glyphs[j].xMin - font->glyphs[j].xMax);
            }
        }
        // read character mapping, IE what character corresponds to what glyph
        if (directory->tag == 'cmap')
        {
            /*
            uint16 version = EndianSwap(*(uint16*)data);
            data += sizeof(uint16);
            uint16 numTables = EndianSwap(*(uint16*)data);
            data += sizeof(uint16);
            for (int j = 0; j < numTables; j++)
            {
                uint16 platformID = ReadUint16BBigEndian(&data);
                uint16 encodingID = ReadUint16BBigEndian(&data);
                Offset32 subtableOffset = ReadUint16BBigEndian(&data);

                uint8* subtable = table + subtableOffset;
                // only supported configuration right now:hahaha
                if (platformID == PLATFORMID_UNICODE && encodingID == ENCODINGID_UNICODE_2_0_BMP)
                {
                    int format = ReadUint16BBigEndian(&subtable);
                    int len = ReadUint16BBigEndian(&subtable);
                    int language = ReadUint16BBigEndian(&subtable);
                    // Format 4: Segment mapping to delta values
                    if (format == 4)
                    {
                        int segCount = ReadUint16BBigEndian(&subtable) / 2;
                        int searchRange = ReadUint16BBigEndian(&subtable);
                        int entrySelector = ReadUint16BBigEndian(&subtable);
                        int rangeShift = ReadUint16BBigEndian(&subtable);

                    }
                }
            }
        */
        // Might do proper cmap reading later, for now it's hard-coded based on the one font I will use.
            //for (int j = 0; j < ArrayCount(font->glyphs); j++)
            //{
            //}

        }
    }

    int DrawFinalSpritesheet = 1;
    int DrawAllGlyphs = 2;
    int DrawSingleGlyph = 3;
    int DrawMultiGlyph = 4;
    int state = DrawSingleGlyph;

    if (state == DrawFinalSpritesheet)
    {
        float totalSizeX = 0;
        float tallestSizeY = 0;
        for (int i = 0; i < StringLength(validchars); i++)
        {
            int glyphIndex = GetGlyphIndex(validchars[i], italic, bolditallic);
            Glyph* glyph = &font->glyphs[glyphIndex];

            float2 boxSize = abs(float2(glyph->minX, glyph->minY) - float2(glyph->maxX, glyph->maxY));
            totalSizeX += boxSize.x;
            tallestSizeY = max(tallestSizeY, boxSize.y);
        }
        float squareSize = sqrt(totalSizeX * tallestSizeY);
        float outputImageResolution = 1000;
        float scaleMultiplier = outputImageResolution / squareSize * 0.945;


        FontImageMetrics metrics = {};
        const char* cookie = "h a v e n - font";

        Copy((uint8*)cookie, (uint8*)&metrics, 16);

        //FontImageGlyphMetrics glyphs[105] = {};
        float currentPosX = 0;
        float currentPosY = 0;
        int len = StringLength(validchars);
        for (int i = 0; i < len; i++)
        {
            int glyphIndex = GetGlyphIndex(validchars[i], italic, bolditallic);
            //if (i == 8)
            //    break;
            Glyph* glyph = &font->glyphs[glyphIndex];
            Glyph* glyphNext = &font->glyphs[glyphIndex + 1];

            float2 boxMin = float2(glyph->minX, glyph->minY);
            float2 boxMax = float2(glyph->maxX, glyph->maxY);
            float2 boxSize = abs(boxMin - boxMax);

            float2 boxMin2 = float2(glyphNext->minX, glyphNext->minY);
            float2 boxMax2 = float2(glyphNext->maxX, glyphNext->maxY);
            float2 boxSize2 = abs(boxMin2 - boxMax2);

            //if (i > 10 && i < 30)
            {
                //DrawGlyphSDF(videoBuffer, glyph, float2(currentPosX, currentPosY), scaleMultiplier, 0.75f, &metrics.glyphs[i]);
                DrawGlyphSimple(videoBuffer, glyph, float2(currentPosX, currentPosY), scaleMultiplier, 0.75f, &metrics.glyphs[i]);
            }

            currentPosX += boxSize.x * scaleMultiplier; // advance

            if (currentPosX + boxSize2.x * scaleMultiplier > outputImageResolution)
            {
                currentPosX = 0;
                currentPosY += tallestSizeY * scaleMultiplier;
            }
        }

        memory->platformWriteFile(outputFile, &metrics, sizeof(metrics));
        return;
    }
    else if (state == DrawAllGlyphs)
    {
        int glyps[1] = { 3 };
        int count = ArrayCount(font->glyphs);

        float size = 0.05f;

        for (int i = 0; i < ArrayCount(font->glyphs); i++)
        {
            int glyphIndex = i;

            Glyph* glyph = &font->glyphs[glyphIndex];
            float ss = 1200;

            int x = i % 16;
            int y = i / 16;

            float2 offset = float2(x * ss + ss * 1.0f, y * ss + ss * 1.0f) * size;

            float2 boxMin = float2(glyph->minX, glyph->minY) * size + offset;
            float2 boxMax = float2(glyph->maxX, glyph->maxY) * size + offset;
            int expand = 10;
            for (int x = (boxMin.x - expand); x < (boxMax.x + expand); x++)
            {
                for (int y = (boxMax.y - expand); y < (boxMin.y + expand); y++)
                {
                    float dist = 999999;
                    int winding = 0;
                    for (int j = 0; j < ArrayCount(glyph->contours); j++)
                    {
                        float3 actualColor = float3(1, 1, 1);
                        GlyphContour* contour = &glyph->contours[j];
                        for (int k = 0; k < ArrayCount(contour->glyphSegments); k++)
                        {
                            GlyphSegment* segment = &contour->glyphSegments[k];
                            float2 pos0 = segment->pos0 * size + offset;
                            float2 pos1 = segment->pos1 * size + offset;
                            float2 pos2 = segment->pos2 * size + offset;

                            if (segment->isLine)
                                dist = min(dist, distanceToLineSegment(float2(x, y), pos0, pos2));
                            else
                                dist = min(dist, distanceToBezier(float2(x, y), pos0, pos1, pos2));

                            winding += GetLineWinding(float2(x, y) + 0.001f, pos0, pos1, pos2, segment->isLine);

                        }
                    }
                    float3 color = float3(dist, dist, dist) / 20.0f;
                    if (winding != 0)
                        SetPixel(videoBuffer, x, y, (color)+0.5);
                    else
                        SetPixel(videoBuffer, x, y, (-color) + 0.5);
                }
            }
        }

    }
    else if (state == DrawSingleGlyph)
    {
        bool drawHelpers = true;
        bool drawSDF = false;
        Glyph* glyph = &font->glyphs[111];

        float size = 2.0f;
        float2 offset = float2(500, 800);
        float2 boxMin = float2(glyph->minX, glyph->minY) * size + offset;
        float2 boxMax = float2(glyph->maxX, glyph->maxY) * size + offset;
        boxMin.x -= 10.0f;
        boxMax.x += 10.0f;
        boxMin.y += 10.0f;
        boxMax.y -= 10.0f;

        float2 boxCenter = (boxMin + boxMax) / 2.0f;
        float2 boxSize = abs(boxMin - boxMax);
        if (drawSDF)
        {

#define TEST_SINGLE_LINE 0

#if !TEST_SINGLE_LINE
        for (int x = boxMin.x; x < boxMax.x; x++)
        {
            for (int y = boxMax.y; y < boxMin.y; y++)
            {
#else
        // figure out if we are inside the shape or outside it.
        int x = input->MouseX;
        int y = input->MouseY;
        y = 522;
        char text[100] = {};
        Append(text, "height: ");
        Append(text, input->MouseY);
        DrawFont(engineState->fontSpritesheet, videoBuffer, 100, 200, text);
        DrawCircle(videoBuffer, float2(x, y), 5);
#endif
        float dist = 999999;
        int winding = 0;
        for (int j = 0; j < ArrayCount(glyph->contours); j++)
        {
            GlyphContour* contour = &glyph->contours[j];

            for (int k = 0; k < ArrayCount(contour->glyphSegments); k++)
            {
                GlyphSegment* segment = &contour->glyphSegments[k];
                float2 pos0 = segment->pos0 * size + offset;
                float2 pos1 = segment->pos1 * size + offset;
                float2 pos2 = segment->pos2 * size + offset;

                if (segment->isLine)
                    dist = min(dist, distanceToLineSegment(float2(x, y), pos0, pos2));
                else
                    dist = min(dist, distanceToBezier(float2(x, y), pos0, pos1, pos2));
                if (k == 17)
                {
                    k = 17;
                }
                bool hit0 = false;
                bool hit1 = false;
                float2 hit0Pos = float2(0, 0);
                float2 hit1Pos = float2(0, 0);
                winding += GetLineWinding(float2(x, y) + 0.001f, // fudge
                    pos0,
                    pos1,
                    pos2, segment->isLine, &hit0Pos, &hit1Pos, &hit0, &hit1);
#if TEST_SINGLE_LINE
                if (k == 17)
                {
                    char text[100] = {};
                    Append(text, k);
                    DrawFont(engineState->fontSpritesheet, videoBuffer,
                        segment->pos2.x * size + offset.x,
                        segment->pos2.y * size + offset.y, text);
                    DrawFont(engineState->fontSpritesheet, videoBuffer,
                        segment->pos0.x * size + offset.x,
                        segment->pos0.y * size + offset.y, text);
                }
                if (hit0)
                    DrawCircle(videoBuffer, hit0Pos, 3.0f, float3(1, 0, 0));
                if (hit1)
                    DrawCircle(videoBuffer, hit1Pos, 3.0f, float3(0, 1, 0));
#endif
            }
#if TEST_SINGLE_LINE
            char text[100] = {};
            Append(text, "winding: ");
            Append(text, winding);
            DrawFont(engineState->fontSpritesheet, videoBuffer, 100, j * 20, text);
#endif
#if !TEST_SINGLE_LINE

#if 1

            float3 color = float3(dist, dist, dist) / 25.0f;
            SetPixel(videoBuffer, x, y, 1-(color));
            //if (winding != 0)
            //    SetPixel(videoBuffer, x, y, (color)+0.5);
            //else
            //    SetPixel(videoBuffer, x, y, (-color) + 0.5);
            //float3 color = float3(dist, dist, dist) / 20.0f;
            //if (winding != 0)
            //    SetPixel(videoBuffer, x, y, float3(1,1,1));
            //else
            //    SetPixel(videoBuffer, x, y, float3(0,0,0));
#else
            float3 color = float3(0, 0, 0);
            if (ContourIsClockwise(contour))
                color = float3(1, 1, 1);
            if (winding != 0)
                SetPixel(videoBuffer, x, y, color);
#endif
        }
            }
#endif
        }
        }
        if (drawHelpers)
        {
            bool drawIntersection = false;
            bool drawRealPoints = true;
            bool drawPhantomPoints = true;
            bool drawArrows = false;
            int drawState = 0;
            int intersectSize = 4;
            float2 testPos = float2(input->MouseX, input->MouseY);
            
            if (drawIntersection)
            {
                DrawLine(videoBuffer, testPos, testPos + float2(2000, 0));
                DrawLine(videoBuffer, testPos - float2(5, 5), testPos + float2(5, 6));
                DrawLine(videoBuffer, testPos - float2(-5, 5), testPos + float2(-5, 6));
            }
            //DrawBox(videoBuffer, boxCenter, boxSize, float3(0.5f, 0.5f, 0.5f));
            for (int j = 0; j < ArrayCount(glyph->contours); j++)
            {
                float3 actualColor = float3(frac((float)randHash(j) / 1000000.0f), frac((float)randHash(j + 20) / 1000000.0f), frac((float)randHash(j + 40) / 1000000.0f));
                //actualColor = float3(1, 1, 1);
                GlyphContour* contour = &glyph->contours[j];
                for (int k = 0; k < ArrayCount(contour->glyphSegments); k++)
                {
                    GlyphSegment* segment = &contour->glyphSegments[k];
                    float2 pos0 = segment->pos0 * size + offset;
                    float2 pos1 = segment->pos1 * size + offset;
                    float2 pos2 = segment->pos2 * size + offset;

                    if ((k % 2) == 0 && drawArrows && (drawState == 0 || (drawState == 1 && j == 0) || (drawState == 2 && j == 1)))
                    {
                        float2 delta = normalize(pos0 - pos2);
                        float2 middle = (pos0 + pos2) * 0.5;
                        float2 back = middle + delta * 16;
                        float2 front = middle - delta * 16;
                        float2 right = pos0 - pos2;
                        right = normalize(float2(-right.y, right.x)) * 6;
                        DrawLine(videoBuffer, middle, (back + right), actualColor);
                        DrawLine(videoBuffer, middle, (back - right), actualColor);
                    }
                }
                for (int k = 0; k < ArrayCount(contour->glyphSegments); k++)
                {
                    GlyphSegment* segment = &contour->glyphSegments[k];
                    float2 pos0 = segment->pos0 * size + offset;
                    float2 pos1 = segment->pos1 * size + offset;
                    float2 pos2 = segment->pos2 * size + offset;

                    if (segment->isLine)
                    {
                        if (drawIntersection && (drawState == 0 || (drawState == 1 && j == 0) || (drawState == 2 && j == 1)))
                        {
                            float2 hit0Pos = float2(0, 0);
                            lineIntersect(pos0, pos2, testPos.y, &hit0Pos);

                            if (hit0Pos.x > testPos.x)
                            {
                                DrawFont(engineState->fontSpritesheet, videoBuffer, hit0Pos.x + 4, hit0Pos.y - 18, (pos0.y < pos2.y) ? "+1" : "-1");
                                DrawBox(videoBuffer, hit0Pos, float2(intersectSize, intersectSize), float3(1, 1, 1));
                            }
                        }

                        DrawLine(videoBuffer, pos0, pos2, actualColor);
                        if (drawRealPoints)
                        {
                            DrawBox(videoBuffer, pos0, float2(3, 3), clamp01(actualColor * 2));
                            DrawBox(videoBuffer, pos2, float2(3, 3), clamp01(actualColor * 2));
                        }
                    }
                    else
                    {
                        if (j == 0 && (k == 30 || k == 29 || k == 31))
                        {
                            if (k == 30)
                            {
                                DrawFont(engineState->fontSpritesheet, videoBuffer, pos0.x + 2, pos0.y + 3, "End");
                                DrawFont(engineState->fontSpritesheet, videoBuffer, pos1.x + 2, pos1.y + 3, "Tan");
                                DrawFont(engineState->fontSpritesheet, videoBuffer, pos2.x + 2, pos2.y + 3, "End");
                            }
                            if (k == 31)
                            {
                                DrawFont(engineState->fontSpritesheet, videoBuffer, pos1.x + 2, pos1.y + 3, "Tan");
                            }
                            if (k == 29)
                            {
                                //DrawFont(engineState->fontSpritesheet, videoBuffer, pos0.x + 2, pos0.y + 3, "End");
                                DrawFont(engineState->fontSpritesheet, videoBuffer, pos1.x + 2, pos1.y + 3, "Tan");
                                //DrawFont(engineState->fontSpritesheet, videoBuffer, pos2.x + 2, pos2.y + 3, "End");
                            }
                            //if (k == 27)
                            //{
                            //    DrawFont(engineState->fontSpritesheet, videoBuffer, pos0.x + 2, pos0.y + 3, "End");
                            //    DrawFont(engineState->fontSpritesheet, videoBuffer, pos1.x + 2, pos1.y + 3, "Tan");
                            //    DrawFont(engineState->fontSpritesheet, videoBuffer, pos2.x + 2, pos2.y + 3, "End");
                            //}
                            //
                        }
                        if (drawIntersection && (drawState == 0 || (drawState == 1 && j == 0) || (drawState == 2 && j == 1)))
                        {
                            float2 hit0Pos = float2(0, 0);
                            float2 hit1Pos = float2(0, 0);
                            bool hit0;
                            bool hit1;
                            bezierIntersect(pos0, pos1, pos2, testPos.y, &hit0Pos, &hit0, &hit1Pos, &hit1);
                            if (hit0 && hit0Pos.x > testPos.x)
                            {
                                DrawBox(videoBuffer, hit0Pos, float2(intersectSize, intersectSize), float3(1, 1, 1));
                                DrawFont(engineState->fontSpritesheet, videoBuffer, hit0Pos.x + 4, hit0Pos.y - 18, "-1");
                            }
                            if (hit1 && hit1Pos.x > testPos.x)
                            {
                                DrawBox(videoBuffer, hit1Pos, float2(intersectSize, intersectSize), float3(1, 1, 1));
                                DrawFont(engineState->fontSpritesheet, videoBuffer, hit1Pos.x + 4, hit1Pos.y - 18, "+1");
                            }
                        }

                        DrawLineBezier( videoBuffer, pos0, pos1, pos2, actualColor);
                        //DrawLine(       videoBuffer, pos0, pos1, actualColor);
                        //DrawLine(       videoBuffer, pos1, pos2, actualColor);
                        if (drawRealPoints)
                        {
                            DrawBox(videoBuffer, pos1, float2(3, 3), clamp01(actualColor * 2));
                        }
                        if (drawPhantomPoints)
                        {
                            DrawBox(videoBuffer, pos0, float2(3, 3), float3(1,0,0));
                            DrawBox(videoBuffer, pos2, float2(3, 3), float3(1,0,0));
                        }
                    }
                }
            }
        }
        //DrawPoint(videoBuffer, (boxMin + boxMax) / 2.0f);
        DrawLine(videoBuffer, float2(boxMin.x, -0), float2(boxMin.x, 1500), float3(0.5, 0.5, 0.5));
        DrawLine(videoBuffer, float2(boxMax.x, -0), float2(boxMax.x, 1500), float3(0.5, 0.5, 0.5));
        DrawLine(videoBuffer, float2(-0, boxMin.y), float2(1500, boxMin.y), float3(0.5, 0.5, 0.5));
        DrawLine(videoBuffer, float2(-0, boxMax.y), float2(1500, boxMax.y), float3(0.5, 0.5, 0.5));
        if (drawHelpers)
        {
        }
    }
    else if (state == DrawMultiGlyph)
    {
    // draw a bunch of glyphs

    int glyps[1] = { 3 };
    bool single = false;
    int count = ArrayCount(font->glyphs);
    if (single)
        count = ArrayCapacity(glyps);

    float size = 0.1f;
    if (single)
        size = 0.5f;

    for (int i = 0; i < ArrayCount(font->glyphs); i++)
    {
        int glyphIndex = i;
        if (single)
            glyphIndex = 111;// 16 * 7 - 1;

        Glyph* glyph = &font->glyphs[glyphIndex];
        float ss = 1100;

        int x = i % 16;
        int y = i / 16;

        float2 offset = float2(x * ss + ss * 1.1f, y * ss + ss * 1.1f) * size;
        if (single)
            offset = float2(x * ss + ss * 0.1f, y * ss + ss * 0.7f) * size;

        DrawBox(videoBuffer, offset, float2(2, 2), float3(1, 1, 1));

        float2 boxMin = float2(glyph->minX, glyph->minY) * size + offset;
        float2 boxMax = float2(glyph->maxX, glyph->maxY) * size + offset;
        float2 boxCenter = (boxMin + boxMax) / 2.0f;
        float2 boxSize = abs(boxMin - boxMax);
        DrawBox(videoBuffer, boxCenter, boxSize + float2(8, 8), float3(0.5f, 0.5f, 0.5f));
        //void DrawFont(Image * fontSpritesheet, VideoBuffer * videoBuffer, float X, float Y, const char* text, float R = 1.0, float G = 1.0, float B = 1.0, bool transparent = true, int LengthOverride = 0)

        char text[100] = {};
        Append(text, i);
        DrawFont(engineState->fontSpritesheet, videoBuffer, offset.x, offset.y, text);
        for (int j = 0; j < ArrayCount(glyph->contours); j++)
        {
            float3 actualColor = float3(frac((float)randHash(j + i) / 1000000.0f), frac((float)randHash(j + i + 20) / 1000000.0f), frac((float)randHash(j + i + 40) / 1000000.0f));
            if (ContourIsClockwise(&glyph->contours[j]))
                actualColor = float3(1, 0, 0);
            else
                actualColor = float3(0, 1, 0);

            GlyphContour* contour = &glyph->contours[j];
            for (int k = 0; k < ArrayCount(contour->glyphSegments); k++)
            {
                GlyphSegment* segment = &contour->glyphSegments[k];
                if (single)
                {
                    char text[100] = {};
                    Append(text, k);
                    DrawFont(engineState->fontSpritesheet, videoBuffer,
                        segment->pos2.x * size + offset.x,
                        segment->pos2.y * size + offset.y, text);

                }
                if (segment->isLine)
                {
                    DrawLine(videoBuffer, segment->pos0 * size + offset, segment->pos2 * size + offset, actualColor);
                    float2 middle = (segment->pos0 + segment->pos2) * 0.5f;
                    float2 middleLittle = lerp(segment->pos0, segment->pos2, 0.3);
                    float2 right = segment->pos0 - segment->pos2;
                    right = normalize(float2(-right.y, right.x)) * 20;
                    middleLittle += right;
                    DrawLine(videoBuffer, middle * size + offset, middleLittle * size + offset, actualColor);
                    middleLittle -= right * 2.0;
                    DrawLine(videoBuffer, middle * size + offset, middleLittle * size + offset, actualColor);
                }
                else
                {
                    DrawLineBezier(videoBuffer, segment->pos0 * size + offset, segment->pos1 * size + offset, segment->pos2 * size + offset, actualColor);
                }
            }
        }
    }
    }
}