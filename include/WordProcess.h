#ifndef __H_WORDPROCESSCOMM_H__
#define __H_WORDPROCESSCOMM_H__

#include <iostream>
#include <string>
#include <ctype.h>
#include <algorithm>
#include <set>
#include "Word.h"
#include "iconv.h"
#include "Hz2Py.h"


using namespace std;

// 命名统一:
// SG: single word 表示单字
// alpha：字母
// digit：数字


class WordProcess
{
public:
	WordProcess()
	{
	}


public:
	// 判断字符串是否是纯字母或者数字
	static bool isAlphaOrDigit(const std::string& src)
	{
		for (size_t i = 0; i < src.length(); ++i)
		{
			unsigned char tmp = src[i];
			if (false == isalpha(tmp) && ' ' != tmp && false == isdigit(tmp))
			{
				return false;
			}
		}

		return true;
	}

	/**
	 *@brief: 从字符串里面获取单个非字母&数字的单个词. 主要作为搜索分词用
	 *@param[bTrans]: 为true的时候，做汉子的简繁体转换，以及全角半角转换。
	 *@return:
	 */
	static bool getSingleNonAlphaSet(const std::string & src, std::set<std::string>& res, bool bTrans = true)
	{
		res.clear();

		std::string ch;
		for (size_t i = 0, len = 0; i != src.length(); i += len) {
			unsigned char byte = (unsigned)src[i];
			if (byte >= 0xFC) // lenght 6
				len = 6;
			else if (byte >= 0xF8)
				len = 5;
			else if (byte >= 0xF0)
				len = 4;
			else if (byte >= 0xE0)
				len = 3;
			else if (byte >= 0xC0)
				len = 2;
			else
				len = 1;
			ch = src.substr(i, len);

			if (1 == len) // length be 1 ,mean alpha words, ignore it.
			{
				continue;
			}

			// 如果长度为3，考虑偏向于汉字体系， 尝试简繁体转换. 这里后续看要不要根据语系做细化。
			if (true == bTrans && 3 == len)
			{
				// 处理全角半角
				if (true == fullToHalf(ch))
				{
					ch = TC_Common::upper(ch); // std::toupper 对全角字母是没有大写转换效果的
					res.insert(ch);
					continue;
				}
				else // 全半角转换和简繁体肯定不会同时出现
				{
					auto itor = g_mComplexToSimpleCN.find(ch);
					if (true == bTrans && g_mComplexToSimpleCN.end() != itor)
					{
						ch = itor->second;
					}
				}
			}

			if (len >= 5)
			{
				LOG->debug() << __FUNCTION__ << " char:" << ch << " len:" << len << endl;
			}

			res.insert(ch); 
		}

		return true;
	}

	// 从字串里面获取非中文的字符串列表。如： 水123电asdf中1212df  得到结果 [123,asdf,1212df]
	static bool getSerialUpperAlphabetSet(const std::string & src, std::vector<std::string>& res)
	{
		string sSerial;
		for (size_t i = 0, len = 0; i != src.length(); i += len) {
			unsigned char byte = (unsigned)src[i];

			if (byte >= 0xFC) // lenght 6
				len = 6;
			else if (byte >= 0xF8)
				len = 5;
			else if (byte >= 0xF0)
				len = 4;
			else if (byte >= 0xE0)
				len = 3;
			else if (byte >= 0xC0)
				len = 2;
			else
				len = 1;

			if (1 == len)
			{
				if (('0' <= byte && '9' >= byte) || ('A' <= byte && 'Z' >= byte))
				{
					sSerial += src.substr(i, len);
					continue;
				}

				if (('a' <= byte && 'z' >= byte))
				{
					char c = (char)((int)byte - 32);
					// 以前的写法: sSerial += string(&c)  这样可能会出现后面出现乱码的问题，因为string不知道长度。具体可能会根据编译器不同出现不一样的结果。目前看gcc这样是没有问题的。现在这种写法最稳妥。
					sSerial += string(1, c);
					continue;
				}
			}

			// 进入这里说明连续断了
			if (!sSerial.empty())
			{
				res.emplace_back(sSerial);
				sSerial = "";
			}
		}

		if (!sSerial.empty()) // 最后一个
		{
			res.emplace_back(sSerial);
		}

		return res.size();
	}

	/**
	 *@brief: 获取连续的字符串集合
	 *@param[src]: 源字符串
	 *@param[res]: 连续字符串集合。包含中文和英文的
	 *@return:
	 */
	static bool getSerialStrSet(const std::string & src, std::vector<std::string>& res)
	{
		string sSerialAlpha;
		string sSerialNonAlpha;
		for (size_t i = 0, len = 0; i != src.length(); i += len) {
			unsigned char byte = (unsigned)src[i];

			if (byte >= 0xFC) // lenght 6
				len = 6;
			else if (byte >= 0xF8)
				len = 5;
			else if (byte >= 0xF0)
				len = 4;
			else if (byte >= 0xE0)
				len = 3;
			else if (byte >= 0xC0)
				len = 2;
			else
				len = 1;

			if (1 == len)
			{
				// 进入这里说明非字母的连续断了
				if (!sSerialNonAlpha.empty())
				{
					res.emplace_back(sSerialNonAlpha);
					sSerialNonAlpha = "";
				}

				if (('0' <= byte && '9' >= byte) || ('A' <= byte && 'Z' >= byte))
				{
					sSerialAlpha += src.substr(i, len);
					continue;
				}

				if (('a' <= byte && 'z' >= byte))
				{
					char c = (char)((int)byte - 32);
					// 以前的写法: sSerial += string(&c)  这样可能会出现后面出现乱码的问题，因为string不知道长度。具体可能会根据编译器不同出现不一样的结果。目前看gcc这样是没有问题的。现在这种写法最稳妥。
					sSerialAlpha += string(1, c);
					continue;
				}
			}
			else
			{
				// 进入这里说明连续断了
				if (!sSerialAlpha.empty())
				{
					res.emplace_back(sSerialAlpha);
					sSerialAlpha = "";
				}

				string ch = src.substr(i, len);
				if (3==len)
				{
					// 处理全角半角
					if (true == fullToHalf(ch))
					{
						ch = TC_Common::upper(ch); // std::toupper 对全角字母是没有大写转换效果的
					}
					else // 全半角转换和简繁体肯定不会同时出现
					{
						auto itor = g_mComplexToSimpleCN.find(ch);
						if (g_mComplexToSimpleCN.end() != itor)
						{
							ch = itor->second;
						}
					}
				}
				sSerialNonAlpha += ch;
			}

	
		}

		if (!sSerialAlpha.empty()) // 最后一个
		{
			res.emplace_back(sSerialAlpha);
		}

		if (!sSerialNonAlpha.empty())
		{
			res.emplace_back(sSerialNonAlpha);
		}

		return res.size();
	}

	// 获取字符串的长度。 1毛钱A1 = 5
	static int getStrLen(const string & src)
	{
		int iLen = 0;

		for (size_t i = 0, len = 0; i < src.length(); i += len)
		{
			unsigned char byte = (unsigned)src[i];
			if (byte >= 0xFC) // lenght 6
				len = 6;
			else if (byte >= 0xF8)
				len = 5;
			else if (byte >= 0xF0)
				len = 4;
			else if (byte >= 0xE0)
				len = 3;
			else if (byte >= 0xC0)
				len = 2;
			else
				len = 1;

			++iLen;
		}

		return iLen;
	}

	/**
	 *@brief: 全角转半角, 转换代码按照utf8编码进行转换，请先确保字符串是utf8编码的。
	 *@param[s]: 待转换的字符串。转换后直接修改s
	 *@return: 是否有过全角转换动作
	 */
	static bool fullToHalf(string& s)
	{
		bool b =false;
		string r;
		for (size_t i = 0, len = 0; i != s.length(); i += len)
		{
			unsigned char byte = (unsigned)s[i];
			if (byte >= 0xFC) // lenght 
				len = 6;
			else if (byte >= 0xF8)
				len = 5;
			else if (byte >= 0xF0)
				len = 4;
			else if (byte >= 0xE0)
				len = 3;
			else if (byte >= 0xC0)
				len = 2;
			else
				len = 1;

			if (3 != len)
			{
				r += s.substr(i, len);
				continue;
			}

			unsigned int val = 0;
			val += (unsigned char)s[i + 0];
			val = val << 8;
			val += (unsigned char)s[i + 1];
			val = val << 8;
			val += (unsigned char)s[i + 2];

			// utf8编码是固定的，所以这里硬编码也是ok的
			if (0xe38080 == val) // 空格
			{
				b = true;
				val = 0x20; 
			}
			else if (0XEFBC81 <= val && val <= 0XEFBCBF)
			{
				b = true;
				val -= 0xEFBC60;
			}
			else if (0XEFBD80 <= val && val <= 0XEFBD9E)
			{
				b = true;
				val -= 0xEFBD20;
			}
			else // 全角匹配失败，保存原有字符串
			{
				r += s.substr(i, len);
				continue;
			}

			r += string(1, char(val));
		}

		s = r;

		return b;
	}

	// utf8-转-gbk
	static string UTF8toANSI(const std::string &from)
	{
		string sRes;
		if (from.length() > 1024)
		{
			LOG->error() << __FUNCTION__ << "|find string to long to transfrom:" << from << endl;
			return sRes;
		}

		char *inbuf = const_cast<char*>(from.c_str());
		size_t inlen = strlen(inbuf);
		size_t outlen = inlen * 4;
		char outbuf[4096] = { 0 };

		char *in = inbuf;
		char *out = outbuf;

		// GBK-- gb2312 扩充版，包括所有汉字。
		iconv_t cd = iconv_open("GBK", "UTF-8");
		if ((iconv_t)-1 == cd)
		{
			LOG->error() << __FUNCTION__ << "|iconv_open failed." << endl;
			return sRes;
		}

		int iret = iconv(cd, &in, &inlen, &out, &outlen);
		if (-1 == iret)
		{
			LOG->error() << __FUNCTION__ << "|iconv failed." << endl;
		}

		iconv_close(cd);

		return string(outbuf);
	}
	

	// 获取每个汉字的完整拼音. (拼音连接起来，中间不留空格)
	static string transCnToPinyin(string strText)
	{
		strText = TC_Common::replace(strText, " ", "");
		strText = UTF8toANSI(strText);

		string sResult;
		unsigned char ucHigh = 0, ucLow = 0;
		int  nCode = 0;
		string strPinYin = "";

		for (size_t i = 0; i < strText.length();)
		{
			ucHigh = (unsigned char)strText[i++]; // 获取机内码高字节
			ucLow = (unsigned char)strText[i++];  // 获取机内码低字节

			if (ucHigh >= 0xa0 || ucLow >= 0xa0)
			{
				nCode = (ucHigh - 0xa0) * 100 + ucLow - 0xa0; // 机内码 --> 区位码
				strPinYin = Hz2Py(nCode);					  // 根据区位码找到对应的拼音

				sResult += strPinYin;
			}
		}

		return TC_Common::upper(sResult);
	}


	// 规范化字符串。 全角转为半角的、繁体的转为简体的、英文全部转为大写
	static std::string normalizeStr(const std::string& src)
	{
		string sRes;
		std::string ch;
		for (size_t i = 0, len = 0; i != src.length(); i += len, sRes += ch) {
			unsigned char byte = (unsigned)src[i];
			if (byte >= 0xFC) // lenght 6
				len = 6;
			else if (byte >= 0xF8)
				len = 5;
			else if (byte >= 0xF0)
				len = 4;
			else if (byte >= 0xE0)
				len = 3;
			else if (byte >= 0xC0)
				len = 2;
			else
				len = 1;
			ch = src.substr(i, len);

			//cout << ch << "|" << len << endl;

			if (1 == len && ('a' <= byte && byte <= 'z')) // 小写字母转大写
			{
				char c = (char)((int)byte - 32);
				ch = string(&c, 1);
			}

			if (3 == len)
			{
				if (true == fullToHalf(ch))
				{
					ch = TC_Common::upper(ch);
					continue;
				}
				else
				{
					auto itor = g_mComplexToSimpleCN.find(ch);
					if (g_mComplexToSimpleCN.end() != itor)
					{
						ch = itor->second;
						continue;
					}
				}
			}
		}

		return sRes;
	}


	//C++标准库里面的string::rfind和string : find不是用快速匹配算法实现的，效率不是一般的差。
	//但是由于其逻辑比较简单，减少了函数调用的次数，反而有些时间觉得还是挺快的。
	//为了提高string::find和string::rfind的效率，我实现了两个类似的函数 StringRFind和StringFind，分别对应 string::rfind和string::find。
	//使用系统的 QueryPerformanceCounter 进行精确的速度测试，发现我的程序比标准库的快50倍左右。
	//我的程序的算法是快速匹配算法的简化，采用1级跳跃的方式实现O(N + X*M)的性能，当匹配失败时，如果已经匹配的字符个数没有超过要匹配字符串中首字母的重复距离，
	// 即可不用回退，采用下一个字母与要匹配的字符串进行比较。
	//在实践中还发现，string::find(char)比较string::find(string)慢很多，所以建议不要使用string::find(char)

	size_t stringRFind(
		const string& strContent,
		const string& strToFind,
		size_t nStartPos = 0)
	{
		size_t nToFindLen = strToFind.length();

		//no enough charactor to compare
		if ((nStartPos < nToFindLen) || (nStartPos >= strContent.length()))
		{
			return string::npos;
		}

		//iterator to begin of the strContent
		string::const_iterator itContentBegin = strContent.begin();
		//iterator to last charactor of the strToFind
		string::const_iterator itToFindEnd = strToFind.begin() + nToFindLen - 1;
		//iterator to current scan charactor in strToFind
		string::const_iterator itCharInToFind = itToFindEnd;
		//iterator to current scan charactor in strContent
		string::const_iterator itCharInContent = itContentBegin + nStartPos;

		//matched charactors number
		size_t nCharMatched = 0;

		//repeat charactor with first charactor in strToFind
		size_t nNoRepeatLen = 1;
		itCharInToFind--;
		while ((nNoRepeatLen != nToFindLen) && ((*itCharInToFind) != (*itToFindEnd)))
		{
			nNoRepeatLen++;
			itCharInToFind--;
		}
		itCharInToFind = itToFindEnd;

		//the for loop will not stop at itCharInContent == itContentBegin, because maybe (*itContentBegin) == (*itCharInToFind)
		for (; nCharMatched != nToFindLen; itCharInContent--)
		{
			//compare string
			while ((*itCharInContent) == (*itCharInToFind))
			{
				nCharMatched++;

				if ((itCharInContent == itContentBegin) || (nCharMatched == nToFindLen))
				{
					break;
				}

				itCharInContent--;
				itCharInToFind--;
			}

			//all charactor match, or no content to search
			if ((itCharInContent == itContentBegin) || (nCharMatched == nToFindLen))
			{
				break;
			}

			//only nCharMatched charactors match
			if (nCharMatched > 0)
			{
				//move back only when match char num large than no repeat char in strToFind
				if (nCharMatched > nNoRepeatLen)
				{
					itCharInContent += nCharMatched;
				}
				else
					itCharInContent += 1;

				itCharInToFind = itToFindEnd;
				nCharMatched = 0;
			}

		}

		//all charactor match
		if (nCharMatched == nToFindLen)
		{
			return itCharInContent - itContentBegin;
		}

		//string no found
		return string::npos;
	}

	size_t stringFind(
		const string& strContent,
		const string& strToFind,
		size_t nStartPos = 0)
	{
		size_t nToFindLen = strToFind.length();

		//no enough charactor to compare
		if (nStartPos + nToFindLen >= strContent.length())
		{
			return string::npos;
		}

		//iterator to begin of the strContent
		string::const_iterator itContentBegin = strContent.begin();
		//iterator to end of the strContent
		string::const_iterator itContentEnd = strContent.begin() + strContent.length() - 1;
		//iterator to last charactor of the strToFind
		//string::const_iterator itToFindEnd = strToFind.begin() + nToFindLen - 1;
		//iterator to first charactor of the strToFind
		string::const_iterator itToFindBegin = strToFind.begin();
		//iterator to current scan charactor in strToFind
		string::const_iterator itCharInToFind = itToFindBegin;
		//iterator to current scan charactor in strContent
		string::const_iterator itCharInContent = itContentBegin + nStartPos;

		//matched charactors number
		size_t nCharMatched = 0;

		//repeat charactor with first charactor in strToFind
		size_t nNoRepeatLen = 1;
		itCharInToFind++;
		while ((nNoRepeatLen != nToFindLen) && ((*itCharInToFind) != (*itToFindBegin)))
		{
			nNoRepeatLen++;
			itCharInToFind--;
		}
		itCharInToFind = itToFindBegin;

		//the for loop will not stop at itCharInContent == itContentEnd, because maybe (*itContentEnd) == (*itCharInToFind)
		for (; nCharMatched != nToFindLen; itCharInContent++)
		{
			//compare string
			while ((*itCharInContent) == (*itCharInToFind))
			{
				nCharMatched++;

				if ((itCharInContent == itContentEnd) || (nCharMatched == nToFindLen))
				{
					break;
				}

				itCharInContent++;
				itCharInToFind++;
			}

			//all charactor match, or no content to search
			if ((itCharInContent == itContentEnd) || (nCharMatched == nToFindLen))
			{
				break;
			}

			//only nCharMatched charactors match
			if (nCharMatched > 0)
			{
				//move back only when match char num large than no repeat char in strToFind
				if (nCharMatched > nNoRepeatLen)
				{
					itCharInContent -= nCharMatched;
				}
				else
					itCharInContent -= 1;

				itCharInToFind = itToFindBegin;
				nCharMatched = 0;
			}

		}

		//all charactor match
		if (nCharMatched == nToFindLen)
		{
			return itCharInContent - itContentBegin;
		}

		//string no found
		return string::npos;
	}
};





#endif
