#pragma once
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <locale>

struct TransformRow {
    std::string id;
    float x = 0.0f, y = 0.0f, z = 0.0f;
    float yaw = NAN, pitch = NAN, roll = NAN, fov = NAN; // 未指定はNaN
};

struct SaveSnapShot {
    int version = 1;
    int sceneId = 0;
    int rngSeed = 0;
    std::vector<TransformRow> rows;
};

// ← ここは "struct" を付けない（ヘッダ配置なら inline 推奨）
inline std::filesystem::path SaveDir() {
    auto dir = std::filesystem::current_path() / "Save";
    std::filesystem::create_directories(dir);
    return dir;
}

inline void writeUtf8BOM(std::ofstream& ofs) {
    const unsigned char bom[3] = { 0xEF, 0xBB, 0xBF };
    ofs.write(reinterpret_cast<const char*>(bom), 3);
}

// CSV用エスケープ（RFC4180準拠）
inline std::string csvEscape(std::string_view s) {
    const bool needQuotes = (s.find_first_of(",\"\n\r") != std::string_view::npos);
    if (!needQuotes) return std::string{ s };
    std::string out; out.reserve(s.size() + 2);
    out.push_back('"');
    for (char c : s) out += (c == '"') ? "\"\"" : std::string(1, c);
    out.push_back('"');
    return out;
}

// NaNなら空欄、そうでなければ固定少数で文字列化
inline std::string f2s(float v, int precision = 6) {
    if (std::isnan(v)) return "";
    std::ostringstream oss;
    oss.imbue(std::locale::classic());        // 小数点は常に '.'
    oss.setf(std::ios::fixed);
    oss << std::setprecision(precision) << v;
    return oss.str();
}

// スナップショットをCSV保存
inline bool SaveToCSV(const SaveSnapShot& snap, const std::filesystem::path& filePath) {
    // Windowsでも改行を制御できるようにbinaryで開く
    std::ofstream ofs(filePath, std::ios::binary);
    if (!ofs) return false;

    writeUtf8BOM(ofs);

    // メタ情報（必要なければ削除可）
    ofs << "#version," << snap.version << "\r\n";
    ofs << "#sceneId," << snap.sceneId << "\r\n";
    ofs << "#rngSeed," << snap.rngSeed << "\r\n";

    // ヘッダー
    ofs << "id,x,y,z,yaw,pitch,roll,fov\r\n";

    // 本体
    for (const auto& r : snap.rows) {
        ofs << csvEscape(r.id) << ','
            << f2s(r.x) << ','
            << f2s(r.y) << ','
            << f2s(r.z) << ','
            << f2s(r.yaw) << ','
            << f2s(r.pitch) << ','
            << f2s(r.roll) << ','
            << f2s(r.fov) << "\r\n";
    }
    return true; // ofstreamのデストラクタで閉じられる
}