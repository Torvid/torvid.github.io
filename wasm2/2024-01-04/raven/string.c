#pragma once

typedef struct
{
	char* text;
	int length;
	int capacity;
} string;
string str(char* str)
{
	string result;
	result.text = str;
    result.length = 0;
    while (str[result.length] != 0)
    {
        result.length++;
    }
    return result;
}
int len(string str)
{
	return str.length;
}
#define StringCreate(name, capacity) char* name##_data[capacity] = {}; string name = {name##_data, 0, capacity};

string StringAppend(string result, char* src)
{
	string source = str(src);
	
	if(result.capacity < result.length+source.length)
		return result;

	for (int i = 0; i < source.length; i++)
	{
		result.text[result.length] = source.text[i];
		result.length++;
	}
	result.text[result.length] = 0; // add null-terminator just to be sure
	return result;
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
string StringAppendInt(string result, int a)
{
	if(result.capacity <= 0)
		return result;
    if (a == 0 || ((*(uint32*)&a) == 2147483648))
    {
		result = StringAppend(result, ("0"));
        return result;
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
        result.text[result.length] = '-';
		result.length++;
        offset = 1;
    }
	
	if(result.capacity < (digits + offset + 1))
        return result;

	//result.length = (digits + offset +1);

    for (int i = 0; i < (digits + 1); i++)
    {
        int d = (abs(a) / pow10(digits-i)) % 10;
        result.text[result.length] = d + 48;
		result.length++;
    }
	result.text[result.length] = 0;
	return result;
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
string StringAppendFloat(string result, float a)
{
	int precision = 4;
	if(result.capacity <= 0)
		return result;
	if(a != a)
	{
		result = StringAppend(result, ("NaN"));
        return result;
	}
    if (a == 0)
    {
		result = StringAppend(result, ("0.0"));
        return result;
    }

    int extraDigits = 1;
    int digits = digitCount(abs(a)) + 1;
    int intigerPart = floor(a);
    if (a < 0)
    {
        intigerPart = ceil(a);
        digits += 1;
    }

    int decimalPart = abs((float)((abs(a) - abs(intigerPart)) * pow10(precision)));

    char intigerPartString[100] = {};
    char decimalPartString[100] = {};
    if (a < 0 && intigerPart == 0)
    {
		result = StringAppend(result, ("-"));
		result = StringAppendInt(result, intigerPart);
    }
    else
	{
		result = StringAppendInt(result, intigerPart);
	}
    result = StringAppend(result, ("."));
    result = StringAppendInt(result, decimalPart);
	return result;

}
string StringAppendFloat2(string result, float2 a)
{
	result = StringAppend(result, "(");
	result = StringAppendFloat(result, a.x);
	result = StringAppend(result, ", ");
	result = StringAppendFloat(result, a.y);
	result = StringAppend(result, ")");
	return result;
}
string StringAppendFloat3(string result, float3 a)
{
	result = StringAppend(result, "(");
	result = StringAppendFloat(result, a.x);
	result = StringAppend(result, ", ");
	result = StringAppendFloat(result, a.y);
	result = StringAppend(result, ", ");
	result = StringAppendFloat(result, a.z);
	result = StringAppend(result, ")");
	return result;
}
