
/*
* 1. C++反射 rpc https://blog.csdn.net/weixin_44857939/article/details/115867694
* 2. RPC传参
* 3. c++反射ponder https://github.com/billyquith/ponder
*/

#define TEST2

#ifdef TEST1

比如我们使用一个map来实现下面两个不同参数列表的函数反射。
void foo1(int);
float foo2(float);

首先指定一个模板函数
template<typename F>
void call(F fun, Serializer* ds, char* data, int len) {
    //需要在借助于一个函数来解析F类型
    callproxy(fun, ds, data, len);
}

template<typename R, typename Params...>
void callproxy(std::function<R(Params... ps)>fun) {
    //使用tuple从data得到参数列表args
    r = std::invoke(fun, args);
    把r放入到ds序列化返回值
}

map<string, std::function<void(Serializer* ds, char* data, int len)>>handler;
template<typename F>
void bind(string name, F fun) {
    handler[name] = std::bind(call <F>，std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

//如何调用
准备好传入传出参数。
Serializer* ds = new Serializer();
char* data;
int len;
handlers["foo1"](ds, data, len);

#endif // TEST1

#ifdef TEST2

#include <cstdio> 
#include <cstdint> 
#include <map> 
#include <vector> 
#include <deque> 
#include <functional> 

// "rpc" infrastructure (could be an API for a dll or a lib or so: 

// Just one way to do it. Somehow, your various data types need 
// to be handled/represented. 
class RpcVariant
{
public:
    enum class VariantType
    {
        RVT_EMPTY,
        RVT_UINT,
        RVT_SINT,
        RVT_FLOAT32,
        RVT_BYTES
    };
private:
    VariantType m_type;
    uint64_t m_uintValue;
    int64_t m_intValue;
    float m_floatValue;
    std::vector<uint8_t> m_bytesValue;

    explicit RpcVariant(VariantType type)
        : m_type(type)
    {

    }
public:
    static RpcVariant MakeEmpty()
    {
        RpcVariant result(VariantType::RVT_EMPTY);
        return result;
    }
    static RpcVariant MakeUint(uint64_t value)
    {
        RpcVariant result(VariantType::RVT_UINT);
        result.m_uintValue = value;
        return result;
    }
    // ... More make-functions 

    uint64_t AsUint() const
    {
        // TODO: check if correct type... 
        return m_uintValue;
    }
    // ... More AsXXX() functions 

    // ... Some ToWire()/FromWire() functions... 
};
typedef std::map<uint32_t, RpcVariant> RpcMessage_t;
typedef std::function<void(const RpcMessage_t*)> RpcHandler_t;

void RpcInit();
void RpcUninit();

// application writes handlers and registers them with the infrastructure. 
// rpc_context_id can be anything opportune - chose uint32_t, here. 
// could as well be a string or a pair of values (service,method) or whatever. 
void RpcRegisterHandler(uint32_t rpc_context_id, RpcHandler_t handler);

// Then according to taste/style preferences some receive function which uses the registered information and dispatches to the handlers... 
void RpcReceive();
void RpcBeginReceive();
void RpcEndReceive();

// maybe some sending, too... 
void RpcSend(uint32_t rpc_context_id, const RpcMessage_t* message);

int main(int argc, const char* argv[])
{
    RpcInit();
    RpcRegisterHandler(42, [](const RpcMessage_t* message) { puts("message type 42 received."); });
    RpcRegisterHandler(43, [](const RpcMessage_t* message) { puts("message type 43 received."); });
    while (true)
    {
        RpcReceive();
    }
    RpcUninit();
    return 0;
}
#endif // TEST2