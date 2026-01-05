#pragma once
#include "quote.h"

int StringLength(const char* checkString)
{
    int i = 0;
    while (checkString[i] != 0)
    {
        i++;
    }
    return i;
}

void StringCopy(const char* string, char* target)
{
    for (int i = 0; i < StringLength(string); i++)
    {
        target[i] = string[i];
    }
}

bool EndsWith(char* inputString, const char* checkString)
{
    int endOfString = StringLength(inputString);
    int checkStringSize = StringLength(checkString);

    // Loop through string to check if the ending matches
    for (int i = 0; i < checkStringSize; i++)
    {
        if (checkString[i] != inputString[(endOfString - checkStringSize) + i])
        {
            return false;
        }
    }
    return true;
}
void CatStrings(int SourceACount, const char* SourceA,
    int SourceBCount, const char* SourceB,
    int DestCount, char* Dest)
{
    char* DestCounter = Dest;
    for (int i = 0; i < SourceACount; i++)
    {
        *DestCounter++ = *SourceA++;
    }

    for (int i = 0; i < SourceBCount; i++)
    {
        *DestCounter++ = *SourceB++;
    }

    *DestCounter++ = 0;
}
void CatStrings3(int SourceACount, const char* SourceA,
    int SourceBCount, const char* SourceB,
    int SourceCCount, const char* SourceC,
    int DestCount, char* Dest)
{
    char* DestCounter = Dest;
    for (int i = 0; i < SourceACount; i++)
    {
        *DestCounter++ = *SourceA++;
    }

    for (int i = 0; i < SourceBCount; i++)
    {
        *DestCounter++ = *SourceB++;
    }

    for (int i = 0; i < SourceCCount; i++)
    {
        *DestCounter++ = *SourceC++;
    }

    *DestCounter++ = 0;
}
char* Append(char* str, const char* value, int max = 100)
{
    int current_length = StringLength(str);
    int newstring_length = StringLength(value);
    for (int i = 0; i < newstring_length; i++)
    {
        if (current_length + i >= max)
            break;
        str[current_length + i] = value[i];
    }
    return str;
}

int iabs(int a)
{
    if (a < 0)
        return -a;
    return a;
}

void ToString(char* result, int a)
{
    if (a == 0 || ((*(uint32*)&a) == 2147483648))
    {
        result[0] = '0';
        result[1] = 0;
        return;
    }

    int num = a;
    int digits = -1;
    do
    {
        digits++;
        num /= 10;
    } while (num != 0);


    int offset = 0;
    if (a < 0)
    {
        result[0] = '-';
        offset = 1;
    }

    for (int i = 0; i < (digits + 1); i++)
    {
        int current_number = 1;
        // 0, 10, 100, 1000
        for (int j = 0; j < i; j++)
        {
            current_number *= 10;
        }
        int d = ((int)iabs(a) / current_number) % 10;
        result[(digits - i) + offset] = d + 48;
    }
    if (a < 0)
    {
        result[(digits + 2)] = 0;
    }
    else
    {
        result[(digits + 1)] = 0;
    }
}
void ToString(char* result, bool value)
{
    if (value != 0)
    {
        Append(result, "true");
    }
    else
    {
        Append(result, "false");
    }
}

int pow10(int count)
{
    int current_number = 1;
    // 0, 10, 100, 1000
    for (int j = 0; j < count; j++)
    {
        current_number *= 10;
    }
    return current_number;
}
int digitCount(int a)
{
    int num = a;
    int digits = -1;
    do
    {
        digits++;
        num /= 10;
    } while (num != 0);
    return digits;
}

void ToString(char* result, float a, int precision = 4)
{
    if (a == 0)
    {
        result[0] = '0';
        result[1] = 0;
        return;
    }

    int extraDigits = 1;
    int digits = digitCount(abs(a)) + 1;
    int intigerPart = floor(a);
    if (a < 0)
    {
        intigerPart = ceil(a);
        digits += 1;
        //precision -= 1;
    }

    int decimalPart = abs((float)((abs(a) - abs(intigerPart)) * pow10(precision)));

    char intigerPartString[100] = {};
    char decimalPartString[100] = {};
    if (a < 0 && intigerPart == 0)
    {
        ToString(intigerPartString+1, intigerPart);
        intigerPartString[0] = '-';
    }
    else
        ToString(intigerPartString, intigerPart);
    ToString(decimalPartString, decimalPart);

    CatStrings3(digits, intigerPartString, 
        1, ".", 
        precision, decimalPartString, 
        digits + 1 + precision, result);
}

char* Append(char* str, int value, int max = 100)
{
    char convertedString[100] = {};
    ToString(convertedString, value);
    Append(str, convertedString, max);
    return str;
}

char* Append(char* str, float value, int max = 100, int precision = 4)
{
    char convertedString[100] = {};
    ToString(convertedString, value, precision);
    Append(str, convertedString, max);
    return str;
}
char* Append(char* str, float3 value, int max = 100)
{
    char convertedString[100] = {};
    ToString(convertedString, value.x);
    Append(str, "(", max);
    ToString(convertedString, value.x);
    Append(str, convertedString, max);
    Append(str, ", ", max);
    ToString(convertedString, value.y);
    Append(str, convertedString, max);
    Append(str, ", ", max);
    ToString(convertedString, value.z);
    Append(str, convertedString, max);
    Append(str, ")", max);
    return str;
}
// Takes the bit-pattern of a float and smashes it to an int.
char* CoerceAppend(char* str, float3 value, int max = 100)
{
    if (value.x == 0)
        value.x = 0;
    if (value.y == 0)
        value.y = 0;
    if (value.z == 0)
        value.z = 0;
    char convertedString[100] = {};
    Append(str, "float3_coerce(", max);
    Append(str, *(int*)&value.x, max);
    Append(str, ", ", max);
    Append(str, *(int*)&value.y, max);
    Append(str, ", ", max);
    Append(str, *(int*)&value.z, max);
    Append(str, ")", max);
    return str;
}
// Takes the bit-pattern of a float and smashes it to an int.
char* CoerceAppend(char* str, float value, int max = 100)
{
    char convertedString[100] = {};
    Append(str, "float_coerce(", max);
    Append(str, *(int*)&value, max);
    Append(str, ")", max);
    return str;
}


char* Append(char* str, float2 value, int max = 100)
{
    char convertedString[100] = {};
    ToString(convertedString, value.x);
    Append(str, "(", max);
    ToString(convertedString, value.x);
    Append(str, convertedString, max);
    Append(str, ", ", max);
    ToString(convertedString, value.y);
    Append(str, convertedString, max);
    Append(str, ")", max);
    return str;
}

int FromString(const char* str)
{
    int value = 0;
    while (*str >= '0' && *str <= '9')
    {
        value = value * 10 + (*str - '0');
        str++;
    }
    return value;
}