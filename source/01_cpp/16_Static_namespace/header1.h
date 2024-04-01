namespace
{
    int anonymousInt = 9;
}

namespace MyName
{
    int namedInt = 8;
}

static int myStaticInt = 7; // header1.h被多个cpp文件包含，就会有多个myStaticInt，包含文件就相当于拷贝文件
int myGlobalInt = 6;