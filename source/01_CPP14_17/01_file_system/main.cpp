/*
* 1. C++读写文件 https://www.cnblogs.com/mmmmmmmmm/p/14136211.html
* 2. C风格读写文件
* 3. std::local std::format 本地化库 中文路径 编码转换 utf-8 unicode
* 4. std::setw std::setfill 控制流中的字符空格，打印格式
* 5. filesystem getcwd 获取当前路径
* 6. 压缩解压缩
* 7. 判断文件编码格式
* 8. filesystem 常用函数
* 9. 读取二进制文件图片，将读取的图片写入一个新的文件
* 10.filesystem相对路径绝对路径
* 11.多线程读写文件，还没弄好
*/

// 关于流(stream)的使用可以查看 00_08

#define TEST1

#ifdef TEST1

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[])
{
    // 写文件
    {
        std::ofstream ofs;
        ofs.open("fruit.csv", std::ios::out); // 打开模式可省略
        ofs << "Apple" << ',' << 5.5 << ',' << "2.2kg" << '\n';
        ofs << "Banana" << ',' << 4.5 << ',' << "1.5kg" << '\n';
        ofs << "Grape" << ',' << 6.8 << ',' << "2.6kg" << '\n';

        ofs.close();
    }

    // 读文件
    std::cout << "-----第一种方式------" << std::endl;
    {
        std::ifstream ifs("fruit.csv", std::ios::in);
        std::string subStr;

        // 直到遇到','或'\n'停止读取
        // std::getline返回std::ios_base，std::ios_base重载了operator bool()
        while (std::getline(ifs, subStr, ','))
        {
            std::cout << subStr << '\n';
        }
    }
    // 读文件
    std::cout << "-----第二种方式------" << std::endl;
    {
        std::string subLine;
        std::ifstream ifs("fruit.csv", std::ios::in);  //此处需要重读一遍文件，因为前面的ifs已经读取完毕
        // 直到遇到'\n'停止读取
        // 先将每一行内容读取到std::string
        while (std::getline(ifs, subLine))
        {
            // 将刚才读取的一行内容构造为std::stream
            std::stringstream ss(subLine);
            std::string subStr;
            // 从刚才读取的一行中再读取','
            while (std::getline(ss, subStr, ','))
            {
                std::cout << subStr << '\n';
            }
        }
    }

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <cstdio>

// fwrite write fread read
// fflush

int main()
{
    //FILE* fp1,* fp2;
    //char str[] = "this is test for FILE fwrite";


    //fp1 = fopen("test1.txt", "w" );
    //fp2 = fopen("test2.txt", "w");

    //fputs("test fputs\t", fp1);

    //fprintf(fp1, "test %s", "fprintf");

    //fwrite(str, sizeof(str), 1, fp2);  //二进制

    //fclose(fp1);
    //fclose(fp2);

    FILE* fp1;
    fpos_t position;
    fp1 = fopen("file1.txt", "w+");
    int r = fgetpos(fp1, &position);
    fputs("hello,world.", fp1);
    fsetpos(fp1, &position);
    fputs("this is old position.", fp1);  //会覆盖"hello,world."
    fclose(fp1);

    return 0;
}

#endif // TEST2

#ifdef TEST3

// https://zh.cppreference.com/w/cpp/locale

// icu https://icu.unicode.org/

#include <iostream>
#include <locale>

int main()
{
    std::wcout << "User-preferred locale setting is " << std::locale("").name().c_str() << '\n';
    // 在启动时，全局本地环境是 "C" 本地环境
    std::wcout << 1000.01 << '\n';
    // 以用户偏好的本地环境替换 C++ 本地环境和 C 本地环境
    std::locale::global(std::locale(""));
    // 将来的宽字符输出使用新的全局本地环境
    std::wcout.imbue(std::locale());
    // 再次输出同一数字
    std::wcout << 1000.01 << '\n';
}

//#include <iostream>
//#include <ctime>
//#include <iomanip>
//#include <codecvt>
//
//int main()
//{
//    std::time_t t = std::time(NULL);
//    std::wbuffer_convert<std::codecvt_utf8<wchar_t>> conv(std::cout.rdbuf());
//    std::wostream out(&conv);
//    out.imbue(std::locale("ja_JP.utf8"));
//    // 此 I/O 操纵符 std::put_time 使用 std::time_put<wchar_t>
//    out << std::put_time(std::localtime(&t), L"%A %c") << '\n';
//
//    return 0;
//}

#endif // TEST3

#ifdef TEST4

#include <iostream>
#include <iomanip>
#include <locale>

int main()
{
    // 左对齐，"123"总共占10个字符宽度，其余7个位置使用'*'代替
    std::cout << std::left << std::setfill('*') << std::setw(10) << "123" << "abc" << std::endl;
}

#endif // TEST4

#ifdef TEST5

#include <iostream>
#include <filesystem>
#include <fstream>
#include <direct.h>

int main()
{
    //返回的是工作目录，不是当前应用程序所在目录
    auto path = std::filesystem::current_path();
    auto path1 = _getcwd(nullptr, 10);

    std::ifstream ifs;

    ifs.open(path.append("filesystem.txt"));

    if (ifs)
    {
        std::cout << "good\n";
    }

    std::filesystem::current_path("C:\\bc");       // "c:\\bc"目录如果不存在则抛出异常
    auto path2 = std::filesystem::current_path();  // C:\bc

}

#endif // TEST5

#ifdef TEST6

#include <windows.h>
#include <iostream>
using namespace std;

typedef    NTSTATUS(WINAPI* pRtlGetCompressionWorkSpaceSize)(
    _In_  USHORT CompressionFormatAndEngine,
    _Out_ PULONG CompressBufferWorkSpaceSize,
    _Out_ PULONG CompressFragmentWorkSpaceSize
    );
typedef    NTSTATUS(WINAPI* pRtlCompressBuffer)(
    _In_  USHORT CompressionFormatAndEngine,
    _In_  PUCHAR UncompressedBuffer,
    _In_  ULONG  UncompressedBufferSize,
    _Out_ PUCHAR CompressedBuffer,
    _In_  ULONG  CompressedBufferSize,
    _In_  ULONG  UncompressedChunkSize,
    _Out_ PULONG FinalCompressedSize,
    _In_  PVOID  WorkSpace
    );

typedef    NTSTATUS(WINAPI* pRtlDecompressBuffer)(
    _In_  USHORT CompressionFormat,
    _Out_ PUCHAR UncompressedBuffer,
    _In_  ULONG  UncompressedBufferSize,
    _In_  PUCHAR CompressedBuffer,
    _In_  ULONG  CompressedBufferSize,
    _Out_ PULONG FinalUncompressedSize
    );

pRtlGetCompressionWorkSpaceSize    RtlGetCompressionWorkSpaceSize = NULL;
pRtlCompressBuffer                RtlCompressBuffer = NULL;
pRtlDecompressBuffer            RtlDecompressBuffer = NULL;
bool    InitAPI() {
    auto    hDll = LoadLibrary("ntdll.dll");
    if (!hDll) {
        return FALSE;
    }
    RtlGetCompressionWorkSpaceSize = (pRtlGetCompressionWorkSpaceSize)GetProcAddress(hDll, "RtlGetCompressionWorkSpaceSize");
    if (!RtlGetCompressionWorkSpaceSize) {
        return FALSE;
    }
    RtlCompressBuffer = (pRtlCompressBuffer)GetProcAddress(hDll, "RtlCompressBuffer");
    if (!RtlCompressBuffer) {
        return FALSE;
    }
    RtlDecompressBuffer = (pRtlDecompressBuffer)GetProcAddress(hDll, "RtlDecompressBuffer");
    if (!RtlDecompressBuffer) {
        return FALSE;
    }
    return TRUE;
}

bool    Compress(BYTE* data, DWORD size, BYTE* buffer, DWORD& bufsize) {
    DWORD dwWorkSpaceSize = 0, dwFragmentWorkSpaceSize = 0;
    BYTE* pWorkSpace = NULL;
    BYTE* pCompressData = NULL;
    NTSTATUS    status = FALSE;
    DWORD dwCompressDataLength = 4096;
    DWORD dwFinalCompressSize = 0;
    status = RtlGetCompressionWorkSpaceSize(COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_STANDARD, &dwWorkSpaceSize, &dwFragmentWorkSpaceSize);
    if (status != 0) {
        return FALSE;
    }
    pWorkSpace = new BYTE[dwWorkSpaceSize];
    if (NULL == pWorkSpace)
    {
        return FALSE;
    }
    ::RtlZeroMemory(pWorkSpace, dwWorkSpaceSize);
    status = RtlCompressBuffer(COMPRESSION_FORMAT_LZNT1, data, size, buffer, bufsize, 4096, &dwFinalCompressSize, (PVOID)pWorkSpace);
    if (status != 0) {
        return FALSE;
    }
    delete[] pWorkSpace;
    bufsize = dwFinalCompressSize;
    return TRUE;
}

bool    Decompress(BYTE* data, DWORD size, BYTE* buffer, DWORD& bufsize) {
    DWORD dwWorkSpaceSize = 0, dwFragmentWorkSpaceSize = 0;
    BYTE* pWorkSpace = NULL;
    BYTE* pCompressData = NULL;
    NTSTATUS    status = FALSE;
    DWORD dwCompressDataLength = 4096;
    DWORD dwFinalCompressSize = 0;
    status = RtlGetCompressionWorkSpaceSize(COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_STANDARD, &dwWorkSpaceSize, &dwFragmentWorkSpaceSize);
    if (status != 0) {
        return FALSE;
    }
    pWorkSpace = new BYTE[dwWorkSpaceSize];
    if (pWorkSpace == NULL) {
        return FALSE;
    }
    RtlZeroMemory(pWorkSpace, dwWorkSpaceSize);
    status = RtlDecompressBuffer(COMPRESSION_FORMAT_LZNT1, buffer, bufsize, data, size, &dwFinalCompressSize);
    if (status != 0) {
        return FALSE;
    }
    delete[] pWorkSpace;
    bufsize = dwFinalCompressSize;
    return TRUE;
}

int main()
{

    //BYTE    data[] = "DDDDDDDDDDGGGGGGGGGGGG";
    BYTE    data[] = "12345678912345678900aa";
    auto    buffer = new BYTE[100];
    DWORD        size = 100;
    if (!InitAPI()) {
        cout << "InitFailed" << endl;
        return -1;
    }
    if (Compress(data, 22, buffer, size)) {
        for (int i = 0; i < size; i++) {
            printf("%02X ", buffer[i]);
        }

        std::cout << "====\n";
        DWORD bufSize = 100;
        auto destBuf = new BYTE[bufSize];

        if (!destBuf) {
            return -1;
        }
        if (Decompress(buffer, size, destBuf, bufSize)) {
            for (int i = 0; i < bufSize; i++) {
                printf("%c ", destBuf[i]);
            }
        }
        std::cout << "====\n";

    }
    else {
        cout << "Failed" << endl;
    }
    delete[] buffer;
    getchar();
    return 0;
}

#endif // TEST6

#ifdef TEST7

//判断⽂件编码格式
//ANSI：        ⽆格式定义；
//Unicode：       前两个字节为FFFE；
//Unicode big endian： 前两字节为FEFF；
//UTF - 8：        前两字节为EFBB；

#include <iostream>
#include <fstream>
using namespace std;

int main()
{
    ifstream fin("test.txt", ios::binary);
    if (!fin)
    {
        std::cout << "read error!\n";
        return 0;
    }
    unsigned char  s2;
    fin.read((char*)&s2, sizeof(s2));//读取第⼀个字节，然后左移8位    
    int p = s2 << 8;
    fin.read((char*)&s2, sizeof(s2));//读取第⼆个字节    
    p |= s2;     string code;
    switch (p)//判断⽂本前两个字节    
    {
    case 0xfffe:  //65534        
        code = "Unicode";
        break;
    case 0xfeff:  //65279        
        code = "Unicode big endian";
        break;
    case 0xefbb:  //61371        
        code = "UTF-8";
        break;
    break; default:
        code = "ANSI";
    }

    std::cout << "coding = " << code << '\n';
    fin.close();

    return 0;
}
#endif // TEST7

#ifdef TEST8

#include <filesystem>
#include <iostream>

//using namespace std::filesystem;

int main()
{
    // 判断目录是否存在
    std::filesystem::path myPath1("test");
    auto exists1 = std::filesystem::exists(myPath1);  //如果myPath目录存在则返回true

    // 判断文件是否存在
    std::filesystem::path myFilePath1("test.bmp");
    auto fileExits1 = std::filesystem::exists(myFilePath1);

    std::filesystem::path myPath2("sssssssss");
    auto exists2 = std::filesystem::exists(myPath2);

    std::filesystem::path myPath3("***");
    auto exists3 = std::filesystem::exists(myPath3); //false

    //auto create1 = std::filesystem::create_directory(myPath2);
    auto create1 = std::filesystem::create_directories(myPath2);
    auto create2 = std::filesystem::create_directory("C\\s");  //创建多级目录失败，必须使用create_directories
    //auto create3 = std::filesystem::create_directories("******");  //抛出异常
    auto is_directory1 = std::filesystem::is_directory("****");
    auto is_directory2 = std::filesystem::is_directory("23<aa");
    auto is_directory3 = std::filesystem::is_directory("sss?ss");
    auto is_directory4 = std::filesystem::is_directory("x:\\sss");
    auto is_directory5 = std::filesystem::is_directory("c:\\sss");
    auto is_directory6 = std::filesystem::is_directory("test.jpg");
    auto is_directory7 = std::filesystem::is_directory("C:\\Users\\yangpan\\Desktop");

    std::filesystem::path myPath4("test.jpg");
    auto has_name = myPath4.has_filename();
    auto name = myPath4.filename();  //L"test.jpg"  // 文件名，包括后缀
    auto has_extension = myPath4.has_extension();
    auto extension = myPath4.extension();  //L".jpg" // 文件名后缀，即文件类型
    auto has_stem = myPath4.has_stem();
    auto stem = myPath4.stem();  //L"test"  // 文件名，不包括后缀

    std::filesystem::path path5("sss\\test\\fff");
    std::filesystem::path path6("\\**ss**\\test\\fff");
    std::filesystem::path path7("sss<rrr");
    std::filesystem::path path8("sss\\test\\fff\\.test.txt");
    std::filesystem::path path9("sss\\test\\fff\\");
    std::filesystem::path path10("sss\\test\\fff\\te**st.txt");

    //parent_path 返回父路径，即最后一个"\\"之前的字符串，如果没有"\\"则返回空
    auto parPath5 = path5.parent_path(); //L"sss\\test"
    auto parPath6 = path6.parent_path(); //L"c:\\sss\\test"
    auto parPath7 = path7.parent_path(); //L""
    auto parPath8 = path8.parent_path(); //L"sss\\test\\fff"
    auto parPath9 = path9.parent_path(); //L"sss\\test\\fff"
    auto parPath10 = path10.parent_path(); //L"sss\\test\\fff"

    auto fielname5 = path5.filename();   //
    auto fielname6 = path6.filename();   //
    auto fielname7 = path7.filename();   //
    auto fielname8 = path8.filename();   //
    auto fielname9 = path9.filename();   //
    auto fielname10 = path10.filename(); //

}
#endif // TEST8

#ifdef TEST9

// read a file into memory
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

int main() {

    std::ifstream is("test.bmp", std::ifstream::binary);
    std::ofstream os("out.bmp", std::ifstream::binary);
    if (is) {
        // get length of file:
        is.seekg(0, is.end);
        int length = is.tellg();
        is.seekg(0, is.beg);

        char* buffer = new char[length];

        std::cout << "Reading " << length << " characters... \n";
        // read data as a block:
        is.read(buffer, length);

        if (is)
        {
            std::cout << "all characters read successfully.";
        }
        else
        {
            // gcount返回的是读取的字符数
            std::cout << "error: only " << is.gcount() << " could be read";
        }

        is.close();

        // ...buffer contains the entire file...

        os.write(buffer, length);

        delete[] buffer;
    }

    return 0;
}

#endif // TEST9

#ifdef TEST10

#include <fstream>
#include <direct.h>
#include <string>
#include <filesystem>

int main()
{
    auto path = _getcwd(nullptr, 255);
    std::ifstream ifs;
    try
    {
        std::ifstream ifs1("file/s");
        if (!ifs1)
            throw "error";
        //ifs.open("../file/s");
        //if (ifs)
        //    throw "error";
        std::filesystem::current_path("C:\\bc");
        std::string s;
        ifs.open("main.cpp");
        std::getline(ifs, s);

        if (!ifs)
            throw "error";
    }
    catch (...)
    {
        int test = 0;
    }
    return 0;
}

#endif // TEST10

#ifdef TEST11

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>

std::mutex m;

void readfile(std::string path, long posStart, long long size, std::string& str)
{
    std::ifstream ifs(path);  //必须在此处打开文件，不能以引用的方式在参数列表中传入fstream对象
    ifs.seekg(posStart, ifs.beg);   //定位到指定字节位置
    std::string outStr;

    const int readSize = 6;
    while (true)
    {
        char buf[readSize + 1] = { 0 };
        if (size < readSize)
        {
            ifs.read(buf, size);
            outStr += buf;
            break;
        }
        else
        {
            ifs.read(buf, readSize);
            size -= readSize;
            outStr += buf;
            ifs.seekg(posStart + readSize, ifs.beg);
        }
    }




    //std::lock_guard<std::mutex> lock(m);
    str = outStr;
    //std::cout << "====" << outStr << '\n';
}

int main()
{



    const int cpuNum = 4;
    std::string path("file.txt");
    std::ifstream infile(path);  //加载文件

    //infile.seekg(3, infile.beg);
    char buf[512] = { 0 };
    infile.read(buf, 25);
    //infile.read(buf, 4);
    //long now = infile.tellg();


    infile.seekg(0, infile.end);
    long length = infile.tellg();//获取文件字节数

    long block = (length + 2) / cpuNum;//根据cpu数目对文件分块
    block = 6;
    std::vector<std::thread> t;


    std::string s;
    for (int i = 0; i < cpuNum; i++)
    {
        std::string str;
        long start = i * block + i;
        readfile(path, start, block, str);

        s += str;


        //readfile(infile, posStart, posEnd, str);

        //t.emplace_back(std::thread(readfile, path, posStart, posEnd, std::ref(str)));
    }
    std::cout << s;
    //for (int i = 0; i < cpuNum; i++)
    //{
    //    t[i].join();
    //}

    //int sss = 0;

    //char s = 'a';


    //std::ifstream infile("file.txt");
    //std::string str;
    //long pos1 = infile.tellg();
    //std::getline(infile, str);
    //long pos2 = infile.tellg();

    //infile.seekg(15, infile.beg);
    //if (std::getline(infile, str))
    //    std::cout << "true";
    //long pos4 = infile.tellg();

    //infile.seekg(13, infile.beg);
    //if (std::getline(infile, str))
    //    std::cout << "true";
    //long pos5 = infile.tellg();

    //infile.seekg(14, infile.beg);
    //if (std::getline(infile, str))
    //    std::cout << "true";
    //long pos6 = infile.tellg();

    //infile.seekg(16, infile.beg);
    //if (std::getline(infile, str))
    //    std::cout << "true";
    //long pos7 = infile.tellg();

    //infile.seekg(1, infile.beg);
    //std::getline(infile, str);
    ////infile.seekg(0,infile.end);
    //infile.seekg(5, infile.beg);
    //

    ////infile.seekg(pos + 12);

    //std::string str;
    //char buf[1024] = { 0 };
    //infile.get(buf, 10);
    //long pos1 = infile.tellg();
    //char buf1[1024] = { 0 };
    //infile.seekg(pos1 + 2);   //换行加2
    //infile.getline(buf1, 5);
    //long pos2 = infile.tellg();
    ////infile.getline(str, 19);

    //std::getline(infile, str);
    //long posx = infile.tellg();






    int x = 0;
}

#endif // TEST11
