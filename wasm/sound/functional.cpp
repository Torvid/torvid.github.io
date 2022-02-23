
struct FunctionWithContext;

typedef void* (uberfunction)(void* x, FunctionWithContext f);

struct FunctionWithContext
{
    uberfunction* function;
    void* context;
};

FunctionWithContext MakeFunctionWithContext(uberfunction* function, void* context)
{
    FunctionWithContext Add1;
    Add1.function = function;
    Add1.context = context;
    return Add1;
}

//void Map(int* thing, int size, faf* f, void* context)
//{
//    for (int i = 0; i < size; i++)
//    {
//        thing[i] = f(thing[i], context);
//    }
//}
//

void* Apply(FunctionWithContext f, void* y)
{
    return f.function(y, f);
}
void* Add(void* y, FunctionWithContext f)
{
    return (void*)(*((int*)f.context) + *((int*)y));
}

void Test2()
{
    int one = 1;
    FunctionWithContext Add1 = MakeFunctionWithContext(&Add, &one);

    int value = 3;
    int LOL = (int)Apply(Add1, &value);
    print(LOL);
}

