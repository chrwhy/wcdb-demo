#include <WCDB/WCDBCpp.h>
#include "Sample.hpp"

// WCDB C++ ORM 实现：声明 identifier、content 与表字段的映射
WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(Sample)
WCDB_CPP_SYNTHESIZE(identifier)
WCDB_CPP_SYNTHESIZE(content)
WCDB_CPP_ORM_IMPLEMENTATION_END

Sample::Sample() : identifier(0) {}

Sample::Sample(int id, const std::string& text) : identifier(id), content(text) {}
