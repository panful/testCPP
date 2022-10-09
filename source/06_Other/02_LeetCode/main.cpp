/*
* 1.
* 2. https://leetcode-cn.com/problems/longest-substring-without-repeating-characters/
* 3.
* 4. 两个有序数组中位数 https://leetcode-cn.com/problems/median-of-two-sorted-arrays/
* 5. 最长回文子串 https://leetcode-cn.com/problems/longest-palindromic-substring/
* 6. 正则表达式
* 7. 斐波拉契数列
*
*/

#define TEST7

#ifdef TEST1

#include <vector>

using namespace std;

struct ListNode {
    int val{ 0 };
    ListNode* next{ nullptr };
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
    ListNode(initializer_list<int> list)
    {
        ListNode* node = nullptr;
        auto it = list.begin();
        while (it != list.end())
        {
            if (!next)
            {
                val = *it++;
                if (it != list.end())
                    next = node = new ListNode(*it++);
            }
            else
            {
                node->next = new ListNode(*it++);
                node = node->next;
            }
        }
    }
};

class Solution {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode* head = nullptr;
        ListNode* node = nullptr;
        std::vector<int> ret1;
        std::vector<int> ret2;
        while (l1)
        {
            ret1.emplace_back(l1->val);
            l1 = l1->next;
        }

        while (l2)
        {
            ret2.emplace_back(l2->val);
            l2 = l2->next;
        }

        if (ret1.size() > ret2.size())
        {
            ret2.resize(ret1.size());
        }
        else
        {
            ret1.resize(ret2.size());
        }

        bool flag = false;
        int num = 0;
        for (int i = 0; i < ret1.size(); i++)
        {
            num = ret1[i] + ret2[i];

            if (flag)
            {
                num++;
                flag = false;
            }

            if (num > 9)
            {
                flag = true;
                if (!head)
                {
                    head = node = new ListNode(num - 10);
                }

                else
                {
                    node->next = new ListNode(num - 10);
                    node = node->next;
                }
            }
            else
            {
                if (!head)
                    head = node = new ListNode(num);
                else
                {
                    node->next = new ListNode(num);
                    node = node->next;
                }
            }
        }
        if (flag)
        {
            node->next = new ListNode(1);
            node = node->next;
        }
        return head;
    }
};

int main()
{
    ListNode* l1 = new ListNode({ 9,9,9,9,9,9,9 });
    ListNode* l2 = new ListNode({ 9,9,9,9 });

    Solution s;
    auto ret = s.addTwoNumbers(l1, l2);


    int test = 0;
}

#endif // TEST1

#ifdef TEST2

#include <string>
using namespace std;

class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        string str;
        int size = 0;
        for (auto elem : s)
        {
            if (str.find(elem) == string::npos)
                str += elem;
            else
            {
                size = size > str.size() ? size : str.size();

                str = str.substr(str.find_first_of(elem) + 1);

                //str.clear();
                str += elem;
            }

        }
        return size > str.size() ? size : str.size();
    }
};

int main()
{
    Solution s;
    auto ret = s.lengthOfLongestSubstring("abcabcbb");

    int test = 0;
}
#endif // TEST2

#ifdef TEST4
#include <vector>
#include <set>
using namespace std;

class Solution {
public:
    double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
        multiset<int> Myset;
        Myset.insert(nums1.begin(), nums1.end());
        Myset.insert(nums2.begin(), nums2.end());
        auto size = Myset.size();
        auto it = Myset.begin();

        if (size % 2 == 0)
        {
            std::advance(it, (size - 2) / 2);// 偶数
            return (*it++ + *it) / 2.0;
        }
        else
        {
            std::advance(it, (size - 1) / 2);// 奇数
            return *it;
        }
    }
};

int main()
{
    vector<int> v1{ 1,2,3,4 };
    vector<int> v2{ 5,6,7 };

    Solution s;
    auto ret = s.findMedianSortedArrays(v1, v2);

    int test = 0;

}

#endif // TEST4

#ifdef TEST5
#include <string>
#include <vector>
using namespace std;

// 暴力
class Solution {
public:
    string longestPalindrome(string s) {
        if (s.size() < 2)
            return s;
        auto size = s.size();
        string ret;
        auto maxSize = 0;
        for (size_t i = 0; i < size; i++)
        {
            auto length = size - i;

            for (size_t j = 0; j < length; j++)
            {
                string str1 = s.substr(i, length - j);
                string str2 = str1;
                std::reverse(str1.begin(), str1.end());
                if (str1 == str2)
                {
                    if (str2.size() > maxSize)
                    {
                        ret = str2;
                        maxSize = str2.size();
                    }

                }
            }
        }
        return ret;
    }
};

// 中心扩展（时间空间都很小）
class Solution1 {
public:
    void extend(string& s, string& res, int l, int r, int n)
    {
        while (l >= 0 && r < n && s[l] == s[r])
        {
            //if (r - l + 1 > res.size())
            //{
            //	res = s.substr(l, r - l + 1);
            //}
            l--;
            r++;
        }

        if (r - l - 1 > res.size())
        {
            res = s.substr(l + 1, r - l - 1);
        }
    }

    string longestPalindrome(string s) {
        int n = s.size();
        string res = "";

        for (int i = 0; i < n; i++)
        {
            extend(s, res, i, i, n);
            extend(s, res, i, i + 1, n);
        }
        return res;
    }
};

// 动态规划
class Solution2 {
public:
    string longestPalindrome(string s) {
        int len = s.size();
        if (len == 0 || len == 1)
            return s;
        int start = 0;//回文串起始位置
        int max = 1;//回文串最大长度
        vector<vector<int>>  dp(len, vector<int>(len));//定义二维动态数组
        for (int i = 0; i < len; i++)//初始化状态
        {
            dp[i][i] = 1;
            if (i < len - 1 && s[i] == s[i + 1])
            {
                dp[i][i + 1] = 1;
                max = 2;
                start = i;
            }
        }
        for (int l = 3; l <= len; l++)//l表示检索的子串长度，等于3表示先检索长度为3的子串
        {
            for (int i = 0; i + l - 1 < len; i++)
            {
                int j = l + i - 1;//终止字符位置
                if (s[i] == s[j] && dp[i + 1][j - 1] == 1)//状态转移
                {
                    dp[i][j] = 1;
                    start = i;
                    max = l;
                }
            }
        }
        return s.substr(start, max);//获取最长回文子串
    }
};

// 翻转找最大子串
class Solution3 {
public:
    string longestPalindrome(string s) {
        if (s.length() == 1) return s;//大小为1的字符串必为回文串
        string rev = s;//rev存放s反转结果
        string res;//存放结果
        std::reverse(rev.begin(), rev.end());
        if (rev == s) return s;
        int len = 0;//存放回文子串的长度
        for (int i = 0; i < s.length(); i++)//查找s与rev的最长公共子串
        {
            string temp;//存放待验证子串
            for (int j = i; j < s.length(); j++)
            {
                temp = temp + s[j];
                if (len >= temp.length())
                    continue;
                else if (rev.find(temp) != -1)//在rev中找到temp
                {
                    string q = temp;//q用来验证temp是否是回文子串
                    std::reverse(q.begin(), q.end());
                    if (q == temp)
                    {
                        len = temp.length();
                        res = temp;
                    }
                }
                else break;
            }
            temp = "";
        }
        return res;
    }
};

// 动态规划
class Solution4 {
public:
    string longestPalindrome(string s) {
        int len = s.size();
        if (len == 0 || len == 1)
            return s;
        int start = 0; //回文串起始位置
        int max = 1;   //回文串最大长度
        vector<vector<int>>  dp(len, vector<int>(len));//定义二维动态数组
        for (int i = 0; i < len; i++)//初始化状态
        {
            dp[i][i] = 1;
            if (i < len - 1 && s[i] == s[i + 1])
            {
                dp[i][i + 1] = 1;
                max = 2;
                start = i;
            }
        }
        for (int l = 3; l <= len; l++)//l表示检索的子串长度，等于3表示先检索长度为3的子串
        {
            for (int i = 0; i + l - 1 < len; i++)
            {
                int j = l + i - 1;//终止字符位置
                if (s[i] == s[j] && dp[i + 1][j - 1] == 1)//状态转移
                {
                    dp[i][j] = 1;
                    start = i;
                    max = l;
                }
            }
        }
        return s.substr(start, max);//获取最长回文子串
    }
};

// 中心扩展法
class Solution5 {
public:
    string longestPalindrome(string s) {
        size_t length = s.size();
        if (length < 2)
            return s;

        size_t maxLen = 1;
        size_t start = 0;
        for (size_t i = 0; i < length; i++)
        {
            auto len1 = expandAround(s, i, i);
            auto len2 = expandAround(s, i, i + 1);

            if (std::max(len1, len2) > maxLen)
            {
                start = i;
                maxLen = std::max(len1, len2);
            }
        }
        if (maxLen % 2 == 0)
            return s.substr(start - (maxLen - 1) / 2, maxLen);
        else
            return s.substr(start - maxLen / 2, maxLen);
    }

    size_t expandAround(string s, size_t _left, size_t _right)
    {
        int left = _left;
        int right = _right;

        while (left >= 0 && right < s.length() && s[left] == s[right])
        {
            left--;
            right++;
        }

        return right - left - 1;
    }
};

// 马拉车

int main()
{
    Solution5 s;
    auto ret = s.longestPalindrome("ababax");

    int test = 0;
}

#endif // TEST5

#ifdef TEST6

#include <string>
#include <vector>
using namespace std;

class Solution {
public:
    bool isMatch(string s, string p) {
        size_t len1 = p.size();
        size_t len2 = s.size();

        if (len1 <= 0 || len2 <= 0)
            return false;

        if (p[0] == '*')
            return false;

        size_t current = 0;
        char pre = p[0];

        for (size_t i = 0; i < len1; i++)
        {
            if (p[i] == '.')
            {
                current++;
                continue;
            }
            else if (p[i] = '*')
            {
                if (p[i - 1] == '.');
            }
            else
            {
                if (p[i] != s[current])
                    return false;
            }

        }
    }
};


class Solution1 {
public:
    bool isMatch(string s, string p) {
        int m = s.size(), n = p.size();
        vector<vector<int>> dp(m + 1, vector<int>(n + 1, false));
        dp[0][0] = true;
        for (int i = 2; i <= n; i++) {
            if (p[i - 1] == '*')
                dp[0][i] = dp[0][i - 2];
        }
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                if (p[j - 1] == '*') {
                    dp[i][j] = dp[i][j - 2] || (dp[i - 1][j] && (s[i - 1] == p[j - 2] || p[j - 2] == '.'));
                }
                else {
                    dp[i][j] = dp[i - 1][j - 1] && (s[i - 1] == p[j - 1] || p[j - 1] == '.');
                }
            }
        }
        return dp[m][n];
    }
};


int main()
{
    Solution1 s;
    auto ret = s.isMatch("abc", ".*");
}
#endif // TEST6

#ifdef TEST7

class Solution {
public:
    int fib(int n) {
        if (n < 2) {
            return n;
        }
        int p = 0, q = 0, r = 1;
        for (int i = 2; i <= n; ++i) {
            p = q;
            q = r;
            r = p + q;
        }
        return r;
    }
};

int fib(int x)
{
    if (x < 2)
        return x;

    int n = 0, m = 0, ret = 1;
    for (size_t i = 2; i <= x; ++i)
    {
        n = m;
        m = ret;
        ret = m + n;
    }

    return ret;
}

int main()
{
    //Solution s;
    //auto ret = s.fib(5);

    auto ret1 = fib(5);

    return 0;
}

#endif // TEST7

