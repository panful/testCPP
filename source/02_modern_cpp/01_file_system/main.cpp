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

#define TEST8

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
    {

    }

    // 读文件
    std::cout << "============================================" << std::endl;
    {
        std::ifstream ifs("fruit.csv", std::ios::in);
        std::string subStr;

        // 直到遇到','或'\n'停止读取
        // std::getline返回std::ios_base，std::ios_base重载了operator bool()
        while (std::getline(ifs, subStr, ','))
        {
            std::cout << subStr << '\n';
        }
        ifs.close();
    }

    // 读文件 getline()
    std::cout << "------------------------" << std::endl;
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
        ifs.close();
    }

    // 读文件 read() seekg() tellg()
    std::cout << "------------------------" << std::endl;
    {
        std::ifstream ifs("fruit.csv");
        ifs.seekg(0L, std::ios::end);
        long length = ifs.tellg(); // 获取文件长度
        ifs.seekg(0L, std::ios::beg);
        char* buf = new char[length + 1]();
        ifs.read(buf, length);
        ifs.close();
        std::cout << buf << '\n';
        delete[] buf;
        buf = nullptr;
    }

    // 读文件 read() eof()
    std::cout << "------------------------" << std::endl;
    {
        std::ifstream ifs("fruit.csv");
        std::stringstream ss;
        while (!ifs.eof())  // 注意有个 '!'
        {
            char* buf = new char[5]();
            ifs.read(buf, 4);
            ss.write(buf, 4);
            delete[] buf;
            buf = nullptr;
        }
        ifs.close();
        std::cout << ss.str() << '\n';
    }

    // 读文件 rdbuf()
    std::cout << "------------------------" << std::endl;
    {
        std::ifstream ifs("fruit.csv");
        std::filebuf* buf = ifs.rdbuf();
        std::cout << buf << '\n';
        ifs.close();
    }

    // 一次读取整个文件到std::string
    {
        {
            std::ifstream in("fruit.csv");
            std::istreambuf_iterator<char> beg(in), end;
            std::string str(beg, end);
        }
        // 或
        {
            std::ifstream in("fruit.csv");
            std::ostringstream tmp;
            tmp << in.rdbuf();
            std::string str = tmp.str();
        }
    }


    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <cstdio>

// fwrite fread write read
// fflush fputs fgets

int main()
{
    // 写文件
    {
        FILE* fp1 = fopen("test1.txt", "w");  // w表示写文件
        fputs("test fputs\t", fp1);
        fprintf(fp1, "test %s", "fprintf");
        fclose(fp1);
    }

    // 写文件
    {
        FILE* fp2 = fopen("test2.txt", "w");
        char str[] = "this is test for FILE fwrite";
        fwrite(str, sizeof(str), 1, fp2);  //二进制
        fclose(fp2);
    }

    // 写文件
    {
        FILE* fp1;
        fpos_t position;
        fp1 = fopen("file1.txt", "w+");  // w+表示读写文件
        int r = fgetpos(fp1, &position);
        fputs("hello,world.", fp1);
        fsetpos(fp1, &position);
        fputs("this is old position.", fp1);  //会覆盖"hello,world."
        fclose(fp1);
    }

    // 读文件
    {
        FILE* fp2 = fopen("test2.txt", "r");
        char str[30]{ 0 };
        fread(str, 29, 1, fp2); // 第二个参数表示读取的字节数
        printf("read:\t%s\n", str);
        fclose(fp2);
    }

    // 读文件
    {
        FILE* fp1 = fopen("test1.txt", "r+");
        char str[100]{ 0 };
        fgets(str, 99, fp1);
        printf("get:\t%s\n", str);
        fclose(fp1);
    }

    return 0;
}

#endif // TEST2

#ifdef TEST3

// https://zh.cppreference.com/w/cpp/locale

// fmt https://github.com/fmtlib/fmt

// icu https://icu.unicode.org/

// std::format

#if(0)

#include <format> //  c++20
#include <iostream>
#include <string>
#include <string_view>

template <typename... Args>
std::string dyna_print(std::string_view rt_fmt_str, Args&&... args) {
    return std::vformat(rt_fmt_str, std::make_format_args(args...));
}

int main() {
    std::cout << std::format("Hello {}!\n", "world");

    std::string fmt;
    for (int i{}; i != 3; ++i) {
        fmt += "{} "; // constructs the formatting string
        std::cout << fmt << " : ";
        std::cout << dyna_print(fmt, "alpha", 'Z', 3.14, "unused");
        std::cout << '\n';
    }
}

#endif

#include <iostream>
#include <locale>
#include <codecvt>
#include <string>

int main()
{
    std::locale l1;  // l1 is a copy of the classic "C" locale
    std::locale l2("en_US.UTF-8"); // l2 is a unicode locale
    std::locale l3(l1, l2, std::locale::ctype); // l3 is "C" except for ctype, which is unicode
    std::locale l4(l1, new std::codecvt_utf8<wchar_t>); // l4 is "C" except for codecvt
    std::cout << "Locale names:\nl1: " << l1.name() << "\nl2: " << l2.name()
        << "\nl3: " << l3.name() << "\nl4: " << l4.name() << '\n';

    std::wstring str(L"123abc中文");
    std::wcout << str << '\n'; // 中文打印不出来
}

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

// linux下使用-std=c++17还是报错：undefined reference to `std::filesystem***
// 添加选项 -lstdc++fs 即 g++ main.cpp -std=c++17 -lstdc++fs

#include <iostream>
#include <filesystem>
#include <fstream>
#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

int main(int argc, char** argv)
{
    {
        // Linux下打印当前可执行文件的相对路径
        // Windows下打印当前可执行文件的绝对路径
        std::cout << "The exe path:" << argv[0] << '\n';
    }

    std::cout << "----------------------------------------\n";

    {
        //返回的是工作目录，不是当前应用程序所在目录
        std::cout << "current_path:" << std::filesystem::current_path() << '\n';
        std::cout << "current_path.string():" << std::filesystem::current_path().string() << '\n';
    }

    {
        // 可执行文件如果在 aa/bb/cc/路径下，但是在控制台的aa目录下，以./bb/cc/file_name执行文件，
        // 则获取的是aa，并不是aa/bb/cc/file_name或aa/bb/cc
        // Windows和Linux结果一样，也就是getcwd获取的也是工作目录
        std::cout << "getcwd:" << getcwd(0, 0) << '\n';

        // @note getcwd(0,0)会有内存泄漏
        // char* path[512]{0}; getcwd(path,512);不会有内存泄漏
    }

    std::cout << "----------------------------------------\n";

    {
        try {
            std::cout << "std::filesystem::current_path(\"C:/bc\")\t";
            // 设置工作目录为"C:/bc"，如果"C:/bc"目录不存在则抛出异常
            std::filesystem::current_path("C:/bc");
        }
        catch (...) {
            std::cout << "============ \terror";
        }
        std::cout << "\ncurrent_path:\t" << std::filesystem::current_path() << '\n';
        std::cout << "getcwd:" << getcwd(0, 0) << '\n';
    }

    std::cout << "----------------------------------------\n";

    {
        try {
            std::cout << "std::filesystem::current_path(\"C:/Program Files\")\t";
            std::filesystem::current_path("C:/Program Files");
        }
        catch (...) {
            std::cout << "============ \terror";
        }
        // Windows下这里会打印 C:/Program Files
        std::cout << "\ncurrent_path:\t" << std::filesystem::current_path() << '\n';
        std::cout << "getcwd:" << getcwd(0, 0) << '\n';
    }

    std::cout << "----------------------------------------\n";

    {
        try {
            std::cout << "std::filesystem::current_path(\"/home\")\t";
            std::filesystem::current_path("/home");
        }
        catch (...) {
            std::cout << " ============ \terror";
        }
        // Linux下这里会打印 /home
        std::cout << "\ncurrent_path:\t" << std::filesystem::current_path() << '\n';
        std::cout << "getcwd:" << getcwd(0, 0) << '\n';
    }

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

namespace FS = std::filesystem;

// 以下结果都是在target目录下运行该程序

int main()
{
    std::cout << std::boolalpha;

    // 判断目录是否存在
    {
        std::filesystem::path myPath1("test");
        // "./test"目录存在则返回true
        auto exists1 = std::filesystem::exists(myPath1); // false

        // "./resource"目录是否存在
        auto exists2 = std::filesystem::exists(std::filesystem::path("resource")); // true
        auto exists3 = std::filesystem::exists(std::filesystem::path("C:/Users")); // true
        auto exists4 = std::filesystem::exists(std::filesystem::path("C:/users")); // true 目录名不区分大小写
        auto exists5 = std::filesystem::exists("C:/Program Files");                // true std::filesystem::path可以直接使用字符串

        std::cout << exists1 << '\t' << exists2 << '\t' << exists3 << '\t' << exists4 << '\t' << exists5 << '\n';
    }

    // 判断文件是否存在，和判断目录是一样的
    {
        std::filesystem::path myPath1("01_01_File.exe");
        auto exists1 = std::filesystem::exists(myPath1); // true

        auto exists2 = std::filesystem::exists(std::filesystem::path("AAAAAAAA")); // false
        auto exists3 = std::filesystem::exists(std::filesystem::path("<=-.txt"));  // false '<'是特殊字符，不可能存在该文件
        auto exists4 = std::filesystem::exists(std::filesystem::path("*.exe"));    // false '*'是特殊字符

        std::cout << exists1 << '\t' << exists2 << '\t' << exists3 << '\t' << exists4 << '\n';
    }

    // 创建目录
    {
        std::filesystem::path myPath1("resource");
        auto create1 = std::filesystem::create_directory(myPath1);                                  // false，resource目录已经存在
        auto create2 = std::filesystem::create_directory(std::filesystem::path("dir1"));            // true
        auto create3 = std::filesystem::create_directories(std::filesystem::path("dir2"));          // true，创建多层目录，即使只有一层也可以成功
        auto create4 = std::filesystem::create_directories(std::filesystem::path("dir3/abc/xyz"));  // true，创建多层目录
        auto create5 = std::filesystem::create_directories(std::filesystem::path("创建的目录\\2"));  // true，注意编码格式，可能会乱码

        std::cout << create1 << '\t' << create2 << '\t' << create3 << '\t' << create4 << '\t' << create5 << '\n';

        try
        {
            // 创建多层目录如果该目录存在则返回false
            auto create5 = std::filesystem::create_directory(std::filesystem::path("dir3/abc/xyz"));
            std::cout << create5 << '\n';
        }
        catch (...)
        {
            std::cout << R"(std::filesystem::create_directory(std::filesystem::path("dir3/abc/xyz"));)" << "\t\t error\n";
        }

        try
        {
            // 创建多层目录如果该目录不存在则抛出异常
            std::filesystem::create_directory(std::filesystem::path("dir4/abc/xyz"));
        }
        catch (...)
        {
            std::cout << R"(std::filesystem::create_directory(std::filesystem::path("dir4/abc/xyz"));)" << "\t\t error\n";
        }

        try
        {
            // 含有特殊字符，抛出异常
            std::filesystem::create_directory(std::filesystem::path("<*>"));
        }
        catch (...)
        {
            std::cout << R"(std::filesystem::create_directory(std::filesystem::path("<*>"));)" << "\t\t error\n";
        }

    }

    // 判断字符串是否是一个目录，目录存在返回true，否则返回false
    {
        auto is_directory1 = std::filesystem::is_directory("****");             // false
        auto is_directory2 = std::filesystem::is_directory("23<aa");            // false
        auto is_directory3 = std::filesystem::is_directory("sss\\ss");          // false
        auto is_directory4 = std::filesystem::is_directory("x:\\sss");          // false
        auto is_directory5 = std::filesystem::is_directory("c:/sss");           // false
        auto is_directory6 = std::filesystem::is_directory("test.jpg");         // false
        auto is_directory7 = std::filesystem::is_directory("C:/Users");         // true
        auto is_directory8 = std::filesystem::is_directory("C:\\users");        // true 不区分大小写
        auto is_directory9 = std::filesystem::is_directory("路径/ss");          // false
        auto is_directory10 = std::filesystem::is_directory("dir3/abc/xyz");    // true 可以判断多层目录

        std::cout << is_directory1 << '\t' << is_directory2 << '\t' << is_directory3 << '\t' << is_directory4 << '\n';
        std::cout << is_directory5 << '\t' << is_directory6 << '\t' << is_directory7 << '\t' << is_directory8 << '\n';
        std::cout << is_directory9 << '\t' << is_directory10 << '\n';

        try
        {
            // 由于编码的原因可能会抛出异常
            auto is_directory = std::filesystem::is_directory("创建的目录");
            std::cout << R"(std::filesystem::is_directory("创建的目录");)" << "\t\t yes\n";
        }
        catch (...)
        {
            std::cout << R"(std::filesystem::is_directory("创建的目录");)" << "\t\t error\n";
        }
    }

    // 提取文件名
    {
        {
            std::filesystem::path myPath("test.jpg");

            auto has_filename = myPath.has_filename();
            auto filename = myPath.filename();     // L"test.jpg" 文件名，包括后缀

            auto has_extension = myPath.has_extension();
            auto extension = myPath.extension();   // L".jpg" 文件名后缀，即文件类型

            auto has_stem = myPath.has_stem();
            auto stem = myPath.stem();             // L"test" 文件名，不包括后缀
        }

        {
            std::filesystem::path myPath("C:/abc");

            auto has_filename = myPath.has_filename();
            auto filename = myPath.filename();     // L"abc" 文件名，包括后缀

            auto has_extension = myPath.has_extension();
            auto extension = myPath.extension();   // L"" 文件名后缀，此处为空

            auto has_stem = myPath.has_stem();
            auto stem = myPath.stem();             // L"abc" 文件名，不包括后缀
        }
    }
    std::cout << "-----------------------------------------------------\n";
    {
        // parent_path      文件所在目录，即最后一个"/"之前的字符串，如果没有"/"则返回空
        // filename         文件名，包括文件类型，即最后一个"/"之后的字符串，如果没有"/"则返回整个字符串
        // extension        文件类型 即最后一个"."之后的字符串，如果没有"."则返回空
        // stem             文件名，不包括文件类型
        // relative_path    相对路径，除过根目录之外的字符串，包括文件名

        {
            std::filesystem::path path1("C:/ABC/xyz/test.txt");

            auto has_extension = path1.has_extension();
            auto extension = path1.extension();

            auto has_filename = path1.has_filename();
            auto filename = path1.filename();

            auto has_parent_path = path1.has_parent_path();
            auto parent_path = path1.parent_path();

            auto has_relative_path = path1.has_relative_path();
            auto relative_path = path1.relative_path();

            auto has_root_directory = path1.has_root_directory();
            auto root_directory = path1.root_directory();

            auto has_root_name = path1.has_root_name();
            auto root_name = path1.root_name();

            auto has_root_path = path1.has_root_path();
            auto root_path = path1.root_path();

            auto has_stem = path1.has_stem();
            auto stem = path1.stem();

            std::cout << "path: " << path1 << '\n';
            std::cout << "---has_extension: " << has_extension << '\n' << "extension" << extension << '\n';
            std::cout << "---has_filename: " << has_filename << '\n' << "filename" << filename << '\n';
            std::cout << "---has_parent_path: " << has_parent_path << '\n' << "parent_path" << parent_path << '\n';
            std::cout << "---has_relative_path: " << has_relative_path << '\n' << "relative_path" << relative_path << '\n';
            std::cout << "---has_root_directory: " << has_root_directory << '\n' << "root_directory" << root_directory << '\n';
            std::cout << "---has_root_name: " << has_root_name << '\n' << "root_name" << root_name << '\n';
            std::cout << "---has_root_path: " << has_root_path << '\n' << "root_path" << root_path << '\n';
            std::cout << "---has_stem: " << has_stem << '\n' << "stem" << stem << '\n';

        }
        std::cout << "--------------------------------------\n";
        {
            std::filesystem::path path1("./XXX/test.txt");

            auto has_extension = path1.has_extension();
            auto extension = path1.extension();

            auto has_filename = path1.has_filename();
            auto filename = path1.filename();

            auto has_parent_path = path1.has_parent_path();
            auto parent_path = path1.parent_path();

            auto has_relative_path = path1.has_relative_path();
            auto relative_path = path1.relative_path();

            auto has_root_directory = path1.has_root_directory();
            auto root_directory = path1.root_directory();

            auto has_root_name = path1.has_root_name();
            auto root_name = path1.root_name();

            auto has_root_path = path1.has_root_path();
            auto root_path = path1.root_path();

            auto has_stem = path1.has_stem();
            auto stem = path1.stem();

            std::cout << "path: " << path1 << '\n';
            std::cout << "---has_extension: " << has_extension << '\n' << "extension" << extension << '\n';
            std::cout << "---has_filename: " << has_filename << '\n' << "filename" << filename << '\n';
            std::cout << "---has_parent_path: " << has_parent_path << '\n' << "parent_path" << parent_path << '\n';
            std::cout << "---has_relative_path: " << has_relative_path << '\n' << "relative_path" << relative_path << '\n';
            std::cout << "---has_root_directory: " << has_root_directory << '\n' << "root_directory" << root_directory << '\n';
            std::cout << "---has_root_name: " << has_root_name << '\n' << "root_name" << root_name << '\n';
            std::cout << "---has_root_path: " << has_root_path << '\n' << "root_path" << root_path << '\n';
            std::cout << "---has_stem: " << has_stem << '\n' << "stem" << stem << '\n';
        }
    }

    {
        std::filesystem::path path1("C:/ABC/xyz/test.txt");

        path1.replace_extension("aaa"); // 将文件类型改为aaa即"txt"改为"aaa"
        path1.replace_filename("aaa");  // 会将文件名包括后缀一起替换为"aaa"
        path1.remove_filename();

        auto is_absolute = path1.is_absolute();
        auto is_relative = path1.is_relative();
        auto lexically_normal = path1.lexically_normal();
        auto lexically_proximate = path1.lexically_proximate("nnn");
        auto lexically_relative = path1.lexically_relative("mmm");
    }

    return 0;
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
