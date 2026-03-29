#!/usr/bin/env bash
# 编译并运行 wcdb-demo，每次运行前删除 sample 数据库（全新数据）

set -e
cd "$(dirname "$0")"

# WCDB 编译产物目录，与 CMakeLists.txt 默认一致；可通过环境变量覆盖
WCDB_BUILD_DIR="${WCDB_BUILD_DIR:-/Users/stephen/Lab/github/wcdb/src/build}"

echo "=== build ==="
mkdir -p build
cd build
cmake .. -DWCDB_BUILD_DIR="$WCDB_BUILD_DIR"
make -j4
cd ..

echo ""
echo "=== remove old sample db ==="
rm -f sample.db sample.db-shm sample.db-wal
rm -f build/sample.db build/sample.db-shm build/sample.db-wal

echo ""
echo "=== run ==="
export DYLD_FRAMEWORK_PATH="$WCDB_BUILD_DIR"
./build/wcdb-demo
