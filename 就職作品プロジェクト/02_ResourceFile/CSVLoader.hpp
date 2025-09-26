#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <cctype>

// 簡単・安全に CSV を読み込むためのヘッダオンリー（RFC4180 準拠の基本）
// - 区切り文字はデフォルト ',' だが、'\t' などにも変更可能
// - ダブルクオートで囲まれたフィールド内の区切り文字や改行に対応
// - エスケープは "" -> " に対応
// - UTF-8 BOM を自動除去
// - CRLF / LF どちらの改行でもOK
// - 失敗時は例外（std::runtime_error）を投げる
//
// 使い方：
//   #include "Csv.hpp"
//   auto [header, rows] = csv::ReadAll(u8"data/sample.csv");
//   // header はヘッダ行（存在しなければ空）、rows はデータ本体
//
//   // 行ごとにアクセス
//   for (const auto& r : rows) {
//       // r は std::vector<std::string> で各セル文字列
//   }
//
//   // 区切り文字を変更したい場合：
//   auto [h2, r2] = csv::ReadAll(path, /*delimiter=*/'\t', /*has_header=*/true);
//
namespace csv {

    struct Table {
        std::vector<std::string> header;                 // 先頭行（has_header=true の時のみ格納）
        std::vector<std::vector<std::string>> rows;      // データ本体
    };

    namespace detail {

        inline bool StartsWithBOM(std::string_view s) {
            // UTF-8 BOM: 0xEF,0xBB,0xBF
            return s.size() >= 3 && static_cast<unsigned char>(s[0]) == 0xEF
                && static_cast<unsigned char>(s[1]) == 0xBB
                && static_cast<unsigned char>(s[2]) == 0xBF;
        }

        inline void StripCR(std::string& line) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
        }

        // 1行を CSV のルールで分解する
        inline std::vector<std::string> ParseCsvLine(std::string_view line, char delimiter) {
            std::vector<std::string> out;
            std::string cur;
            cur.reserve(line.size());

            const char quote = '"';
            bool in_quotes = false;

            for (size_t i = 0; i < line.size(); ++i) {
                char c = line[i];
                if (in_quotes) {
                    if (c == quote) {
                        // 連続する "" はエスケープ -> " 1文字として追加
                        if (i + 1 < line.size() && line[i + 1] == quote) {
                            cur.push_back(quote);
                            ++i; // 次も消費
                        }
                        else {
                            in_quotes = false; // クオート閉じ
                        }
                    }
                    else {
                        cur.push_back(c);
                    }
                }
                else {
                    if (c == delimiter) {
                        out.emplace_back(std::move(cur));
                        cur.clear();
                    }
                    else if (c == quote) {
                        in_quotes = true;
                    }
                    else {
                        cur.push_back(c);
                    }
                }
            }
            out.emplace_back(std::move(cur));
            return out;
        }

    } // namespace detail

    // ファイル全体を読み込む（ヘッダ行の有無を指定できる）
    inline Table ReadAll(const std::filesystem::path& path,
        char delimiter = ',',
        bool has_header = true)
    {
        std::ifstream ifs(path, std::ios::binary);
        if (!ifs) {
            throw std::runtime_error("csv::ReadAll: ファイルを開けませんでした: " + path.string());
        }

        Table tbl{};
        std::string line;
        bool first_line = true;

        while (std::getline(ifs, line)) {
            detail::StripCR(line); // CRLF -> LF

            if (first_line) {
                // BOM 除去
                if (detail::StartsWithBOM(line)) {
                    line.erase(0, 3);
                }
            }

            auto fields = detail::ParseCsvLine(line, delimiter);

            if (first_line && has_header) {
                tbl.header = std::move(fields);
            }
            else {
                tbl.rows.emplace_back(std::move(fields));
            }

            first_line = false;
        }

        if (ifs.bad()) {
            throw std::runtime_error("csv::ReadAll: 読み取り中にエラーが発生しました");
        }

        return tbl;
    }

    // セル文字列を任意型へ変換する補助（失敗時は例外）
    // 例: auto v = csv::To<int>(row[0]);

    template <class T>
    T To(const std::string& s);

    template <>
    inline int To<int>(const std::string& s) {
        size_t idx = 0; int v = std::stoi(s, &idx); if (idx != s.size()) throw std::runtime_error("csv::To<int>: 余剰文字: " + s); return v;
    }

    template <>
    inline long To<long>(const std::string& s) {
        size_t idx = 0; long v = std::stol(s, &idx); if (idx != s.size()) throw std::runtime_error("csv::To<long>: 余剰文字: " + s); return v;
    }

    template <>
    inline long long To<long long>(const std::string& s) {
        size_t idx = 0; long long v = std::stoll(s, &idx); if (idx != s.size()) throw std::runtime_error("csv::To<long long>: 余剰文字: " + s); return v;
    }

    template <>
    inline float To<float>(const std::string& s) {
        size_t idx = 0; float v = std::stof(s, &idx); if (idx != s.size()) throw std::runtime_error("csv::To<float>: 余剰文字: " + s); return v;
    }

    template <>
    inline double To<double>(const std::string& s) {
        size_t idx = 0; double v = std::stod(s, &idx); if (idx != s.size()) throw std::runtime_error("csv::To<double>: 余剰文字: " + s); return v;
    }

    template <>
    inline std::string To<std::string>(const std::string& s) { return s; }

    // 行を構造体に変換するユーティリティ
    // 例：
    //   struct Enemy { int id; float x, y; std::string name; };
    //   auto makeEnemy = [](const std::vector<std::string>& r){
    //       if (r.size() < 4) throw std::runtime_error("列数不足");
    //       return Enemy{ csv::To<int>(r[0]), csv::To<float>(r[1]), csv::To<float>(r[2]), r[3] };
    //   };
    //   std::vector<Enemy> enemies = csv::MapRows<Enemy>(tbl.rows, makeEnemy);

    template <class T, class F>
    std::vector<T> MapRows(const std::vector<std::vector<std::string>>& rows, F&& f) {
        std::vector<T> out;
        out.reserve(rows.size());
        for (const auto& r : rows) out.emplace_back(f(r));
        return out;
    }

} // namespace csv
