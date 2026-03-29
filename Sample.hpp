#ifndef Sample_hpp
#define Sample_hpp

#include <string>
#include <WCDB/WCDBCpp.h>

/// 与数据库表 sampleTable 绑定的模型类
class Sample {
public:
    Sample();  // WCDB ORM 要求必须有默认构造
    Sample(int identifier, const std::string& content);

    int identifier;
    std::string content;

    // WCDB C++ ORM 声明：将本类绑定到表字段
    WCDB_CPP_ORM_DECLARATION(Sample)
};

#endif /* Sample_hpp */
