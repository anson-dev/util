#ifndef JCE_UTIL_H
#define JCE_UTIL_H

namespace HuyaUtil
{
	
template<typename T>
string Serialize(const T& a)
{
    JceOutputStream<BufferWriter> os;
    a.writeTo(os);
    return string(os.getBuffer(), os.getLength());
}

template<typename T>
T DeSerialize(const string& str)
{
    T a;
    if (str.size() > 0)
    {
        JceInputStream<BufferReader> is;
        is.setBuffer(str.data(), str.size());
        a.readFrom(is);
    }
    return a;
}

template<typename T>
string SerializeV(const vector<T>& v)
{
    JceOutputStream<BufferWriter> os;
    if (v.size() > 0)
    {
        os.write(v, 1);
    }
    return string(os.getBuffer(), os.getLength());
}

template<typename T>
vector<T> DeSerializeV(string& str)
{
    vector<T> v;
    if (str.size() > 0)
    {
        JceInputStream<BufferReader> os;
        os.setBuffer(str.c_str(), str.size());
        os.read(v, 1);
    }
    return v;
}
}


#endif




