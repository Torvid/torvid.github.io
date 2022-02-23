
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
char* Append(char* Str, const char* value, int max = 100)
{
    int current_length = StringLength(Str);
    int newstring_length = StringLength(value);
    //int new_length = current_length + newstring_length;
    for (int i = 0; i < newstring_length; i++)
    {
        if (current_length + i >= max)
            break;
        Str[current_length + i] = value[i];
    }
    return Str;
}


void ToString(char* result, int a)
{
    if (a == 0)
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
        int d = ((int)abs(a) / current_number) % 10;
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

    int ExtraDigits = 1;
    int digits = digitCount(abs(a)) + 1;
    int IntigerPart = floor(a);
    if (a < 0)
    {
        IntigerPart = ceil(a);
        digits += 1;
        //precision -= 1;
    }

    int DecimalPart = abs((float)((abs(a) - abs(IntigerPart)) * pow10(precision)));

    char IntigerPartString[100];
    char DecimalPartString[100];
    if (a < 0 && IntigerPart == 0)
    {
        ToString(IntigerPartString+1, IntigerPart);
        IntigerPartString[0] = '-';
    }
    else
        ToString(IntigerPartString, IntigerPart);
    ToString(DecimalPartString, DecimalPart);

    CatStrings3(digits, IntigerPartString, 
        1, ".", 
        precision, DecimalPartString, 
        digits + 1 + precision, result);
}

//char GetText(game_input* Input)
//{
//    for (int i = 0; i < 256; i++)
//    {
//        if (Input->Text[i] == true)
//        {
//            return i;
//        }
//    }
//}

char* Append(char* Str, int value, int max = 100)
{
    char ConvertedString[100];
    ToString(ConvertedString, value);
    Append(Str, ConvertedString, max);
    return Str;
}

char* Append(char* Str, float value, int max = 100)
{
    char ConvertedString[100];
    ToString(ConvertedString, value);
    Append(Str, ConvertedString, max);
    return Str;
}
char* Append(char* Str, float3 value, int max = 100)
{
    char ConvertedString[100];
    ToString(ConvertedString, value.x);
    Append(Str, "float3(", max);
    ToString(ConvertedString, value.x);
    Append(Str, ConvertedString, max);
    Append(Str, ", ", max);
    ToString(ConvertedString, value.y);
    Append(Str, ConvertedString, max);
    Append(Str, ", ", max);
    ToString(ConvertedString, value.z);
    Append(Str, ConvertedString, max);
    Append(Str, ")", max);
    return Str;
}
char* Append(char* Str, float2 value, int max = 100)
{
    char ConvertedString[100];
    ToString(ConvertedString, value.x);
    Append(Str, "float2(", max);
    ToString(ConvertedString, value.x);
    Append(Str, ConvertedString, max);
    Append(Str, ", ", max);
    ToString(ConvertedString, value.y);
    Append(Str, ConvertedString, max);
    Append(Str, ")", max);
    return Str;
}
