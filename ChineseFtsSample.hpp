#ifndef ChineseFtsSample_hpp
#define ChineseFtsSample_hpp

#include <string>
#include <WCDB/WCDBCpp.h>

/// 用于 FTS5 汉字全文搜索的虚拟表模型（Verbatim + SimplifyChinese 分词）
class ChineseFtsSample {
public:
    ChineseFtsSample() = default;
    explicit ChineseFtsSample(const std::string& content) : content(content) {}

    std::string content;

    WCDB_CPP_ORM_DECLARATION(ChineseFtsSample)
};

#endif /* ChineseFtsSample_hpp */
