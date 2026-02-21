#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include "Transform.h"
#include <ImGui.h>

using json = nlohmann::json;

//-----------------------------------------
//          Transform読み書き処理
//-----------------------------------------

inline void to_json(json& j, const NVector3& v) {
    j = json::array({ v.x, v.y, v.z });
}
inline void from_json(const json& j, NVector3& v) {
    v.x = j.at(0).get<float>();
    v.y = j.at(1).get<float>();
    v.z = j.at(2).get<float>();
}

inline void to_json(json& j, const Transform& t) {
    j = json{
        {"pos", t.m_Position},
        {"rot", t.m_Rotation},
        {"scale", t.m_Scale}
    };
}

inline void from_json(const json& j, Transform& t) {
    t.m_Position = j.value("pos",   NVector3{ 0,0,0 });
    t.m_Rotation = j.value("rot",   NVector3{ 0,0,0 });
    t.m_Scale    = j.value("scale", NVector3{ 1,1,1 });
}

//-----------------------------------------
//      Windows・Audio・Objectコンフィグ
//-----------------------------------------
struct WindowConfig {
    int width = 1280;
    int height = 720;
    bool fullscreen = false;
};

struct ObjectConfig {
    std::string id;
    std::string prefab;
    Transform transform;
};

struct AppConfig {
    WindowConfig window;
    std::vector<ObjectConfig> objects;
};

// JSON化
inline void to_json(json& j, const WindowConfig& w) {
    j = json{ {"width", w.width}, {"height", w.height}, {"fullscreen", w.fullscreen} };
}
inline void from_json(const json& j, WindowConfig& w) {
    w.width = j.value("width", 1280);
    w.height = j.value("height", 720);
    w.fullscreen = j.value("fullscreen", false);
}

inline void to_json(json& j, const ObjectConfig& o) {
    j = json{ {"id", o.id}, {"prefab", o.prefab}, {"transform", o.transform} };
}
inline void from_json(const json& j, ObjectConfig& o) {
    o.id = j.value("id", "");
    o.prefab = j.value("prefab", "");
    o.transform = j.value("transform", Transform{});
}

inline void to_json(json& j, const AppConfig& c) {
    j = json{ {"window", c.window}, {"objects", c.objects} };
}
inline void from_json(const json& j, AppConfig& c) {
    c.window = j.value("window", WindowConfig{});
    c.objects = j.value("objects", std::vector<ObjectConfig>{});
}

class ConfigIO {
public:
    // Load（読み込み）
    static bool Load(const std::filesystem::path& path, AppConfig& out) {
        std::ifstream ifs(path);
        if (!ifs) return false;
        json j; ifs >> j;
        out = j.get<AppConfig>();
        return true;
    }

    // Save（保存）
    static bool SaveAtomic(const std::filesystem::path& path, const AppConfig& cfg) {
        const auto tmp = path.string() + ".tmp";
        {
            std::ofstream ofs(tmp, std::ios::trunc);
            if (!ofs) return false;
            json j = cfg;
            ofs << j.dump(2); // 2=見やすい整形
        }
        std::error_code ec;
        std::filesystem::rename(tmp, path, ec);
        if (ec) {
            // 既存があると rename 失敗する環境もある → removeして再rename等に分岐してもOK
            return false;
        }
        return true;
    }
};

//-----------------------------------------
//          ImGui読み書きの登録
//-----------------------------------------
bool EditTransformImGui(const char* label, Transform& t) {
    bool changed = false;
    if (ImGui::TreeNode(label)) {
        changed |= ImGui::DragFloat3("Pos", &t.m_Position.x, 0.1f);
        changed |= ImGui::DragFloat3("Rot", &t.m_Rotation.x, 0.5f);
        changed |= ImGui::DragFloat3("Scale", &t.m_Scale.x , 0.01f, 0.001f, 100.0f);
        ImGui::TreePop();
    }
    return changed;
}

struct LiveEditSession {
    AppConfig config;       // 最終的に保存するやつ
    bool dirty = false;

    // 例：IDで参照して編集する（実際はオブジェクト実体と紐付け）
    Transform& GetTransformById(const std::string& id) {
        for (auto& o : config.objects) if (o.id == id) return o.transform;
        static Transform dummy{};
        return dummy;
    }
};

void DrawConfigEditor(LiveEditSession& s, const std::filesystem::path& path) {
    ImGui::Text("Config: %s", path.string().c_str());

    // Window
    if (ImGui::TreeNode("Window")) {
        s.dirty |= ImGui::InputInt("Width", &s.config.window.width);
        s.dirty |= ImGui::InputInt("Height", &s.config.window.height);
        s.dirty |= ImGui::Checkbox("Fullscreen", &s.config.window.fullscreen);
        ImGui::TreePop();
    }

    // Objects
    if (ImGui::TreeNode("Objects")) {
        for (auto& obj : s.config.objects) {
            if (ImGui::TreeNode(obj.id.c_str())) {
                s.dirty |= EditTransformImGui("Transform", obj.transform);
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    if (s.dirty) ImGui::TextColored(ImVec4(1, 0.6f, 0.2f, 1), "Modified (not saved)");

    if (ImGui::Button("Save")) {
        if (ConfigIO::SaveAtomic(path, s.config)) {
            s.dirty = false;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Reload")) {
        AppConfig tmp;
        if (ConfigIO::Load(path, tmp)) {
            s.config = std::move(tmp);
            s.dirty = false;
        }
    }
}