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
