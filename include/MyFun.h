#ifndef __MYFUN_H__
#define __MYFUN_H__
#include <vector>
#include <functional>
#include <algorithm>
#include <ctype.h>
#include <string>

using namespace std;

namespace HuyaUtil
{

    inline unsigned char ToHex(unsigned char x)
    {
        return  x > 9 ? x + 55 : x + 48;
    }

    inline unsigned char FromHex(unsigned char x)
    {
        unsigned char y;
        if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
        else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
        else if (x >= '0' && x <= '9') y = x - '0';
        else assert(0);
        return y;
    }
    
    inline std::string UrlEncode(const std::string& str)
    {
        std::string strTemp = "";
        size_t length = str.length();
        for (size_t i = 0; i < length; i++)
        {
            if (isalnum((unsigned char)str[i]) ||
                (str[i] == '-') ||
                (str[i] == '_') ||
                (str[i] == '.') ||
                (str[i] == '~'))
                strTemp += str[i];
            else if (str[i] == ' ')
                strTemp += "+";
            else
            {
                strTemp += '%';
                strTemp += ToHex((unsigned char)str[i] >> 4);
                strTemp += ToHex((unsigned char)str[i] % 16);
            }
        }
        return strTemp;
    }

    inline std::string UrlDecode(const std::string& str)
    {
        std::string strTemp = "";
        size_t length = str.length();
        for (size_t i = 0; i < length; i++)
        {
            if (str[i] == '+') strTemp += ' ';
            else if (str[i] == '%')
            {
                assert(i + 2 < length);
                unsigned char high = FromHex((unsigned char)str[++i]);
                unsigned char low = FromHex((unsigned char)str[++i]);
                strTemp += high * 16 + low;
            }
            else strTemp += str[i];
        }
        return strTemp;
    }

    template <typename T, typename K>
    inline T myFind(T start, T end, const K &key)
    {
        T find = start;
        for (; find != end; ++find)
        {
            if (key == *find) break;
        }
        return find;
    }

    // return 0 true; else miss match fun
    inline int myAndList(const vector<std::function<bool()>> &vFuns)
    {
        int n = 0;
        for (auto &fun : vFuns)
        {
            ++n;
            if (!fun()) return n;
        }
        return 0;
    }


    enum FilterResult
    {
        FilterNormal,   //正常插入
        FilterRest,     //补位插入
        FilterThrow        //丢弃
    };

    // 插入的vector不会被清空旧数据,返回开始补位的size()
    template <typename IN_TYPE, typename OUT_TYPE>
    inline int myAssign(const vector<IN_TYPE> *from, vector<OUT_TYPE> &to,
        const std::function<OUT_TYPE(const IN_TYPE&)> convertFun = [](const IN_TYPE&){return OUT_TYPE(); },
        const std::function<FilterResult(const IN_TYPE&)> filterFun = [](const IN_TYPE&){return true; },
        const bool &isAll = true,
        const unsigned &pos = 0,
        const unsigned &len = 0
        )
    {
        // 参数检查
        if (pos >= from->size() || (!isAll && (pos < 0 || len <= 0)))
            return -1;
        // 补位暂存
        typedef typename vector<IN_TYPE>::const_iterator const_iterator;
        vector<const_iterator> vRest;
        int uRestPos = -1;
        unsigned uInsertNum = 0;
        unsigned uBeforNum = 0;     //分页之前的有效数据
        // 设定起点
        typedef typename vector<IN_TYPE>::const_iterator const_iterator;
        const_iterator start = from->begin();
        // 执行筛选
        for (unsigned index = 0; start != from->end(); ++start)
        {
            FilterResult result = filterFun(*start);
            if (result == FilterNormal)
            {
                if (isAll)      // 全部符合的都插入
                    to.push_back(convertFun(*start));
                else if (pos <= index)   // 请求的分页内插入
                {
                    to.push_back(convertFun(*start));
                    ++uInsertNum;
                    if (uInsertNum == len) break;   // 分页已经全部插入,跳出
                }
                else
                {
                    ++uBeforNum;    // 本数据有效,但是再分页之前
                }
                ++index;
            }
            else if (result == FilterRest)
            {
                if (isAll)
                    vRest.push_back(start);
                else if (vRest.size() != pos+len)     // 正式数据还不够,需要补位
                {
                    vRest.push_back(start);
                }
                else break;
            }
        }
        
        // 执行补位
        if (isAll)
        {
            // 记录补位pos
            uRestPos = to.size();
            for (auto iter : vRest)
                to.push_back(convertFun(*iter));
        }
        else if (uInsertNum < len)
        {
            // 记录补位pos
            uRestPos = to.size();

            int passNum = pos - uBeforNum;   // 在pos之前补过的数据要去掉
            int subNum = len - uInsertNum;    // 需要补多少个

            for (auto iter : vRest)
            {
                if (passNum <= 0 && subNum>0)
                {
                    to.push_back(convertFun(*iter));
                    --subNum;
                    if (subNum == 0) break;
                }
                --passNum;
            }
        }
        return uRestPos;
    }

    enum SQLTYPE
    {
        SQLTYPE_IDS=1,
        SQLTYPE_VALUES,
    };

    // return string like (a1,b1,c1),(a2,b2,c2)
    // fun format <a,b,c> to "a,b,c"
    template <typename Container, typename Fun>
    inline string formatSql(const Container &con, const Fun &fun, const SQLTYPE &sqlType)
    {
        string str;
        for (auto &tmp : con)
        {
            switch (sqlType)
            {
            case SQLTYPE_IDS:
                str += fun(tmp) + ",";
                break;
            case SQLTYPE_VALUES:
                str += "(" + fun(tmp) + "),";
                break;
            default:
                break;
            }
        }
        if (!str.empty())
        {
            str.erase(str.end() - 1);
            switch (sqlType)
            {
            case SQLTYPE_IDS:
                str = "(" + str + ")";
                break;
            default:
                break;
            }
            return str;
        }
        else
        {
            return "";
        }
    }
}


#endif
