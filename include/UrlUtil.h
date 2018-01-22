#ifndef  URL_UTIL_H_
#define  URL_UTIL_H_
#include "TypeDefine.h"
#include <string>
#include <map>


typedef std::map<std::string, std::string>  UrlParam ;

std::string urlEncode(const std::string& szToEncode) 
{
	std::string src = szToEncode;  
    char hex[] = "0123456789ABCDEF";  
    std::string dst;  

    for (size_t i = 0; i < src.size(); ++i)  
    {  
        unsigned char cc = src[i];  

        if ( (cc >= 'A' && cc <= 'Z') || 
        	 (cc >='a' && cc <= 'z')  || 
        	 (cc >='0' && cc <= '9') || 
        	 (cc == '.')  || 
        	 (cc == '_')  || 
        	 (cc == '-' ) || 
        	 (cc == '*'))  
        {  
        	if (cc == ' ')  
            {  
                dst += "+";  
            }  
            else
            {  
                dst += cc; 
            } 
        }  
        else  
        {  
            unsigned char c = static_cast<unsigned char>(src[i]);  
            dst += '%';  
            dst += hex[c / 16];  
            dst += hex[c % 16];  
        }  
    }  

 	return dst; 
}

 bool buildUrlParam(const std::map<std::string, std::string>& param, std::string& urlparam)
 {
    if (param.empty())
        return false;
    
    std::string sTmp ;
    std::map<std::string, std::string>::const_iterator iter = param.begin();
    for (; iter != param.end(); ++iter)
    {
        if (!sTmp.empty())
            sTmp.append("&");
        sTmp.append(iter->first + "=" + iter->second);
    }

    urlparam = sTmp;

    return true ;
 }

#endif
