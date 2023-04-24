
/*
* 命令模式是一种数据驱动的设计模式。
*
*    最大的优点就是去耦合 更方便的去实现业务
*    缺点就是类特别多 需要实现的类很多，管理上面不太方便。
*
* https://juejin.cn/post/7046575907275800590
*/



//class Command
//{
//public:
//    virtual void Execute() = 0;
//
//public:
//    Command() = default;
//    virtual ~Command() = default;
//};

#define TEST1

#ifdef TEST1

// https://refactoringguru.cn/design-patterns/command/cpp/example

#include <iostream>
#include <string>

// 命令接口声明用于执行命令的方法。
class Command {
public:
    virtual ~Command() {
    }
    virtual void Execute() const = 0;
};

// 一些命令可以自己实现简单的操作。
class SimpleCommand : public Command {
private:
    std::string pay_load_;

public:
    explicit SimpleCommand(std::string pay_load) : pay_load_(pay_load) {
    }
    void Execute() const override {
        std::cout << "SimpleCommand: See, I can do simple things like printing (" << this->pay_load_ << ")\n";
    }
};

// 接收类包含一些重要的业务逻辑。他们知道如何执行与执行请求相关的各种操作。事实上，任何类都可以充当接收者
class Receiver {
public:
    void DoSomething(const std::string& a) {
        std::cout << "=== Receiver: Working on (" << a << ".)\n";
    }
    void DoSomethingElse(const std::string& b) {
        std::cout << "=== Receiver: Also working on (" << b << ".)\n";
    }
};

// 然而，一些命令可以将更复杂的操作委托给其他对象，称为“接收器”
class ComplexCommand : public Command {

private:
    Receiver* receiver_;

    // 上下文数据，启动接收方方法所需。
    std::string a_;
    std::string b_;

    //复杂命令可以通过构造函数接受一个或多个接收方对象以及任何上下文数据。
public:
    ComplexCommand(Receiver* receiver, std::string a, std::string b) : receiver_(receiver), a_(a), b_(b) {
    }

    // 命令可以委托给接收器的任何方法
    void Execute() const override {
        std::cout << "=== ComplexCommand\n";
        this->receiver_->DoSomething(this->a_);
        this->receiver_->DoSomethingElse(this->b_);
    }
};

// 调用程序与一个或多个命令相关联。它向命令发送请求。
class Invoker {
    /**
     * @var Command
     */
private:
    Command* on_start_;
    /**
     * @var Command
     */
    Command* on_finish_;
    /**
     * Initialize commands.
     */
public:
    ~Invoker() {
        delete on_start_;
        delete on_finish_;
    }

    void SetOnStart(Command* command) {
        this->on_start_ = command;
    }
    void SetOnFinish(Command* command) {
        this->on_finish_ = command;
    }

    // 调用程序不依赖于具体的命令或接收器类。调用程序通过执行命令间接地将请求传递给接收方。
    void DoSomethingImportant() {
        //std::cout << "Invoker: Does anybody want something done before I begin?\n";
        std::cout << "=== Invoker:\n";
        if (this->on_start_) {
            this->on_start_->Execute();
        }
        //std::cout << "Invoker: ...doing something really important...\n";
        //std::cout << "Invoker: Does anybody want something done after I finish?\n";
        if (this->on_finish_) {
            this->on_finish_->Execute();
        }
    }
};

// 客户端代码可以用任何命令参数化调用程序。
int main()
{
    Invoker* invoker = new Invoker;
    invoker->SetOnStart(new SimpleCommand("111111111"));
    Receiver* receiver = new Receiver;
    invoker->SetOnFinish(new ComplexCommand(receiver, "22222222", "22222222"));
    invoker->DoSomethingImportant();

    delete invoker;
    delete receiver;

    return 0;
}
#endif // TEST1


#ifdef TEST2
#include <list>
#include <iostream>

using namespace std;

//Receiver基类
class Receiver
{
public:
    virtual void Action() = 0;
    virtual void DeleteAction() = 0;
};

//厨师
class CookReceiver :public Receiver
{
public:
    void Action();
    void DeleteAction();
};

//店长
class ManagerReceiver :public Receiver
{
public:
    void Action();
    void DeleteAction();
};

//命令基类
class Command
{
public:
    virtual void Excute() = 0;
    virtual void SetReceiver(Receiver* r) = 0;
};

class AddCommand :public Command
{
private:
    list<Receiver*>m_list_receiver;

public:
    void Excute();
    void SetReceiver(Receiver* r);
};

class DelCommand :public Command
{
private:
    list<Receiver*>m_list_receiver;

public:
    void Excute();
    void SetReceiver(Receiver* r);
};

//执行命令类
class Invoker
{
private:
    list<Command*>m_commands;
public:
    void setCommand(Command* c);
    void Notify();
};


// CommandDemo.cpp : 定义控制台应用程序的入口点。

//具体实现
//Invoker类

void Invoker::setCommand(Command* c)
{
    m_commands.push_back(c);
}

void Invoker::Notify()
{
    //执行对应操作
    for (auto c_com = m_commands.begin(); c_com != m_commands.end(); c_com++)
    {
        (*c_com)->Excute();
    }
}

//Command类
void AddCommand::SetReceiver(Receiver* r)
{
    m_list_receiver.push_back(r);
}

void AddCommand::Excute()
{
    //执行对应操作
    for (auto c_com = m_list_receiver.begin(); c_com != m_list_receiver.end(); c_com++)
    {
        (*c_com)->Action();
    }
}


void DelCommand::SetReceiver(Receiver* r)
{
    m_list_receiver.push_back(r);
}

void DelCommand::Excute()
{
    //执行对应操作
    for (auto c_com = m_list_receiver.begin(); c_com != m_list_receiver.end(); c_com++)
    {
        (*c_com)->DeleteAction();
    }
}

//Receiver类
void CookReceiver::Action()
{
    cout << "厨师炒菜" << endl;
}

void CookReceiver::DeleteAction()
{
    cout << "厨师下班" << endl;
}

void ManagerReceiver::Action()
{
    cout << "店长接单" << endl;
}

void ManagerReceiver::DeleteAction()
{
    cout << "店长下班" << endl;
}

int main(int argc, char* argv[])
{
    //设置命令
    Command* del_c = new DelCommand(); //结束的命令
    Receiver* del_r = new CookReceiver(); //设置接收者为厨师
    del_c->SetReceiver(del_r); //这个命令是给厨师的

    Command* add_c = new AddCommand(); //添加的命令
    Receiver* add_r = new ManagerReceiver(); //设置接收者为店长
    add_c->SetReceiver(add_r); //这个命令是给厨师的

    //调用命令
    Invoker inv_i;
    inv_i.setCommand(del_c);//设置执行的命令
    inv_i.setCommand(add_c);
    //执行
    inv_i.Notify();
    system("pause");
    return 0;
}
#endif // TEST2


#ifdef TEST3

// https://refactoringguru.cn/design-patterns/command/cpp/example

#include <iostream>
#include <string>

// 命令接口声明用于执行命令的方法。
class Command {
public:
    virtual ~Command() {
    }
    virtual void Execute() const = 0;
};

// 一些命令可以自己实现简单的操作。
class SimpleCommand : public Command {
private:
    std::string pay_load_;

public:
    explicit SimpleCommand(std::string pay_load) : pay_load_(pay_load) {
    }
    void Execute() const override {
        std::cout << "SimpleCommand: See, I can do simple things like printing (" << this->pay_load_ << ")\n";
    }
};

// 然而，一些命令可以将更复杂的操作委托给其他对象，称为“接收器”
class ComplexCommand : public Command {

private:
    Receiver* receiver_;

    // 上下文数据，启动接收方方法所需。
    std::string a_;
    std::string b_;

    //复杂命令可以通过构造函数接受一个或多个接收方对象以及任何上下文数据。
public:
    ComplexCommand(Receiver* receiver, std::string a, std::string b) : receiver_(receiver), a_(a), b_(b) {
    }

    // 命令可以委托给接收器的任何方法
    void Execute() const override {
        std::cout << "=== ComplexCommand\n";
        this->receiver_->DoSomething(this->a_);
        this->receiver_->DoSomethingElse(this->b_);
    }
};

// viewerReceiver sceneReceiver objectReceiver 
class Receiver {
public:
    void DoSomething(const std::string& a) {
        std::cout << "=== Receiver: Working on (" << a << ".)\n";
    }
    void DoSomethingElse(const std::string& b) {
        std::cout << "=== Receiver: Also working on (" << b << ".)\n";
    }

    void SetColor(int color) {}   // Scene
    void SetBackgroundColor(int color) {} // Viewer
};



// 调用程序与一个或多个命令相关联。它向命令发送请求。
class Invoker {
    /**
     * @var Command
     */
private:
    Command* on_start_;
    /**
     * @var Command
     */
    Command* on_finish_;
    /**
     * Initialize commands.
     */
public:
    ~Invoker() {
        delete on_start_;
        delete on_finish_;
    }

    void SetOnStart(Command* command) {
        this->on_start_ = command;
    }
    void SetOnFinish(Command* command) {
        this->on_finish_ = command;
    }

    // 调用程序不依赖于具体的命令或接收器类。调用程序通过执行命令间接地将请求传递给接收方。
    void DoSomethingImportant() {
        //std::cout << "Invoker: Does anybody want something done before I begin?\n";
        std::cout << "=== Invoker:\n";
        if (this->on_start_) {
            this->on_start_->Execute();
        }
        //std::cout << "Invoker: ...doing something really important...\n";
        //std::cout << "Invoker: Does anybody want something done after I finish?\n";
        if (this->on_finish_) {
            this->on_finish_->Execute();
        }
    }
};

// 客户端代码可以用任何命令参数化调用程序。
int main()
{
    Invoker* invoker = new Invoker;
    invoker->SetOnStart(new SimpleCommand("111111111"));
    Receiver* receiver = new Receiver;
    invoker->SetOnFinish(new ComplexCommand(receiver, "22222222", "22222222"));
    invoker->DoSomethingImportant();

    delete invoker;
    delete receiver;

    return 0;
}
#endif // TEST3


#ifdef TEST4

#include <map>
#include <queue>
#include <string>
#include <iostream>

enum class Token
{
    Token0,
    Token1,
    Token2,
};

class Command
{
public:
    virtual void Execute() = 0;
};

//template <typename ...Args>
class SetInt :public Command
{
public:
    SetInt() {}
    SetInt(int c)
    {
        m_int = c;
    }

    // 具体的操作
    void Execute() { std::cout << " === set color\n"; }
private:
    int m_int{ 0 };
};

class SetStrings :public Command
{
public:
    SetStrings() {};
    SetStrings(std::string s1, std::string s2)
    {
        m_str1 = s1;
        m_str2 = s2;
    }
    void Execute() { std::cout << " === set strings\n"; }

private:
    std::string m_str1;
    std::string m_str2;
};

class SetVoid :public Command
{
public:
    SetVoid() {}
    void Execute() { std::cout << " === set void\n"; };
};

class Invoke
{
public:
    Invoke()
    {
    }
public:
    template<typename ...Args>
    void AddCommand(Token t, Args&&...args)
    {
        switch (t)
        {
        case Token::Token0:
        {
            m_command.emplace_back(new SetVoid(std::forward<Args>(args)...));
            break;
        }

        case Token::Token1:
        {
            m_command.emplace_back(new SetInt(std::forward<Args>(args)...));
            break;
        }

        case Token::Token2:
        {
            m_command.emplace_back(new SetStrings(std::forward<Args>(args)...));
            break;
        }

        default:
            break;
        }
    }

    std::vector<Command*> m_command;
};

int main()
{
    Invoke* invoke = new Invoke();

    invoke->AddCommand(Token::Token0);
    invoke->AddCommand(Token::Token1, 1);
    invoke->AddCommand(Token::Token2, "aaa", "bbb");

}

#endif // TEST4

#ifdef TEST5
#include <iostream>
template <typename ...Args>
class Test
{
public:
    Test(Args...args)
    {
        std::cout << "construct\n";
    }
};

int main()
{
    Test(1, 1);
}

#endif // TEST5

#ifdef TEST6

#include <iostream>
#include <string>
#include <queue>
#include <thread>

class Command
{
public:
    virtual ~Command() = default;
    virtual void Execute() const = 0;
};

class SetIntCommand : public Command {
private:
    int m_int;

public:
    explicit SetIntCommand(int a) :
        m_int(a)
    {
    }

    void Execute() const override
    {
        std::cout << "第一个命令\n";
    }
};

class SetVoidCommand : public Command
{
public:
    SetVoidCommand()
    {
    }

    void Execute() const override
    {
        std::cout << "第二个命令\n";
    }
};

class Invoker {
public:
    Invoker()
    {
        // 如果命令不能在子线程中执行，则需要添加一个Notify函数，显式地通知命令去执行
        std::thread t(&Invoker::run, this);
        t.detach();
    }

    void run()
    {
        while (true)
        {
            //std::cout << m_command.size() << " === \n";

            if (m_command.empty())
                continue;

            m_command.front()->Execute();
            m_command.pop();
        }
    }

    void AddCommand(Command* command)
    {
        if (dynamic_cast<SetIntCommand*>(command))
            std::cout << "success\n";

        m_command.emplace(command);
    }

    template<typename ...Args>
    void AddCommand(int token, Args...args)
    {
        //Command* command = nullptr;
        //if (token == 1)
        //{
        //    command = new SetIntCommand(args...);
        //}
        //else if (token == 2)
        //{
        //    command = new SetVoidCommand(args...);
        //}

        //if (command)
        //{
        //    m_command.emplace(command);
        //}
    }

    // 起一个线程，一直循环遍历 command
    std::queue<Command*> m_command;

};

class Receiver {
public:
    void SetInt(int n)
    {

    }
};

// 客户端代码可以用任何命令参数化调用程序。
int main()
{
    Invoker* invoker = new Invoker();
    invoker->AddCommand(new SetIntCommand(111));
    invoker->AddCommand(new SetVoidCommand());

    //invoker->AddCommand(1, 111);
    //invoker->AddCommand(2);


    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "   *** end ***\n";
    return 0;
}
#endif // TEST1