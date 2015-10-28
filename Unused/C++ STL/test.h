#include "Common.h"
#include <map>

typedef void (*_pfnHanders)(void);

typedef std::vector<int32>::iterator _i32VecIte;

typedef std::map<const string, _pfnHanders> CFuncProcMap;

