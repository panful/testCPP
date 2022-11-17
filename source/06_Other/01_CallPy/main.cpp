
/*
* 1.
*/

#define TEST3

#ifdef TEST1

#include <iostream>
#include <Python.h>
#include<string>
using namespace std;
int main()
{
    Py_Initialize();//使用python之前，要调用Py_Initialize();这个函数进行初始化
    if (!Py_IsInitialized())
    {
        printf("初始化失败！");
        return 0;
    }

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./PyFiles')");//这一步很重要，修改Python路径


    PyObject* pModule = NULL;//声明变量
    PyObject* pFunc1 = NULL;// 声明变量
    PyObject* pFunc2 = NULL;// 声明变量
    PyObject* pFunc3 = NULL;// 声明变量
    pModule = PyImport_ImportModule("mytest");//这里是要调用的文件名hello.py
    if (pModule == NULL)
    {
        cout << "没找到" << endl;

        Py_Finalize(); // 与初始化对应
        return -1;
    }
    //1
    //pFunc1 = PyObject_GetAttrString(pModule, "zip_file");//这里是要调用的函数名
    //PyEval_CallObject(pFunc1, NULL);//调用无参数无返回值的python函数

    //2
    pFunc2 = PyObject_GetAttrString(pModule, "zip_file");//这里是要调用的函数名
    string readpath = R"(C:\Users\admin\Desktop\TestData)";
    string writepath = R"(C:\Users\admin\Desktop\TestData.zip)";
    PyObject* args = Py_BuildValue("ss", readpath.c_str(), writepath.c_str());//给python函数参数赋值

    PyObject_CallObject(pFunc2, args);//调用函数
    //3
    pFunc3 = PyObject_GetAttrString(pModule, "_getValue");//这里是要调用的函数名
    PyObject* args2 = Py_BuildValue("ii", 28, 103);//给python函数参数赋值

    PyObject* pRet = PyObject_CallObject(pFunc3, args2);//调用函数
    int res = 0;
    PyArg_Parse(pRet, "i", &res);//转换返回类型

    cout << "res:" << res << endl;//输出结果

    Py_Finalize(); // 与初始化对应

    return 0;
}
#endif // TEST1

#ifdef TEST2

#include <iostream> 
#include <Python.h>
using namespace std;

int main()
{
    Py_Initialize(); // 初始化，这是必须的，用来初始化python所需的环境
    if (!Py_IsInitialized())
        return -1;
    // 导入模块
    PyObject* pModule = NULL;
    PyRun_SimpleString("import sys");
    //PyRun_SimpleString("sys.path.append(\"C:\\Program Files\\Python39");
    PyRun_SimpleString("sys.path.append('./PyFiles')");
    //PyRun_SimpleString("print(sys.path)");

    auto pName = PyUnicode_FromString("myclass");
    pModule = PyImport_Import(pName);
    //pModule = PyImport_ImportModule("myclass");
    if (!pModule) {
        printf("Cant open python file!/n");
        return -1;
    }
    // 模块的字典列表
    PyObject* pDict = PyModule_GetDict(pModule);
    if (!pDict)
    {
        printf("Cant find dictionary./n");
        return -1;

        // 演示函数调用
        cout << "calling python function..." << endl;
        PyObject* pFunHi = PyDict_GetItemString(pDict, "hi_function");
        PyObject_CallFunction(pFunHi, NULL, NULL);
        Py_DECREF(pFunHi);

        //演示类调用
        //cout << "calling python class..." << endl;
        //// 演示构造一个Python对象，并调用Class的方法
        //// 获取hi_class类
        //PyObject* phi_class = PyDict_GetItemString(pDict, "hi_class");
        //if (!phi_class) {
        //    printf("Cant find phi_class class./n");
        //    return -1;
        //}
        ////构造hi_class的实例
        //PyObject* pInstance_hi_class = PyInstance_New(phi_class, NULL, NULL);
        ////如果python类中有初始化参数，那么创建实例的时候需要将初始化参数以元组形式传递进去（亲测可用）PyObject* pInstance_hi_class = PyInstance_New(phi_class , PyObject*类型的元组（需要在C++里创建python元组）, NULL);
        ////C++中创建并初始化python元组示例如下两行：
        ////PyObject *pArgs3 = PyTuple_New(1);
        ////PyTuple_SetItem(pArgs3, 0, Py_BuildValue("i", 3));
        //if (!pInstance_hi_class) 
        //{
        //    printf("Cant create instance./n");
        //    return -1;
        //}
        ////调用hi_class类实例pInstance_hi_class里面的方法
        //PyObject_CallMethod(phi_class, "sayHi", "O", pInstance_hi_class);
        //释放
        //Py_DECREF(phi_class);
        //Py_DECREF(pInstance_hi_class);
        Py_DECREF(pModule);
        Py_Finalize(); // 与初始化对应

    }

    return 0;
}
#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <Python.h>

int main()
{
    Py_Initialize();   //初始化python环境
    if (!Py_IsInitialized)
    {
        std::cout << "Python解释器初始化失败" << std::endl;
        return -1;
    }
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./PyFiles')"); // 设置*.py文件所在路径
    PyObject* pModule = PyImport_ImportModule("myclass");  //导入模块
    if (!pModule)
    {
        std::cout << "Python get module failed." << std::endl;
        return -1;
    }
    std::cout << "Python get module succeed." << std::endl;

    // 模块的字典列表
    PyObject* pDict = PyModule_GetDict(pModule);
    if (!pDict)
    {
        printf("Cant find dictionary./n");
        return -1;
    }
    // 演示函数调用
    std::cout << "calling python function..." << std::endl;
    std::cout << "--------------------------\n";

    PyObject* pFunHi1 = PyDict_GetItemString(pDict, "hello");
    PyObject_CallFunction(pFunHi1, NULL, NULL);
    PyObject* pFunHi2 = PyDict_GetItemString(pDict, "world");
    PyObject_CallFunction(pFunHi2, NULL, NULL);

    //Py_DECREF(pFunHi);

    //PyObject* pFunc = nullptr;
    //pFunc = PyObject_GetAttrString(pModule, "Hello");  //Hello是Python文件里的函数
    //PyEval_CallObject(pFunc, NULL);
    //PyObject* pv = PyObject_GetAttrString(pModule, "_add");
    //if (!pv || !PyCallable_Check(pv))
    //{
    //	std::cout << "Can't find function(_add)" << std::endl;
    //}
    //std::cout << "Get function succeed." << std::endl;

    ////初始化要传入的参数，args配置成传入两个参数的模式
    //PyObject* args = PyTuple_New(2);
    ////将Long型数据转换成Python可接受的数据
    //PyObject* arg1 = PyLong_FromLong(4);
    //PyObject* arg2 = PyLong_FromLong(2);

    ////将arg1配置为arg带入的第一个参数
    //PyTuple_SetItem(args, 0, arg1);
    ////将arg2配置为arg带入的第二个参数
    //PyTuple_SetItem(args, 1, arg2);

    ////传入参数调用函数，并获取返回值
    //PyObject* pRet = PyObject_CallObject(pv, args);
    //if (pRet)
    //{
    //	//将返回值转换成Long型
    //	long result = PyLong_AsLong(pRet);
    //	std::cout << "result:" << result << std::endl;
    //}
    Py_Finalize();

    return 0;
}

#endif // TEST3



