
/*
* 1 排序
* 2 string类实现
* 3 二叉树
* 4 获取第二大的数
* 5 求最大公约数
* 6 深广度优先遍历二叉树 DFS BFS
* 7 无序数组三个数最大乘积
* 8 字符串大整数相加
* 9 容器中的最大最小值 std::max_element,std::min_element
* 10动态规划
*/

#define TEST1

#ifdef TEST1
#include<iostream>
using namespace std;

void BubbleSort(int r[], int n) //冒泡排序
{
    int temp;
    int i = 0;
    while (i < n - 1)
    {
        for (int k = 0; k < n; k++)
        {
            if (r[k] < r[k + 1])
            {
                temp = r[k];
                r[k] = r[k + 1];
                r[k + 1] = temp;
            }
        }
        i++;
    }
}

void bubble(int a[], int n)  //冒泡 
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (a[i] < a[j])
            {
                swap(a[i], a[j]);
            }
        }
    }
}

void bubble1(int a[], int n) //鸡尾酒排序 
{
    int start = 0, end = n - 1;
    while (end > start)
    {
        for (int i = start; i < end; i++)  //正向循环 
        {
            if (a[i] > a[i + 1])
            {
                swap(a[i], a[i + 1]);
            }
        }
        end--;
        if (end == start) break;

        for (int i = end; i > start; i--) //逆向循环 
        {
            if (a[i] < a[i - 1])
            {
                swap(a[i - 1], a[i]);
            }
        }
        start++;
        if (end == start) break;
    }
}

int partition(int a[], int first, int end)
{
    int i = first, j = end;
    while (i < j)
    {
        while (i < j && a[i] <= a[j]) j--;
        if (i < j)
        {
            swap(a[i], a[j]);
            i++;
        }
        while (i < j && a[i] <= a[j]) i++;
        if (i < j)
        {
            swap(a[i], a[j]);
            j--;
        }
    }
    return i;
}

void quick(int a[], int first, int end)
{
    if (first < end)
    {
        int pivot = partition(a, first, end);
        quick(a, first, pivot - 1);
        quick(a, pivot + 1, end);
    }
}

void insert(int a[], int n)
{

    for (int i = 1; i < n; i++)
    {
        if (a[i] < a[i - 1])
        {
            int j = 0;
            int temp = a[i];
            for (j = i - 1; j >= 0 && (temp < a[j]); j--)
            {
                a[j + 1] = a[j];
            }
            a[j + 1] = temp;
        }
    }
}

void sift(int a[], int k, int m) //堆排序 帅选算法 
{
    int i = k, j = 2 * i;  //0号单元用来做交换的暂存单元 
    while (j <= m)
    {
        if (a[j] > a[j + 1]) j++;
        if (a[i] > a[j]) break;
        else
        {
            swap(a[i], a[j]);
            i = j; j = 2 * i;
        }
    }
}

void heapsort(int a[], int n)  //堆排序 
{
    for (int i = n / 2; i >= 1; i--)
    {
        sift(a, i, n);
    }
    for (int i = 1; i < n; i++)
    {
        swap(a[1], a[n - i + 1]);
        sift(a, 1, n - 1);
    }
}


int main()
{
    int arr[6] = { 9,8,5,6,2,7 };
    insert(arr, 6);
    for (int i = 0; i < 6; i++)
    {
        cout << arr[i];
    }

    int a[8] = { 8,2,6,9,5,0,12,3 };
    BubbleSort(a, 8);
    for (int j = 0; j < 8; j++) {
        cout << a[j] << " ";
    }

    return 0;
}

#endif // TEST1

#ifdef TEST2
/*strcmp:string compare strcat:string catenate strcpy:string copy
strlen:string length strlwr:string lowercase strupr:string upercase*/

#include <iostream>
#include <cstring>
using namespace std;

class String
{
public:
    String(const char* str = NULL);//通用构造函数,String("abc")
    String(const String& str);//拷贝构造
    ~String();

    String& operator=(const String& str);//赋值运算符。返回引用
    String operator+(const String& str) const;
    String& operator+=(const String& str);//+=操作符。返回引用
    char& operator[](int n) const;//下标操作符。返回引用
    bool operator==(const String& str) const;

    int size() const;//字符串实际大小，不包括结束符
    const char* c_str() const;//将string转为char *

private:
    char* data;
    int length;
};

String::String(const char* str)//通用构造
{
    if (!str)
    {//为空。String a()
        length = 0;
        data = new char[1];
        *data = '\0';
    }
    else
    {
        length = strlen(str);
        data = new char[length + 1];
        strcpy(data, str);//会拷贝源的结束符
    }
}


String::String(const String& str)//拷贝构造，深拷贝
{
    length = str.size();
    data = new char[length + 1];
    strcpy(data, str.c_str());
}

String::~String()
{
    delete[] data;
    length = 0;
}

String& String::operator=(const String& str)//赋值操作符4步
{
    if (this == &str) return *this;//1 自我赋值，返回自身引用

    delete[] data;//2 删除原有数据

    length = str.size();//3 深拷贝
    data = new char[length + 1];
    strcpy(data, str.c_str());

    return *this;//4 返回自身引用
}
String String::operator+(const String& str) const//+操作符3步
{//新建对象包括新空间，拷贝两个数据，返回新空间
    String newString;
    newString.length = length + str.size();
    newString.data = new char[newString.length + 1];
    strcpy(newString.data, data);
    strcat(newString.data, str.data);
    return newString;
}

String& String::operator+=(const String& str)//+=操作符5步
{//重分配新空间，拷贝两个数据，删除自己原空间，赋值为新空间，返回引用
    length += str.size();//成员length是实际长度
    char* newdata = new char[length + 1];
    strcpy(newdata, data);
    strcat(newdata, str.c_str());
    delete[] data;
    data = newdata;
    return *this;
}

char& String::operator[](int n) const
{//下标操作符，返回引用
    if (n >= length) return data[length - 1];//如果越界，返回最后一个字符
    else return data[n];
}

bool String::operator==(const String& str) const
{
    if (length != str.size()) return false;
    return strcmp(data, str.c_str()) ? false : true;
}

int String::size() const
{
    return length;
}

const char* String::c_str() const
{
    return data;
}

int main()
{
    char a[] = "Hello", b[] = "World!";
    String s1(a), s2(b);
    cout << s1.c_str() << endl;
    cout << s2.c_str() << endl;
    s1 += s2;
    cout << s1.c_str() << endl;
    s1 = s2;
    cout << s1.c_str() << endl;
    cout << (s1 + s2).c_str() << endl;
    cout << s1.size() << endl;
    cout << s1[1] << endl;

    if (s1 == s2)
        cout << "相等" << endl;
}

#endif // TEST2

#ifdef TEST3
#include<iostream> 
#include<stack>
using namespace std;

struct BiTree
{
    int NodeData = 0;
    BiTree* pLeft = nullptr;
    BiTree* pRight = nullptr;
};

void show(BiTree* pRoot)
{

    if (pRoot == nullptr)
    {
        return;
    }
    int front = 0, rear = 0;
    BiTree* q[8];
    BiTree* Q;
    q[rear++] = pRoot;  //[]优先级高于++ 
    while (front != rear)
    {
        Q = q[front++];
        if (Q->pLeft != nullptr) q[rear++] = Q->pLeft;
        if (Q->pRight != nullptr) q[rear++] = Q->pRight;
    }
    for (int i = 0; i < 8; i++)
    {
        cout << q[i]->NodeData;
    }

}

void InOrder(BiTree* pRoot)//中序遍历，先左再中最后右 
{
    if (pRoot == nullptr)
    {
        return;
    }
    else
    {
        InOrder(pRoot->pLeft);
        cout << pRoot->NodeData;
        InOrder(pRoot->pRight);
    }
}

void PreOrder(BiTree* pRoot) //前序遍历 
{
    if (pRoot == nullptr)
    {
        return;
    }
    else
    {
        cout << pRoot->NodeData;
        PreOrder(pRoot->pLeft);
        PreOrder(pRoot->pRight);
    }
}

void PostOrder(BiTree* pRoot)//后序遍历 
{
    if (pRoot == nullptr)
    {
        return;
    }
    else
    {
        PostOrder(pRoot->pLeft);
        PostOrder(pRoot->pRight);
        cout << pRoot->NodeData;
    }
}

int getNodeCount(BiTree* pRoot)  //获取结点个数 
{
    if (pRoot == nullptr)
    {
        return 0;
    }
    return getNodeCount(pRoot->pLeft) + getNodeCount(pRoot->pRight) + 1;
}

BiTree* revBiTree(BiTree* pRoot)  //翻转二叉树 (镜像二叉树） 
{
    if (pRoot == nullptr)
    {
        return nullptr;
    }
    BiTree* left = revBiTree(pRoot->pLeft);   //临时存储 
    BiTree* right = revBiTree(pRoot->pRight);

    pRoot->pLeft = right;//交换 
    pRoot->pRight = left;

    return pRoot;
}

int getLevelCount(BiTree* pRoot, int n)
{
    return 0;
}
int main()
{
    struct BiTree* pRoot;

    struct BiTree node1;
    struct BiTree node2;
    struct BiTree node3;
    struct BiTree node4;
    struct BiTree node5;
    struct BiTree node6;
    struct BiTree node7;
    struct BiTree node8;

    node1.NodeData = 1;
    node2.NodeData = 2;
    node3.NodeData = 3;
    node4.NodeData = 4;
    node5.NodeData = 5;
    node6.NodeData = 6;
    node7.NodeData = 7;
    node8.NodeData = 8;

    pRoot = &node1;
    node1.pLeft = &node2;
    node1.pRight = &node3;

    node2.pLeft = &node4;
    node2.pRight = &node5;

    node4.pLeft = &node6;
    node4.pRight = &node7;

    node5.pLeft = &node8;
    cout << "层序递归：";
    show(pRoot);
    cout << endl;
    cout << "中序递归：";
    InOrder(pRoot);    //中序递归
    cout << endl;
    //cout << "中序非递归：" << endl;
   // InOrder(pRoot);        //中序非递归

    cout << "先序递归：";
    PreOrder(pRoot); cout << endl;
    //  cout << "先序非递归：" << endl;
     // PreTravel(pRoot);        //先序非递归

    cout << "后序递归：";
    PostOrder(pRoot); cout << endl;
    //  cout << "后序非递归：" << endl;
     // PostTravel(pRoot);        //后序非递归
    revBiTree(pRoot);
    cout << "反转后：";
    show(pRoot);

    return 0;
}

#endif // TEST3

#ifdef TEST4
#include<iostream>

using namespace std;

void find(int a[], int len)
{
    int max = 0, second = 0;
    /*if(a[0]>a[1])
    {
        max=a[0];
        second=a[1];
    }
    else
    {
        max=a[1];
        second=a[0];
    }*/
    a[0] > a[1] ? max = a[0], second = a[1] : second = a[0], max = a[1];

    for (int i = 2; i < len; i++)
    {
        if (a[i] >= max)
        {
            second = max;        //先把原来的最大数给第二大数
            max = a[i];          //再刷新最大数
        }
        else
        {
            if (a[i] > second && a[i] < max)
                second = a[i];       //第二大的数
        }
    }
    cout << "The second number is " << second;
}

int main()
{
    int arr[6] = { -1,-5,-8,0,-7,-1 };
    find(arr, 6);
    return 0;
}

#endif // TEST4

#ifdef TEST5
#include<iostream>
using namespace std;
int main()
{
    int a, b, t, i;
    cin >> a >> b;
    t = 1;
    for (i = 2; i <= a && i <= b; i++)
    {
        while (a % i == 0 && b % i == 0)
        {
            t = t * i;
            a = a / i;
            b = b / i;
        }
    }
    cout << t;
    return 0;
}

#endif // TEST5

#ifdef TEST6
#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <Stack>
#include <Queue>
using namespace std;

typedef struct Node {
    char data;
    struct Node* lchild;
    struct Node* rchild;
} *Tree;
//Tree 是一个node指针的类型定义

int index = 0;  //全局索引变量
//二叉树构造器,按先序遍历顺序构造二叉树
//无左子树或右子树用'#'表示
void treeNodeConstructor(Tree& root, char data[]) {
    char e = data[index++];
    if (e == '#') {
        root = NULL;
    }
    else {
        root = (Node*)malloc(sizeof(Node));
        root->data = e;
        treeNodeConstructor(root->lchild, data);  //递归构建左子树
        treeNodeConstructor(root->rchild, data);  //递归构建右子树
    }
}
//深度优先遍历
void depthFirstSearch(Tree root) {
    stack<Node*> nodeStack;  //使用C++的STL标准模板库
    nodeStack.push(root);
    Node* node;
    while (!nodeStack.empty()) {
        node = nodeStack.top();
        cout << node->data;//遍历根结点
        nodeStack.pop();
        if (node->rchild) {
            nodeStack.push(node->rchild);  //先将右子树压栈
        }
        if (node->lchild) {
            nodeStack.push(node->lchild);  //再将左子树压栈
        }
    }
}

//广度优先遍历
void breadthFirstSearch(Tree root) {
    queue<Node*> nodeQueue;  //使用C++的STL标准模板库
    nodeQueue.push(root);
    Node* node;
    while (!nodeQueue.empty()) {
        node = nodeQueue.front();
        nodeQueue.pop();
        cout << node->data;//遍历根结点
        if (node->lchild) {
            nodeQueue.push(node->lchild);  //先将左子树入队
        }
        if (node->rchild) {
            nodeQueue.push(node->rchild);  //再将右子树入队
        }
    }
}

int main() {
    //上图所示的二叉树先序遍历序列,其中用'#'表示结点无左子树或无右子树
    char data[15] = { 'A', 'B', 'D', '#', '#', 'E', '#', '#', 'C', 'F','#', '#', 'G', '#', '#' };
    Tree tree;
    treeNodeConstructor(tree, data);
    printf("深度优先遍历二叉树结果: ");
    depthFirstSearch(tree);
    printf("\n\n广度优先遍历二叉树结果: ");
    breadthFirstSearch(tree);
    return 0;
}

#endif // TEST6

#ifdef TEST7
#include<iostream>
#include<algorithm>

using namespace std;
int fun(int r[], int n)
{
    sort(r, r + n);
    int a1 = r[0] * r[1];
    int a2 = r[n - 1] * r[n - 2];
    int result;
    if (a1 > a2 && r[n - 1] > 0)
        result = a1 * r[n - 1];
    else result = a2 * r[n - 3];
    return result;
}
int main()
{
    int n;
    cin >> n;
    int A[n];

    for (int i = 0; i < n; i++)
        cin >> A[i];
    cout << fun(A, n);
    return 0;
}

#endif // TEST7

#ifdef TEST8
#include <stdio.h>
#include <string.h>
#define MAXLEN 1000
char a1[MAXLEN];
char a2[MAXLEN];
static int v1[MAXLEN];
static int v2[MAXLEN];
static int v3[MAXLEN];
int i, j, n, L, z;
int main() {

    char* ppp = "aaa";
    scanf("%d", &n);
    for (j = 0; j < n; j++) {
        scanf("%s%s", a1, a2);

        L = strlen(a1);
        for (i = 0; i < L; i++) v1[i] = a1[L - 1 - i] - '0';

        L = strlen(a2);
        for (i = 0; i < L; i++) v2[i] = a2[L - 1 - i] - '0';

        for (i = 0; i < MAXLEN; i++) v3[i] = v1[i] + v2[i];

        for (i = 0; i < MAXLEN; i++) {
            if (v3[i] >= 10) {
                v3[i + 1] += v3[i] / 10;
                v3[i] = v3[i] % 10;
            }
        }

        printf("Case %d:\n", j + 1);
        printf("%s + %s = ", a1, a2);

        z = 0;
        for (i = MAXLEN - 1; i >= 0; i--) {
            if (z == 0) {
                if (v3[i] != 0) {
                    printf("%d", v3[i]);
                    z = 1;
                }
            }
            else {
                printf("%d", v3[i]);
            }
        }
        if (z == 0) printf("0");

        printf("\n");
    }
    return 0;
}
//Sample Input
//3
//0 0
//1 2
//112233445566778899 998877665544332211
//
//Sample Output
//Case 1:
//0 + 0 = 0
//Case 2:
//1 + 2 = 3
//Case 3:
//112233445566778899 + 998877665544332211 = 1111111111111111110


#endif // TEST8

#ifdef TEST9
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <algorithm>

int main()
{
    std::map<int, char> myMap{ {1,'a' }, {2, 'g'}, {7, 'b'}, {4, 'c'} };
    std::vector<int> myVec{ 1,5,6,4,2,8,7 };
    std::vector<int> myVec2{ 3,4,5,8,6 };
    std::list<int> myList{ 6,2,3,9,4,5 };

    auto maxMap = *std::max_element(myMap.begin(), myMap.end());
    auto maxVec = *std::max_element(myVec.begin(), myVec.end());
    auto minList = *std::min_element(myList.begin(), myList.end());
    auto minMax = std::minmax({ 2,3,5,4 }); //返回最大值和最小值构成一个pair
    auto minMax1 = std::minmax(myVec, myVec2); // 把 v1和v2按大小顺序放在pair的第一个位置和第二个位置，参数3默认按小到大
    auto minMax2 = std::minmax_element(myVec.begin(), myVec.end());//返回最大值和最小值构成一个pair

    int t = 0;
}

#endif // TEST9

#ifdef TEST10

#include <iostream>

int main()
{
    int f[105];
    int n = 5, cost = INFINITY;
    f[0] = 0;
    for (size_t i = 1; i < n; ++i)
    {
        if (i >= 1)
            std::min(f[i - 1] + 1, cost);
    }

}

#endif // TEST10
