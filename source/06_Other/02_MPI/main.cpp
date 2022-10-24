/*
* 1.测试MPI
* 2.MPI_Send MPI_Recv
* 3.MPI是多进程不是多线程
* 4.MPI_Send MPI_Recv 多进程计算99!
* 5.MPI_Reduce
*/


#define TEST4

#ifdef TEST1

#include <iostream>
#include <mpi.h>

// 控制台输入:mpiexec -n 8 .\06_02_MPI.exe运行程序

int main(int argc, char* argv[])
{
    int myid, numprocs, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Get_processor_name(processor_name, &namelen);
    if (myid == 0)
        std::cout << "number of processes: " << numprocs << '\n';
    std::cout << processor_name << "Hello world from process " << myid << '\n';
    MPI_Finalize();

    return 0;
}

#endif // TEST1

#ifdef TEST2

// https://cloud.tencent.com/developer/article/1583184

#include <stdio.h>
#include <string.h>
#include "mpi.h"

void main(int argc, char* argv[])
{
    int numprocs, myid, source;
    MPI_Status status;
    char message[100];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    printf("size:%d\trank:%d\n", numprocs, myid);

    if (myid != 0) {  //非0号进程发送消息
        strcpy(message, "Hello World!");
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, 0, 99,
            MPI_COMM_WORLD);
    }
    else {   // myid == 0，即0号进程接收消息
        for (source = 1; source < numprocs; source++) {
            MPI_Recv(message, 100, MPI_CHAR, source, 99,
                MPI_COMM_WORLD, &status);
            printf("接收到第%d号进程发送的消息：%s\n", source, message);
        }
    }
    MPI_Finalize();
}

#endif // TEST2

#ifdef TES3

#include <chrono>
#include <thread>
#include <iostream>

#include "mpi.h"

int main(int argc, char** argv)
{
    auto start = std::chrono::high_resolution_clock::now();

    MPI_Init(&argc, &argv);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    MPI_Finalize();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << '\n';

    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <chrono>
#include <thread>
#include <iostream>

#include "mpi.h"

// 计算 99! 即从0累加到99 = 4950
// mpi进程个数只能是 100能被size-1整除

int main(int argc, char** argv)
{
    //auto start = std::chrono::high_resolution_clock::now();

    int id = 0, size = 0;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //std::this_thread::sleep_for(std::chrono::seconds(2));

    if (size < 2 || 100 % (size - 1) != 0 || size - 1 > 100 )
    {
        MPI_Finalize();
        // 如果mpi启动的进程个数不符合，直接退出
        std::cout << "error of the process size\n";
        return 0;
    }

    if (id != 0)
    {
        // 非0号进程发送消息
        std::cout << 100 / (size-1) * (id - 1) << '\t' << 100 / (size-1) * id << '\n';
        int sum = 0;
        for (int i = 100 / (size-1) * (id - 1); i < 100 / (size-1) * id; i++)
        {
            sum += i;
        }

        // 0 要发送的数据
        // 1 发送数据的长度，不是字节数，例如发送sum只有一个int值，所以是1；int arr[2] = {1,2}}发送arr就应该为2
        // 2 发送的数据类型
        // 3 接收数据的进程号，本测试代码0号进程接收
        // 4 发送密钥，只有发送和接收的密钥一致才能成功接收
        // 5 通讯域，表示向那个组发送消息
        MPI_Send(&sum, 1, MPI_INT, 0, 99, MPI_COMM_WORLD);
    }
    else
    {
        // myid == 0，即0号进程接收消息
        int sum = 0;
        for (int source = 1; source < size; source++) 
        {
            int n = 0;
            // 0-5和MPI_Send一致对应
            // 6消息状态
            MPI_Recv(&n, 1, MPI_INT, source, 99, MPI_COMM_WORLD, &status);
            sum += n;
        }
        std::cout << sum << '\n';
    }

    MPI_Finalize();

    //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << '\n';

    return 0;
}

#endif // TEST4

#ifdef TEST5

// https://zhuanlan.zhihu.com/p/399150417

#include "mpi.h"
#include <stdio.h>
double f(double);
double f(double x)
{
    return (4.0 / (1.0 + x * x));
}
int main(int argc, char* argv[])
{
    int myid, numprocs;
    int n, i;
    double mypi, pi;
    double h, sum, x;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    printf("Process %d of %d.\n", myid, numprocs);
    n = 100;
    h = 1.0 / (double)n;
    sum = 0.0;
    for (i = myid + 1; i <= n; i += numprocs)
    {
        x = h * ((double)i - 0.5);
        sum += f(x);
    }
    mypi = h * sum;
    MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (myid == 0)
    {
        printf("The result is %.10f.\n", pi);
    }
    MPI_Finalize();

    return 0;
}
#endif // TEST5
