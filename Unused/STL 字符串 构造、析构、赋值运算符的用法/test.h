#include "Common.h"
#include <map>

class String
{
public:
	String(const char* pchSrc = NULL);
	String(const String& strOther);
	~String(void);
	String& operator=(const String& strOther);

public:
	char* m_pchData;
};


