# WCDB macOS C++ Demo（纯 C++）

基于 [Tencent/WCDB](https://github.com/Tencent/wcdb) 的**纯 C++** 示例，在 macOS 上演示：

- **建表**：使用 WINQ `StatementCreateTable` + `execute`（与官方 C++ 测试中的 createValueTable 方式一致，避免 ORM `createTable` 在纯 C++ 环境下的兼容问题）
- **增删改查**：使用 ORM（`Sample` 模型 + `WCDB_FIELD` 条件）

数据库文件默认使用**当前工作目录的绝对路径**下的 `sample.db`（避免 macOS 上相对路径导致的 “Empty directory” 问题），也可通过命令行参数指定路径。

## 环境要求

- macOS 10.13+
- C++14
- CMake 3.10+
- 已编译的 WCDB（见下方「编译 WCDB」）

## 一、编译 WCDB

在任意目录执行：

```bash
git clone https://github.com/Tencent/wcdb.git
cd wcdb
git submodule update --init sqlcipher zstd
cd src
mkdir build && cd build
cmake .. && cmake --build . -j8
```

编译完成后，在 `wcdb/src/build` 下会得到 `WCDB.framework`。

## 二、编译并运行 Demo

假设 WCDB 源码在 `~/wcdb`，则：

```bash
cd wcdb-demo
mkdir build && cd build
cmake .. -DWCDB_BUILD_DIR=~/wcdb/src/build
cmake --build .
./wcdb-demo
```

数据库文件会生成在**运行时的当前目录**（例如在 `build` 下执行则生成 `build/sample.db`）。若希望数据库在工程根目录，可从根目录运行：

```bash
cd wcdb-demo
./build/wcdb-demo
```

指定数据库路径：

```bash
./wcdb-demo /tmp/mydb.db
```

若运行时提示找不到 `WCDB.framework`，可先设置再运行：

```bash
export DYLD_FRAMEWORK_PATH=~/wcdb/src/build
./wcdb-demo
```

## 三、Demo 说明

- **Sample**：模型类，字段 `identifier`(int)、`content`(string)，对应表 `sampleTable`。
- **流程**：WINQ 建表 → 插入两条记录 → 查询全部 → 按条件更新 → 按条件删除 → 再次查询验证。
- **FTS5 拼音搜索**：使用虚拟表 `pinyin_fts`（`PinyinSample` 模型），先 `configPinyinConverter` 配置汉字→拼音映射，`addTokenizer(Pinyin)`、`createVirtualTable` 建表，插入中文后可用拼音或拼音首字母做 `.match("bei jing")`、`.match("ce shi")` 等全文搜索。

建表使用 WINQ 的 `StatementCreateTable` + `ColumnDef`，增删改查使用 WCDB C++ ORM 与 `WCDB_FIELD`；拼音搜索使用 `WCDB::BuiltinTokenizer::Pinyin` 与 `WCDB_FIELD(PinyinSample::content).match(query)`，无需手写 SQL。
