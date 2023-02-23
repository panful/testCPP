/*
1. 使用流转换类型
2. 设置流的精度，格式
3. 将vector bool int float 空char* 等写入流，从流中读取这些类型
4. char*分割  strcat strcpy memcpy
5. std::fill std::copy std::fill_n
6. stringstream常用函数 seekg tellg seekp tellp getline get put write read gcount
7. 超大文件
*/

// 为什么说std::stringstream性能不好 https://zhuanlan.zhihu.com/p/467024561
// https://zhuanlan.zhihu.com/p/556307241?utm_campaign=shareopn&utm_medium=social&utm_oi=37296107683840&utm_psn=1545385266965278721&utm_source=wechat_session

// c++的缺陷和思考 https://blog.csdn.net/fl2011sx?type=blog

#define TEST4


#ifdef TEST1

#include <iostream>
#include <sstream>
#include <string>
int main()
{

    //std::stringstream ss;
    //ss << "ss\n";
    //char ch[] = "pp";
    //ss.put('c');
    //std::cout << ss.str();
    //std::string s;
    //ss >> s;


    //string与各种内置类型数据之间的转换
    //int转string
    std::stringstream ss0;
    int int1 = 100;
    ss0 << int1;
    std::string str1 = ss0.str();
    //string转int
    std::stringstream ss1;
    std::string str2 = "666";
    int int2;
    ss1 << str2;
    ss1 >> int2;
    ss1.clear();
    std::string str3 = "12.34";
    float float1;
    ss1 << str3;
    ss1 >> float1;
    std::cout << float1;

    std::string s;
    std::getline(std::cin, s);



    //std::stringstream ss2;
    //char ch[] = "char";
    //ss1 << ch << std::endl;
    //ss1.put('a');
    //ss1.put('b');
    //ss1 << "def" << std::endl;

    //auto r1 = ss1.str();  //r1类型为std::string
    //std::cout << r1;




    system("pause");
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>
#include <sstream>
#include <string>

int main()
{
    float f2 = 123456789.0f;
    float f3 = -0.0011223344f;
    float f4 = 2.0f;

    std::cout << std::fixed << f2 << std::endl;
    std::cout << std::scientific << f2 << std::endl;
    std::cout << std::fixed << f3 << std::endl;
    std::cout << std::scientific << f3 << std::endl;
    std::cout << std::fixed << f4 << std::endl;
    std::cout << std::scientific << f4 << std::endl;

    std::cout << "--------\n";
    std::cout.precision(2); //精度为2，小数点后的数字个数 std::setprecision()也可以设置精度
    std::cout.fill(0);  //以0填充
    std::cout.setf(std::ios::right); //右对齐

    std::cout << std::fixed << f2 << std::endl;
    std::cout << std::scientific << f2 << std::endl;
    std::cout << std::fixed << f3 << std::endl;
    std::cout << std::scientific << f3 << std::endl;
    std::cout << std::fixed << f4 << std::endl;
    std::cout << std::scientific << f4 << std::endl;





    std::stringstream ss1;
    ss1 << "sss";
    auto ret1 = ss1.str();


    std::stringstream ss2;
    //清除默认精度
    //ss2.precision(std::numeric_limits<double>::digits10);
    float f1 = -0.123456789f;
    ss2 << f1;
    auto ret2 = ss2.str();  //"-0.123457"

    std::stringstream ss3;
    ss3.precision(2);
    ss3 << f1;
    auto ret3 = ss3.str();



    int test = 0;

}


#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

int main()
{
    // 流中存储 id + vector<float>
    {
        std::vector<float> vec1;
        vec1.emplace_back(1.1);
        vec1.emplace_back(2.2);
        vec1.emplace_back(3.3);

        std::stringstream ss1;

        double id = 5.5;
        ss1.write(reinterpret_cast<char*>(&id), sizeof(double)); //前8个字节写入id
        ss1.write(reinterpret_cast<char*>(vec1.data()), sizeof(float) * vec1.size()); //接着id写入实际的vector<float>数据

        char buf1[10]{ 0 };
        char buf2[50]{ 0 };
        ss1.read(buf1, sizeof(double)); // 读取id
        ss1.read(buf2, sizeof(float) * vec1.size()); // 读取vector<float>数据
        auto ret1 = reinterpret_cast<double*>(buf1);
        auto ret2 = reinterpret_cast<float*>(buf2);
    }

    // 流中存储 vector<vector<int>>
    {
        std::vector<std::vector<int>> vec2;
        vec2.emplace_back(std::vector<int>{ 1, 2, 3 });
        vec2.emplace_back(std::vector<int>{ 4, 5, 6 });
        vec2.emplace_back(std::vector<int>{ 7, 8, 9 });
        vec2.emplace_back(std::vector<int>{ 10, 11, 12 });

        // 复合vector返回的是内层的指针
        std::vector<int>* data = vec2.data();

        std::stringstream ss2;
        int size = 0;
        for (auto elem : vec2)
        {
            // 遍历将所有vector<int>写入流
            size += elem.size();
            ss2.write(reinterpret_cast<char*>(elem.data()), sizeof(int) * elem.size());
        }

        char buf[100]{ 0 };
        ss2.read(buf, sizeof(int) * (size));
        auto ret1 = reinterpret_cast<int*>(buf);
    }

    // 流中存储bool int复合数据
    {
        bool b1 = true;
        bool b[4] = { true,false,false,true };
        int arr[4] = { 5,6,7,8 };
        std::stringstream ss;
        ss.write(reinterpret_cast<char*>(&b1), sizeof(bool));
        ss.write(reinterpret_cast<char*>(arr), sizeof(int) * 4);
        ss.write(reinterpret_cast<char*>(b), sizeof(bool) * 4);

        char buf1[10]{ 0 };
        char buf2[50]{ 0 };
        char buf3[10]{ 0 };

        ss.read(buf1, 1);
        ss.read(buf2, sizeof(int) * 4);
        ss.read(buf3, sizeof(bool) * 4);

        auto ret1 = reinterpret_cast<bool*>(buf1);
        auto ret2 = reinterpret_cast<int*>(buf2);
        auto ret3 = reinterpret_cast<bool*>(buf3);
    }

    // 流中写入空char*
    {
        std::vector<float> vec;
        std::stringstream ss;
        ss.write(reinterpret_cast<char*>(vec.data()), vec.size() * sizeof(float));
    }
}

#endif // TEST3

#ifdef TEST4

#define _CRT_SECURE_NO_WARNINGS

#include <cstring>
#include <iostream>

int main()
{
    //void *memcpy(void *str1, const void *str2, size_t n) 从存储区 str2 复制 n 个字节到存储区 str1。

    {
        const char* char6 = "1234567890abc";
        const int buf_size = 5;
        int length1 = strlen(char6);
        char* char7 = new char[length1]();
        int length2 = strlen(char7);

        while (length1 > 0)
        {
            char* char8 = new char[buf_size + 1](); // 注意此处申请的大小
            //int sendSize = buf_size;
            if (length1 < 5)
            {
                //sendSize = length1;
                length1 = 0;
            }
            else
            {
                length1 -= buf_size;
            }

            memcpy(char8, char6, 5);
            strcat(char7, char8);
            char6 += 5;
        }
    }

    {
        const char* buf1 = "123456789";
        auto len1 = strlen(buf1);

        char buf2[20]{ 0 };
        memcpy(buf2, buf1, 15);
        buf1 += 5;
        auto len2 = strlen(buf2);

        char buf3[10]{ 0 };
        memcpy(buf3, buf1, 15);   // 不会报错
        auto len3 = strlen(buf3);
    }

    {
        const char* buf1 = "123abc";
        char* buf2 = new char[1]();
        auto s1 = strlen(buf2);
        //char* buf2 = nullptr;
        strcat(buf2, buf1);
        const char* buf3 = "xyz";
        strcat(buf2, buf3);
        strcat(buf2, buf1);
        auto s3 = strlen(buf2);

        int i = 0;
    }

    {
        int a = 99;
        int b = 88;
        int c = 77;

        int buf[]{ 99,88,77 };
        char* buf1 = reinterpret_cast<char*>(buf);
        char* buf5 = new char[100]();
        const char* buf6 = "abc";
        strcat(buf5, buf1);
        strcat(buf5, buf6);

        //char* buf1 = new char[100]();
        //strcat(buf1, reinterpret_cast<char*>(&a));
        //strcat(buf1, reinterpret_cast<char*>(&b));
        //strcat(buf1, reinterpret_cast<char*>(&c));

        //strncat(buf1, reinterpret_cast<char*>(&a), 4);
        //strncat(buf1, reinterpret_cast<char*>(&b), 4);
        //strncat(buf1, reinterpret_cast<char*>(&c), 4);

        auto buf2 = reinterpret_cast<int*>(buf1);

        char* buf3 = new char[13]();
        memcpy(buf3, buf5, 12);
        auto buf4 = reinterpret_cast<int*>(buf3);

        int t = 0;
    }


    //char char1[] = "123456";
    //char char2[] = "abcdefgh";
    //char buffer[1024] = { 0 };
    //auto ret5 = strncpy_s(buffer, char1, 3);

    //const char* char3 = "123456abcdef";
    //char char4[100] = { 0 };

    //const char* char5 = "1234567890";

    ////char* strncpy(char* dest, const char* src, size_t n)
    ////    把 src 所指向的字符串复制到 dest，最多复制 n 个字符。
    //strncpy_s(char4, char5, 5);
    //char5 += 5;

    ////char* strncat(char* dest, const char* src, size_t n)
    ////    把 src 所指向的字符串追加到 dest 所指向的字符串的结尾，直到 n 字符长度为止。
    //char dest[1024]{ 0 };
    //strcat_s(dest, char1);
    //strcat_s(dest, char2);

    //auto ret6 = char3[0];
    //auto ret7 = char3[3];
    //auto len3 = strlen(char3);
    //auto len4 = sizeof(char3);
    //auto len5 = sizeof(char4);



    //auto ret1 = char1;
    //auto ret2 = char2;
    //auto ret3 = char1[0];
    //auto ret4 = char1[1];

    //auto len1 = strlen(char1);
    //auto len2 = strlen(char2);

    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <iostream>
#include <algorithm>
#include <iterator>

int main()
{
    const char* c1 = "abcd1234efgh5678";
    char* c2 = new char[16]();
    char c3[] = "abcd1234";
    char c4[10] = { 0 };
    // 只有c3和c4这种数组可以用std::begin和std::end，c1和c2不行

    std::fill(std::begin(c4), std::end(c4), 'a');
    std::copy(std::begin(c3), std::end(c3), c4);

}

#endif // TEST5

#ifdef TEST6

#include <sstream>
#include <fstream>
#include <iostream>

int main()
{
    // read write
    // 写int数组到sstream，从sstream读int数组
    {
        int c1[] = { 1,2,3,4,5,6 };
        std::stringstream ss;
        ss.write((char*)(c1), 100);
        char* buf = new char[100]();
        ss.read(buf, 100);
        auto ret = (int*)(buf);

    }

    // seekg:seek get 设置流读指针的位置 适用于 istream stream等
    // tellg:tell get 获取流读指针的位置 适用于 istream stream等

    // seekp:seek put 设置流写指针的位置 适用于 ostream stream等
    // tellp:tell put 获取流写指针的位置 适用于 ostream stream等
    {
        // seekp seekg
        // 第一个实参是一个 long 类型的整数，表示文件中的偏移量
        // 第二个实参称为模式标志，它指定从哪里计算偏移量，第一个参数为正数就向后增加，负数向前减少

        std::ostringstream oss;
        oss << "12345abcde";
        long pos1 = oss.tellp();       // 获取输出流的写指针位置
        oss.seekp(0L, std::ios::beg);  // 将输出流的写指针设置到起始
        long pos2 = oss.tellp();
        oss.seekp(0L, std::ios::end);  // 将输出流的写指针设置到末尾
        long pos3 = oss.tellp();       // 相当于求输出流的长度

        char buffer[11]{ 0 };
        std::string str1, str2, str3;
        std::istringstream iss("abcde12345");
        long pos4 = iss.tellg(); // 0
        iss >> str1; // "abcde12345"
        iss.seekg(3, std::ios::beg);
        long pos5 = iss.tellg(); // 3
        iss >> str2; // "de12345"
        iss.seekg(-7, std::ios::end);
        long pos6 = iss.tellg(); // 3
        iss >> str3; // "de12345"
        long pos7 = iss.tellg(); // -1
    }

    // getline 输入流才可以使用，具体使用可以看 01_01_File
    {
        std::ostringstream oss;
        std::istringstream iss("abcde\n12345\n---");

        char buf1[50]{ 0 };
        iss.getline(buf1, 20); // 只能获取第一个换行符之前的内容

        std::string str;
        while (std::getline(iss, str)) // 从上次读的地方继续读，即不会读取abede
        {
            std::cout << str << '\n';
        }
    }

    // peek 输入流才可以使用
    // 此函数将返回输入流文件的下一个字符，但它不移动内置指针，get()会移动指针
    {
        std::ostringstream oss;
        std::istringstream iss("abcde\n12345\n---");
        auto c1 = (char)iss.peek(); // 'a'
        iss.seekg(3, std::ios::beg);
        auto c2 = (char)iss.peek(); // 'd'
    }

    // ignore 输入流才可以使用
    // 使用于读取文件之时略过一定数量的字符，seekg也可以，不过ignore可以指定一个界限规则
    {
        std::ostringstream oss;
        std::istringstream iss("abcde\n12345\n---");
        iss.ignore(1, 'c');
        long pos1 = iss.tellg(); // 1
        auto ret1 = iss.str();   // "abcde\n12345\n---"
        iss.ignore(3, 'z');
        long pos2 = iss.tellg(); // 4
        auto ret2 = iss.str();   // "abcde\n12345\n---"
        iss.ignore(2, '\n');
        long pos3 = iss.tellg(); // 6
        auto ret3 = iss.str();   // "abcde\n12345\n---"

    }

    // get
    {
        const char* c1 = "1234567890abcdef";
        std::stringstream ss;
        ss << c1;

        char* c4 = new char[5];   //"1234"
        ss.get(c4, 5);
        char* c5 = new char[5];   //"567"
        ss.get(c5, 4);
        char* c6 = new char[5](); //"890a"
        ss.get(c6, 5);

        delete[] c4; c4 = nullptr;
        delete[] c5; c5 = nullptr;
        delete[] c6; c6 = nullptr;
    }

    // read函数读取字节数
    {
        const char* c1 = "1234567890abcdef";
        std::stringstream ss;
        ss << c1;
        char* c2 = new char[5](); // "1234"
        ss.read(c2, 4);
        char* c3 = new char[5];   // "56789   "   error
        ss.read(c3, 5);
        char* c4 = new char[5](); // "0abcd   "   error
        ss.read(c4, 5);

        delete[] c2; c2 = nullptr;
        delete[] c3; c3 = nullptr;
        delete[] c4; c3 = nullptr;
    }

    // 使用sstream合并char*
    {
        const char* c1 = "abcd";
        const char* c2 = "1234";
        std::stringstream ss;
        ss << c1;
        ss << c2;
        auto ret1 = ss.str();
        auto ret2 = ret1.c_str();
    }

    // 将文件读到stringstream
    {
        std::ifstream ifs("test.txt"/*, std::ifstream::binary*/);
        std::stringstream ss;
        ss << ifs.rdbuf();
        ifs.close();

        //auto str = ss.str();
        //auto len = str.length();
        //auto ret1 = str[3830000];
        //auto ret2 = str[3830001];
        //auto ret3 = str[3830002];
    }

    // 将文件读到stringstream
    {
        std::ifstream ifs("test.bmp", std::ifstream::binary);
        ifs.seekg(0, ifs.end);
        auto length = ifs.tellg();
        ifs.seekg(0, ifs.beg);
        char* buffer = new char[length]();
        ifs.read(buffer, length);
        std::stringstream ss(buffer);  // 缓冲区会被复制，内存使用加倍
        auto ret = ss.str();
        ifs.close();
    }

    // 读取文件再分次写入文件
    {
        std::ifstream ifs("test.bmp", std::ifstream::binary);
        std::stringstream ss;
        ss << ifs.rdbuf();
        ifs.seekg(0, std::ios::end);
        int length = ifs.tellg();
        ifs.close();
        std::stringstream ss2;
        while (length > 0)
        {
            if (length > 1024)
            {
                char* buffer = new char[1025]();
                ss.read(buffer, 1024);
                //ss2 << buffer;
                // 用 <<写的文件有问题，暂时不知道为啥
                ss2.write(buffer, 1024);
                length -= 1024;
            }
            else
            {
                char* buffer = new char[length + 1]();
                ss.read(buffer, length);
                //ss2 << buffer;
                ss2.write(buffer, length);
                length = 0;
            }
        }

        auto ret1 = ss2.str();
        auto len = ret1.size();
        auto ret2 = ret1.c_str();
        std::ofstream ofs("out.bmp", std::ofstream::binary);
        ofs.write(ret2, len);
        ofs.close();
    }

    {
        const int bufferSize = 1024;
        char buffer[bufferSize] = { 0 };
        int readLen = 0;
        int haveRead = 0;

        std::ifstream ifs("test.bmp", std::ios::binary);
        if (ifs)
        {
            while (!ifs.eof())
            {
                ifs.read(buffer, bufferSize);
                // gcount返回实际读取到的字符数，最大读取bufferSize个字符，但实际流中可能没有这么多字符
                readLen = ifs.gcount();
                //std::cout << readLen << std::endl;
                haveRead += readLen;
            }
        }
        std::cout << "have read:" << haveRead << std::endl;
    }

    system("pause");
    return 0;
}

#endif // TEST6

#ifdef TEST7

#include <iostream>
#include <fstream>
#include <sstream>

int main()
{
    std::fstream fs;

    // fseeko64
}

#endif // TEST7
