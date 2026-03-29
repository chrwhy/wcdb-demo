#ifndef PinyinSample_hpp
#define PinyinSample_hpp

#include <string>
#include <WCDB/WCDBCpp.h>

/// 用于 FTS5 拼音搜索的虚拟表模型，仅一个 content 字段
class PinyinSample {
public:
    PinyinSample() = default;
    explicit PinyinSample(const std::string& content) : content(content) {}

    std::string content;

    WCDB_CPP_ORM_DECLARATION(PinyinSample)
};

#endif /* PinyinSample_hpp */
