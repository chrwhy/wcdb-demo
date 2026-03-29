// WCDB macOS C++ Demo：WINQ 建表 + ORM 增删改查 + FTS5 拼音搜索

#include <WCDB/WCDBCpp.h>
#include "Sample.hpp"
#include "PinyinSample.hpp"
#include "ChineseFtsSample.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <map>
#include <vector>
#include <set>

static const char* kTableName = "sampleTable";
static const char* kPinyinFtsTableName = "pinyin_fts";
static const char* kChineseFtsTableName = "chinese_fts";

int main(int argc, const char* argv[]) {
    // 1. 数据库路径（默认当前工作目录下的 sample.db，使用绝对路径避免 WCDB 路径解析问题）
    std::string dbPath = "./sample.db";
    /*
    if (argc > 1) {
        dbPath = argv[1];
    } else {
        char cwd[4096];
        if (getcwd(cwd, sizeof(cwd))) {
            dbPath = std::string(cwd) + "/sample.db";
        }
    }
    */

    WCDB::Database database(dbPath);

    // 2. 使用 WINQ 建表（避免 ORM createTable 在纯 C++ 下的兼容问题）
    WCDB::StatementCreateTable createTable = WCDB::StatementCreateTable()
        .createTable(kTableName)
        .ifNotExists();
    createTable.define(WCDB::ColumnDef(WCDB::Column("identifier"), WCDB::ColumnType::Integer));
    createTable.define(WCDB::ColumnDef(WCDB::Column("content"), WCDB::ColumnType::Text));

    if (!database.execute(createTable)) {
        std::cerr << "create table failed";
        const WCDB::Error& err = database.getError();
        if (err.getDescription().size() > 0)
            std::cerr << ": " << err.getDescription();
        std::cerr << "\n";
        return 1;
    }
    std::cout << "table created: " << kTableName << "\n";

    // 3. 插入
    bool ok = database.insertObjects<Sample>(Sample(1, "hello"), kTableName);
    ok &= database.insertObjects<Sample>(Sample(2, "world"), kTableName);
    if (!ok) { std::cerr << "insert failed\n"; return 1; }
    std::cout << "inserted 2 rows\n";

    // 4. 查询全部
    auto all = database.getAllObjects<Sample>(kTableName);
    std::cout << "getAllObjects count: " << all->size() << "\n";
    for (const auto& obj : *all)
        std::cout << "  id=" << obj.identifier << " content=" << obj.content << "\n";

    // 5. 条件更新
    ok = database.updateObject(Sample(0, "updated"),
                               WCDB_FIELD(Sample::content),
                               kTableName,
                               WCDB_FIELD(Sample::identifier) > 0);
    if (!ok) { std::cerr << "update failed\n"; return 1; }
    std::cout << "updated rows where identifier > 0\n";

    // 6. 条件删除
    ok = database.deleteObjects(kTableName, WCDB_FIELD(Sample::identifier) == 1);
    if (!ok) { std::cerr << "delete failed\n"; return 1; }
    std::cout << "deleted row where identifier == 1\n";

    // 7. 再次查询
    all = database.getAllObjects<Sample>(kTableName);
    std::cout << "after delete, count: " << all->size() << "\n";
    for (const auto& obj : *all)
        std::cout << "  id=" << obj.identifier << " content=" << obj.content << "\n";

    // -------- FTS5 拼音搜索示例 --------
    std::cout << "\n--- FTS5 拼音搜索 ---\n";

    // 配置汉字 -> 拼音映射（仅示例用，实际可接入完整拼音库）
    static const std::map<std::string, std::vector<std::string>> kPinyinDict = {
        { "北", { "bei" } }, { "京", { "jing" } }, { "上", { "shang" } }, { "海", { "hai" } },
        { "测", { "ce" } },  { "试", { "shi" } },  { "内", { "nei" } },  { "容", { "rong" } },
        { "苹", { "ping" } }, { "果", { "guo" } }, { "树", { "shu" }}, { "哦", { "o" } }, { "欧", { "ou" } }, { "阳", { "yang" } } ,
    };
    WCDB::Database::configPinyinConverter([](const WCDB::UnsafeStringView& token) {
        std::string key(token.data(), token.size());
        auto it = kPinyinDict.find(key);
        if (it == kPinyinDict.end())
            return std::vector<WCDB::StringView>();
        std::vector<WCDB::StringView> result;
        for (const auto& p : it->second)
            result.push_back(WCDB::StringView(p));
        return result;
    });

    // 拼音表：需 Pinyin 分词器
    database.addTokenizer(WCDB::BuiltinTokenizer::Pinyin);
    if (!database.createVirtualTable<PinyinSample>(kPinyinFtsTableName)) {
        std::cerr << "create virtual table (pinyin fts) failed\n";
        return 1;
    }
    std::cout << "pinyin FTS table created: " << kPinyinFtsTableName << "\n";

    // 汉字表：需 Verbatim + SimplifyChinese；先配置 Symbol 检测（避免中文被当符号）
    WCDB::Database::configSymbolDetector([](WCDB::Database::UnicodeChar c) {
        if (c < 0xC0)
            return !(c >= 0x30 && c <= 0x39) && !((c >= 0x41 && c <= 0x5A) || (c >= 0x61 && c <= 0x7A));
        return false;
    });
    database.addTokenizer(WCDB::BuiltinTokenizer::Verbatim);
    if (!database.createVirtualTable<ChineseFtsSample>(kChineseFtsTableName)) {
        std::cerr << "create virtual table (chinese fts) failed\n";
        return 1;
    }
    std::cout << "chinese FTS table created: " << kChineseFtsTableName << "\n";

    // 同一条数据写入拼音表 + 汉字表，同时支持拼音与汉字搜索
    const std::vector<std::string> kFtsRows = { "北京", "哦阳", "欧阳", "上海", "测试内容", "苹果树","欧阳","哦阳" };
    for (const auto& s : kFtsRows) {
        database.insertObjects<PinyinSample>(PinyinSample(s), kPinyinFtsTableName);
        database.insertObjects<ChineseFtsSample>(ChineseFtsSample(s), kChineseFtsTableName);
    }
    std::cout << "inserted " << kFtsRows.size() << " rows into pinyin_fts + chinese_fts\n";

    // 统一搜索：先查拼音表再查汉字表，合并去重后返回
    auto searchBoth = [&database](const char* query) {
        std::set<std::string> merged;
        auto fromPinyin = database.getAllObjects<PinyinSample>(kPinyinFtsTableName,
            WCDB_FIELD(PinyinSample::content).match(query));
        if (fromPinyin.succeed())
            for (const auto& o : fromPinyin.value()) merged.insert(o.content);
        auto fromChinese = database.getAllObjects<ChineseFtsSample>(kChineseFtsTableName,
            WCDB_FIELD(ChineseFtsSample::content).match(query));
        if (fromChinese.succeed())
            for (const auto& o : fromChinese.value()) merged.insert(o.content);
        return merged;
    };

    auto show = [](const char* query, const std::set<std::string>& results) {
        std::cout << "  match \"" << query << "\": ";
        if (results.empty()) { std::cout << "(none)\n"; return; }
        bool first = true;
        for (const auto& c : results) { std::cout << (first ? "" : ", ") << c; first = false; }
        std::cout << "\n";
    };

    std::cout << "--- 拼音 + 汉字 统一搜索 ---\n";
    show("bei jing", searchBoth("bei jing"));
    show("shang hai", searchBoth("shang hai"));
    show("ce shi", searchBoth("ce shi"));
    show("ping guo", searchBoth("ping guo"));
    show("o yang", searchBoth("o yang"));
    show("o y", searchBoth("o y"));
    show("ou yang", searchBoth("ou yang"));
    show("北京", searchBoth("北京"));
    show("北", searchBoth("北"));
    show("上海", searchBoth("上海"));
    show("苹果", searchBoth("苹果"));
    show("测试", searchBoth("测试"));

    std::cout << "demo done. db file: " << dbPath << "\n";
    return 0;
}
