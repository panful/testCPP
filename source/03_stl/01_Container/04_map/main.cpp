
#include <iostream>
#include <map>
#include <string>

int main()
{
    {
        std::map<int, float> map;
        // 返回值类型为：std::pair<std::map<key_type, value_type>::iterator, bool>
        // 插入失败：容器中已有key的迭代器和 false
        // 插入成功：当前插入元素的迭代器和 true
        auto r1 = map.insert(std::make_pair(1, 1.1f)); // 插入成功，返回 ((1, 1.1f), true)
        auto r2 = map.insert(std::make_pair(1, 1.2f)); // 插入失败，返回 ((1, 1.1f), false)
        auto r3 = map.insert({ 2, 2.2f });             // 插入成功，返回 ((2, 2.2f), true)

        // 返回值类型为：std::map<key_type, value_type>::iterator
        auto r4 = map.insert(map.begin(), { 3, 3.3f }); // 插入成功，返回 (3, 3.3f)，第一个参数只是一个提示，并不一定在给定位置插入
        auto r5 = map.insert(map.begin(), { 1, 3.3f }); // 插入失败，返回 (1, 1.1f)
    }

    {
        std::map<int, float> map { { 1, 1.1f }, { 2, 2.2f }, { 3, 3.3f } };
        // 返回值类型为：std::pair<std::map<key_type, value_type>::iterator, bool>
        // 如果 key 不存在：当前插入元素的迭代器和 true
        // 如果 key 已存在：当前插入元素的迭代器和 false ，注意是 insert() 的区别
        auto r1 = map.insert_or_assign(4, 4.4f); // 若key不存在则插入成功
        auto r2 = map.insert_or_assign(1, 3.3f); // 若key已存在则替换掉原来的value
    }

    {
        std::map<int, float> map { { 1, 1.1f }, { 2, 2.2f }, { 3, 3.3f } };
        auto r1 = map.emplace(1, 3.3f);                   // 如果 key 不存在则插入成功，否则失败
        auto r2 = map.try_emplace(2, 4.4f);               // 如果 key 已存在则不做任何操作，即不构造 std::pair<key_type, value_type>
        auto r3 = map.emplace_hint(map.begin(), 2, 3.3f); // 在给定位置插入元素，如果位置合适则能提高效率，否则和 emplace() 一样
    }

    {
        std::map<int, float> map { { 1, 1.1f }, { 2, 2.2f }, { 3, 3.3f } };
        map.at(1) = 2.2f; // 修改 key 对应的 value
        map[2]    = 3.3f; // 修改 key 对应的 value
        auto x = map[4]; // 不会崩溃，如果没有对应的 key，则会插入这个 key，此时 value 是默认构造生成的值 【特别要注意】
        try
        {
            auto y = map.at(5); // 如果没有对应的key，会抛出异常
        }
        catch (...)
        {
            std::cout << "not have key: 5\n";
        }
    }

    // 修改 key
    {
        std::map<int, char> cont { { 1, 'a' }, { 2, 'b' }, { 3, 'c' } };

        auto nh  = cont.extract(1); // 提取节点句柄，会将该元素从 map 中移除
        nh.key() = 4;               // 修改节点的 key
        cont.insert(std::move(nh)); // 将节点句柄插回去，相当于将 map 的键值修改
    }

    // 合并
    {
        std::map<int, char> map1 { { 1, 'a' }, { 2, 'b' }, { 9, 'c' } };
        std::map<int, char> map2 { { 5, 'x' }, { 2, 'y' }, { 3, 'z' } };
        map1.merge(map2); // 将 map2 中的 key 在 map1 中没有的元素移动到 map1 中
        // map1 { { 1, 'a' }, { 2, 'b' }, { 3, 'z' }, { 5, 'x' }, { 9, 'c' } }
        // map2 { { 2, 'y' } }
    }

    // 查找
    {
        std::map<int, char> cont { { 1, 'a' }, { 2, 'b' }, { 3, 'c' } };
        auto has_1   = cont.contains(1); // true 如果 map 中有当前 key 元素，则返回 true，否则返回 false
        auto has_5   = cont.contains(5); // false c++20
        auto count_1 = cont.count(1);    // 1 返回 map 中含有当前 key 的元素的个数
        auto count_5 = cont.count(5);    // 0
    }

    {
        std::map<int, char> cont { { 1, 'a' }, { 2, 'b' }, { 3, 'c' }, { 7, 'x' }, { 8, 'y' }, { 9, 'z' } };
        auto lb1 = cont.lower_bound(3); // 返回指向首个不小于给定键的元素的迭代器
        auto lb2 = cont.lower_bound(5);
        auto lb3 = cont.lower_bound(9);

        auto ub1 = cont.upper_bound(3); // 返回指向首个大于给定键的元素的迭代器
        auto ub2 = cont.upper_bound(5);
        auto ub3 = cont.upper_bound(9);

        auto er1 = cont.equal_range(3); // first 是 lower_bound()，second 是 upper_bound()
        auto er2 = cont.equal_range(5);
        auto er3 = cont.equal_range(9);
    }
}
