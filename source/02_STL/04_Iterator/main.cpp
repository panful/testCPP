/**
 * 迭代器 https://zh.cppreference.com/w/cpp/iterator
 * 1. 迭代器操作 advance distance next prev
 * std::insert_iterator std::reverse_iterator std::move_iterator std::counted_iterator std::common_iterator
 */

#define TEST1

#ifdef TEST1

#include <iostream>
#include <iterator>
#include <vector>

int main()
{
    std::vector<int> vec { 0, 1, 2, 3, 4, 5, 6, 7 };
    auto dist = std::distance(vec.cbegin(), vec.cbegin() + 3);
    auto iter = vec.begin();      // iter指向0
    std::advance(iter, 3);        // 将iter向后移动3，iter指向3
    auto iter2 = std::next(iter); // iter自增，iter指向4
    auto iter3 = std::prev(iter); // iter自减，iter指向2
}

#endif // TEST1
