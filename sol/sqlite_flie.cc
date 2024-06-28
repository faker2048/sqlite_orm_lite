#include "sol/sqlite_flie.h"

namespace sqliteol {

SqliteFile::SqliteFile(const std::filesystem::path& path) : path_(path) {
}

}  // namespace sqliteol