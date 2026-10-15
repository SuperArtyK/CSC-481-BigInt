// Interactive, one-shot CMake + Ninja project generator.
// C++17, standard library only. No TOML, persistent metadata, or CLI flags.
// Inspired by create_cmake_project.cpp and preset_generator.cpp.
// v3: optional CTest / GoogleTest discovery, with separate playground and test executables.
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <functional>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace fs = std::filesystem;
using std::string;
using std::vector;

struct Json {
    enum class Kind { String, Bool, Number, Array, Object } kind;
    string text;
    bool flag = false;
    int number = 0;
    vector<Json> items;
    vector<std::pair<string, Json>> fields;
    explicit Json(Kind k) : kind(k) {}
    static Json str(string s) { Json j{Kind::String}; j.text = std::move(s); return j; }
    static Json boolean(bool b) { Json j{Kind::Bool}; j.flag = b; return j; }
    static Json integer(int n) { Json j{Kind::Number}; j.number = n; return j; }
    static Json array() { return Json{Kind::Array}; }
    static Json object() { return Json{Kind::Object}; }
    void add(const string& key, Json value) { fields.emplace_back(key, std::move(value)); }
    void push(Json value) { items.push_back(std::move(value)); }
    static string escape(const string& s) {
        std::ostringstream o;
        const char* hex = "0123456789abcdef";
        for (unsigned char c : s) {
            switch (c) {
                case '\\': o << "\\\\"; break;
                case '"': o << "\\\""; break;
                case '\n': o << "\\n"; break;
                case '\r': o << "\\r"; break;
                case '\t': o << "\\t"; break;
                default:
                    if (c < 32) o << "\\u00" << hex[c >> 4] << hex[c & 15];
                    else o << static_cast<char>(c);
            }
        }
        return o.str();
    }
    void write(std::ostream& out, int depth = 0) const {
        auto indent = [&out](int d) { for (int i=0; i<d; ++i) out << ' '; };
        switch (kind) {
            case Kind::String: out << '"' << escape(text) << '"'; break;
            case Kind::Bool: out << (flag ? "true" : "false"); break;
            case Kind::Number: out << number; break;
            case Kind::Array:
                out << '[';
                if (!items.empty()) out << '\n';
                for (size_t i=0; i<items.size(); ++i) {
                    indent(depth+2); items[i].write(out, depth+2);
                    if (i+1 != items.size()) out << ',';
                    out << '\n';
                }
                if (!items.empty()) indent(depth);
                out << ']'; break;
            case Kind::Object:
                out << '{';
                if (!fields.empty()) out << '\n';
                for (size_t i=0; i<fields.size(); ++i) {
                    indent(depth+2);
                    out << '"' << escape(fields[i].first) << "\": ";
                    fields[i].second.write(out, depth+2);
                    if (i+1 != fields.size()) out << ',';
                    out << '\n';
                }
                if (!fields.empty()) indent(depth);
                out << '}'; break;
        }
    }
    string dump() const { std::ostringstream o; write(o); o << '\n'; return o.str(); }
};

string trim(string s) {
    auto ws = [](unsigned char c){ return std::isspace(c) != 0; };
    while (!s.empty() && ws(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
    while (!s.empty() && ws(static_cast<unsigned char>(s.back()))) s.pop_back();
    return s;
}
string lower(string s) {
    for (char& c: s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}
string identifier(const string& raw) {
    string out;
    bool gap = false;
    bool useful = false;
    for (unsigned char ch: raw) {
        bool letter = (ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z');
        bool digit = ch>='0'&&ch<='9';
        if (letter || digit || ch=='_') {
            if (gap && !out.empty() && out.back()!='_') out += '_';
            out += static_cast<char>(ch);
            useful |= letter || digit;
            gap = false;
        } else gap = true;
    }
    if (!useful) throw std::runtime_error("Name cannot be converted to a meaningful identifier: '" + raw + "'");
    if (out.empty()) throw std::runtime_error("Empty identifier");
    if (out[0]>='0' && out[0]<='9') out.insert(out.begin(), '_');
    if (out != raw) std::cerr << "Warning: name sanitized: '" << raw << "' -> '" << out << "'\n";
    return out;
}

string prompt(const string& question, const string& fallback = "", bool allowEmpty = false) {
    for (;;) {
        std::cout << question;
        if (!fallback.empty()) std::cout << " [" << fallback << "]";
        std::cout << ": " << std::flush;
        string response;
        if (!std::getline(std::cin, response)) throw std::runtime_error("Input ended before setup completed");
        response = trim(response);
        if (response.empty()) response = fallback;
        if (!response.empty() || allowEmpty) return response;
        std::cout << "  A value is required.\n";
    }
}
bool confirm(const string& question, bool fallback) {
    for (;;) {
        string response = lower(prompt(question + (fallback ? " (Y/n)" : " (y/N)"), "", true));
        if (response.empty()) return fallback;
        if (response=="y" || response=="yes") return true;
        if (response=="n" || response=="no") return false;
        std::cout << "  Please answer yes or no.\n";
    }
}
int promptCount(const string& question) {
    for (;;) {
        string s = prompt(question, "0");
        if (s.empty() || !std::all_of(s.begin(), s.end(), [](unsigned char c){return std::isdigit(c)!=0;})) {
            std::cout << "  Enter a number from 0 to 50.\n"; continue;
        }
        try { int n = std::stoi(s); if (n >= 0 && n <= 50) return n; } catch (...) {}
        std::cout << "  Enter a number from 0 to 50.\n";
    }
}
vector<string> splitComma(const string& input) {
    vector<string> out;
    std::istringstream in(input);
    string part;
    while (std::getline(in, part, ',')) { part = trim(part); if (!part.empty()) out.push_back(part); }
    return out;
}
string join(const vector<string>& strings, const string& delimiter) {
    string result;
    for (const auto& s : strings) { if (!result.empty()) result += delimiter; result += s; }
    return result;
}
string segment(const string& raw) {
    if (raw.empty() || raw=="." || raw==".." || raw.find('/')!=string::npos ||
        raw.find('\\')!=string::npos || raw.find(':')!=string::npos ||
        raw.find('"')!=string::npos || raw.find(';')!=string::npos ||
        raw.find('$')!=string::npos || raw.find('\n')!=string::npos)
        throw std::runtime_error("Invalid directory/path component: '" + raw + "'");
    return raw;
}
string cmakeQuoted(const string& s) {
    string out = "\"";
    for (char ch: s) {
        if (ch=='\\' || ch=='"' || ch=='$' || ch==';') out += '\\';
        out += ch;
    }
    return out + '"';
}

struct Module {
    string id;
    string folder;
    string pathCode;
    bool main = false;
    vector<string> publicDeps, privateDeps;
};
struct Profile {
    string suffix;
    string label;
    string buildType;  // "auto" = leave unset, "empty" = set to empty string
    string compileFlags;
    string linkFlags;
    bool ipo = false;
    bool tunedOnlyDefaultArch = false;
    Profile() = default;
    Profile(string s, string l, string type, string cf="", string lf="", bool enableIpo=false, bool nativeOnly=false)
        : suffix(std::move(s)), label(std::move(l)), buildType(std::move(type)),
          compileFlags(std::move(cf)), linkFlags(std::move(lf)), ipo(enableIpo),
          tunedOnlyDefaultArch(nativeOnly) {}
};
struct Compiler {
    string id;
    string command;
    string extraCompile;
    string extraLink;
    bool msvc = false;
    vector<string> architectures;
    vector<Profile> profiles;
};
string combined(const vector<string>& pieces) {
    string r;
    for (const auto& s : pieces) {
        if (trim(s).empty()) continue;
        if (!r.empty()) r += ' ';
        r += trim(s);
    }
    return r;
}

vector<string> askDependencies(const Module& module, const vector<Module>& modules,
                               bool isPublic, const string& defaultText = "") {
    for (;;) {
        string label = isPublic ? "  Public (transitive) dependencies" : "  Playground-only dependencies";
        string s = prompt(label + " (comma-separated IDs; blank = none)", defaultText, true);
        vector<string> proposed = splitComma(s), unique;
        std::set<string> seen;
        bool valid = true;
        for (const string& id: proposed) {
            auto it = std::find_if(modules.begin(), modules.end(), [&](const Module& m){return m.id==id;});
            if (it==modules.end() || it->main || it->id==module.id) {
                std::cout << "  Invalid dependency '" << id << "' (must be another non-main module).\n";
                valid = false; break;
            }
            if (!seen.insert(id).second) std::cerr << "Warning: duplicate dependency '" << id << "' ignored.\n";
            else unique.push_back(id);
        }
        if (valid) return unique;
    }
}
vector<size_t> dependencyOrder(const vector<Module>& modules) {
    std::map<string, size_t> ids;
    for (size_t i=0; i<modules.size(); ++i) ids[modules[i].id] = i;
    vector<int> color(modules.size(), 0);
    vector<size_t> order;
    std::function<void(size_t)> visit = [&](size_t i) {
        if (color[i]==1) throw std::runtime_error("Dependency cycle detected involving " + modules[i].id);
        if (color[i]==2) return;
        color[i]=1;
        for (const auto& name: modules[i].publicDeps) visit(ids.at(name));
        for (const auto& name: modules[i].privateDeps) visit(ids.at(name));
        color[i]=2;
        order.push_back(i);
    };
    for (size_t i=0; i<modules.size(); ++i) visit(i);
    return order;
}

string moduleTestsCMake() {
    return R"CMAKE(# Shared GoogleTest registration for one-shot generated projects.
# The function is called from each module's CMakeLists.txt.
# A module's tests/ tree is optional: no sources means no test executable.
function(add_module_tests module_name sources_target)
    if(NOT BUILD_TESTING)
        return()
    endif()

    file(GLOB_RECURSE test_sources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cxx"
    )
    if(NOT test_sources)
        return()
    endif()

    # Defer fetching GoogleTest until at least one test source exists.
    # This lets newly generated empty projects configure without network access.
    if(NOT TARGET GTest::gtest_main)
        FetchContent_MakeAvailable(googletest)
    endif()

    set(test_target "${module_name}_tests")
    add_executable(${test_target} ${test_sources})
    target_link_libraries(${test_target} PRIVATE
        ${sources_target}
        GTest::gtest_main
    )
    gtest_discover_tests(${test_target}
        TEST_PREFIX "${module_name}."
        PROPERTIES
            LABELS "${module_name}"
            TIMEOUT 60
    )
endfunction()
)CMAKE";
}

string moduleCMake(const Module& m, bool withTests) {
    std::ostringstream o;
    o << "# Generated once. You may edit this CMakeLists.txt freely.\n";
    o << "# src/ is inherited (except from the main project); private/ is executable-only.\n";
    if (withTests) o << "# tests/ files build into a separate GoogleTest executable, never the playground.\n";
    o << '\n';
    // With testing enabled, main also has an internal source interface for its test runner.
    // Other modules cannot depend on main, as enforced during interactive setup.
    if (!m.main || withTests) {
        o << "add_library(" << m.id << "_sources INTERFACE)\n";
        o << "target_include_directories(" << m.id << "_sources INTERFACE\n"
          << "    \"${CMAKE_CURRENT_SOURCE_DIR}/include\"\n)\n\n";
        o << "file(GLOB_RECURSE _module_public CONFIGURE_DEPENDS\n"
          << "    \"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp\"\n"
          << "    \"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc\"\n"
          << "    \"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx\"\n)\n";
        o << "target_sources(" << m.id << "_sources INTERFACE ${_module_public})\n";
        // Main's dependencies are normally playground-only. For tests, main's own
        // implementation needs them as well; they are private to this project's consumers.
        const auto& inherited = m.main ? m.privateDeps : m.publicDeps;
        if (!inherited.empty()) {
            o << "target_link_libraries(" << m.id << "_sources INTERFACE";
            for (const auto& dep : inherited) o << " " << dep << "_sources";
            o << ")\n";
        }
        o << '\n';
    }
    o << "file(GLOB_RECURSE _module_private CONFIGURE_DEPENDS\n"
      << "    \"${CMAKE_CURRENT_SOURCE_DIR}/private/*.cpp\"\n"
      << "    \"${CMAKE_CURRENT_SOURCE_DIR}/private/*.cc\"\n"
      << "    \"${CMAKE_CURRENT_SOURCE_DIR}/private/*.cxx\"\n)\n";
    if (m.main && !withTests) {
        o << "file(GLOB_RECURSE _module_public CONFIGURE_DEPENDS\n"
          << "    \"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp\"\n"
          << "    \"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc\"\n"
          << "    \"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx\"\n)\n";
        o << "add_executable(" << m.id << " main.cpp ${_module_public} ${_module_private})\n";
        o << "target_include_directories(" << m.id << " PRIVATE \"${CMAKE_CURRENT_SOURCE_DIR}/include\")\n";
    } else {
        o << "add_executable(" << m.id << " main.cpp ${_module_private})\n";
        o << "target_link_libraries(" << m.id << " PRIVATE " << m.id << "_sources)\n";
    }
    // Non-main local-only dependencies never leak into exported source interfaces.
    // Main's dependencies already reach both its executable and tests via main_sources.
    if (!m.main && !m.privateDeps.empty()) {
        o << "target_link_libraries(" << m.id << " PRIVATE";
        for (const auto& dep : m.privateDeps) o << " " << dep << "_sources";
        o << ")\n";
    } else if (m.main && !withTests && !m.privateDeps.empty()) {
        o << "target_link_libraries(" << m.id << " PRIVATE";
        for (const auto& dep : m.privateDeps) o << " " << dep << "_sources";
        o << ")\n";
    }
    if (withTests) {
        o << "\nif(BUILD_TESTING)\n"
          << "    add_module_tests(" << m.id << " " << m.id << "_sources)\n"
          << "endif()\n";
    }
    return o.str();
}
string rootCMake(const string& projectId, int standard, const vector<Module>& modules,
                 const vector<size_t>& order, bool withTests) {
    std::ostringstream o;
    o << "cmake_minimum_required(VERSION 3.23)\n";
    o << "project(" << projectId << " LANGUAGES CXX)\n\n";
    o << "set(CMAKE_CXX_STANDARD " << standard << ")\n";
    o << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n";
    o << "set(CMAKE_CXX_EXTENSIONS OFF)\n\n";
    if (withTests) {
        o << "# CTest and GoogleTest are optional at configure time.\n"
          << "include(CTest)\n\n"
          << "if(BUILD_TESTING)\n"
          << "    include(FetchContent)\n"
          << "    FetchContent_Declare(googletest\n"
          << "        GIT_REPOSITORY https://github.com/google/googletest.git\n"
          << "        # GoogleTest v1.18.0, pinned to its immutable release commit.\n"
          << "        GIT_TAG 063de7e9578f82b369302001269680b4b1553359\n"
          << "    )\n"
          << "    set(gtest_force_shared_crt ON CACHE BOOL \"\" FORCE)\n"
          << "    include(GoogleTest)\n"
          << "    include(\"${CMAKE_CURRENT_SOURCE_DIR}/cmake/ModuleTests.cmake\")\n"
          << "endif()\n\n";
    }
    o << "# Modules are registered in dependency order.\n";
    for (size_t index: order) o << "add_subdirectory(" << cmakeQuoted(modules[index].folder) << ")\n";
    o << "\n# Add any project-specific CMake customization below.\n";
    return o.str();
}
string mainCpp(const Module& m) {
    return "#include <iostream>\n\nint main() {\n    std::cout << \"Hello from " +
           Json::escape(m.id) + "!\\n\";\n    return 0;\n}\n";
}

vector<Profile> offeredProfiles(bool msvc, bool gcc) {
    vector<Profile> r = {
        {"debug", "Debug", "Debug"},
        {"relwithdebinfo", "RelWithDebInfo", "RelWithDebInfo"},
        {"release", "Release", "Release"},
        {"release-lto", "Release + LTO/IPO", "Release", "", "", true}
    };
    if (!msvc) {
        r.push_back({"release-tuned", "Release tuned for this CPU", "Release", "-march=native -mtune=native", "", false, true});
        r.push_back({"debug-asan", "Debug + AddressSanitizer", "Debug", "-fsanitize=address -fno-omit-frame-pointer", "-fsanitize=address"});
    }
    if (gcc) r.push_back({"debug-analyzer", "Debug + GCC static analyzer", "Debug", "-fanalyzer"});
    return r;
}
vector<Profile> selectProfiles(const Compiler& compiler) {
    bool gcc = compiler.id=="gcc";
    vector<Profile> available = offeredProfiles(compiler.msvc, gcc);
    for (;;) {
        std::cout << "\nProfiles for " << compiler.id << ":\n";
        for (size_t i=0; i<available.size(); ++i)
            std::cout << "  " << i+1 << ") " << available[i].label << '\n';
        std::cout << "  (MSVC sanitizer/debug combinations are intentionally not auto-generated.)\n";
        string all;
        for (size_t i=0; i<available.size(); ++i) { if (!all.empty()) all+=','; all+=std::to_string(i+1); }
        string choice = prompt("Profiles to generate (numbers comma-separated, 'all', or 'none')", "all");
        vector<Profile> result;
        std::set<int> seen;
        bool valid = true;
        if (lower(choice)=="all") choice = all;
        if (lower(choice)=="none") choice.clear();
        for (const auto& token: splitComma(choice)) {
            try {
                size_t used = 0;
                int selected = std::stoi(token, &used);
                if (used != token.size() || selected < 1 || selected > static_cast<int>(available.size())) { valid=false; break; }
                if (seen.insert(selected).second) result.push_back(available[static_cast<size_t>(selected-1)]);
            } catch (...) { valid=false; break; }
        }
        if (valid) return result;
        std::cout << "  Invalid selection.\n";
    }
}
void addCustomProfiles(Compiler& c) {
    while (confirm("Add a custom profile for " + c.id + "?", false)) {
        Profile p;
        p.suffix = lower(identifier(prompt("  Preset suffix (e.g. my-experiment)")));
        // Preset suffixes are normalized to lowercase letters/digits/underscores.
        if (std::any_of(c.profiles.begin(), c.profiles.end(), [&](const Profile& x){return x.suffix==p.suffix;})) {
            std::cout << "  That profile already exists.\n"; continue;
        }
        p.label = p.suffix;
        p.buildType = prompt("  CMake build type (auto, empty, Debug, Release, RelWithDebInfo, etc.)", "auto");
        p.compileFlags = prompt("  Extra compiler flags", "", true);
        p.linkFlags = prompt("  Extra linker flags", "", true);
        p.ipo = confirm("  Enable LTO/IPO?", false);
        c.profiles.push_back(std::move(p));
    }
}

Json makePresets(const vector<Compiler>& compilers, bool withTests) {
    Json root = Json::object();
    root.add("version", Json::integer(3));
    Json version = Json::object();
    version.add("major", Json::integer(3));
    version.add("minor", Json::integer(23));
    version.add("patch", Json::integer(0));
    root.add("cmakeMinimumRequired", std::move(version));
    Json configures = Json::array(), builds = Json::array(), tests = Json::array();
    for (const Compiler& c: compilers) {
        const string baseId = c.id + "-base";
        Json base = Json::object();
        base.add("name", Json::str(baseId));
        base.add("hidden", Json::boolean(true));
        base.add("generator", Json::str("Ninja"));
        base.add("binaryDir", Json::str("${sourceDir}/build/${presetName}"));
        Json cache = Json::object();
        cache.add("CMAKE_CXX_COMPILER", Json::str(c.command));
        cache.add("CMAKE_EXPORT_COMPILE_COMMANDS", Json::boolean(true));
        base.add("cacheVariables", std::move(cache));
        if (c.msvc) {
            Json cond = Json::object();
            cond.add("type", Json::str("equals"));
            cond.add("lhs", Json::str("${hostSystemName}"));
            cond.add("rhs", Json::str("Windows"));
            base.add("condition", std::move(cond));
        }
        configures.push(std::move(base));
        for (const string& arch: c.architectures) {
            for (const Profile& p: c.profiles) {
                if (p.tunedOnlyDefaultArch && arch != "native") continue;
                const string name = c.id + "-" + arch + "-" + p.suffix;
                const string archFlag = arch=="x86" ? "-m32" : arch=="x64" ? "-m64" : "";
                Json preset = Json::object();
                preset.add("name", Json::str(name));
                // Configure presets keep their stable IDs; friendly names belong to build presets.
                preset.add("inherits", Json::str(baseId));
                Json vars = Json::object();
                if (p.buildType != "auto") vars.add("CMAKE_BUILD_TYPE", Json::str(p.buildType=="empty" ? "" : p.buildType));
                const string cf = combined({c.extraCompile, archFlag, p.compileFlags});
                const string lf = combined({c.extraLink, archFlag, p.linkFlags});
                if (!cf.empty()) vars.add("CMAKE_CXX_FLAGS", Json::str(cf));
                if (!lf.empty()) vars.add("CMAKE_EXE_LINKER_FLAGS", Json::str(lf));
                if (p.ipo) vars.add("CMAKE_INTERPROCEDURAL_OPTIMIZATION", Json::boolean(true));
                if (!vars.fields.empty()) preset.add("cacheVariables", std::move(vars));
                configures.push(std::move(preset));
                Json build = Json::object();
                build.add("name", Json::str(name));
                build.add("displayName", Json::str(c.id + " / " + arch + " / " + p.label));
                build.add("configurePreset", Json::str(name));
                builds.push(std::move(build));
                if (withTests) {
                    Json test = Json::object();
                    test.add("name", Json::str(name));
                    test.add("configurePreset", Json::str(name));
                    Json output = Json::object();
                    output.add("outputOnFailure", Json::boolean(true));
                    test.add("output", std::move(output));
                    Json execution = Json::object();
                    execution.add("noTestsAction", Json::str("error"));
                    test.add("execution", std::move(execution));
                    tests.push(std::move(test));
                }
            }
        }
    }
    root.add("configurePresets", std::move(configures));
    root.add("buildPresets", std::move(builds));
    if (withTests) root.add("testPresets", std::move(tests));
    return root;
}

struct OutputFile { fs::path name; string content; };
void writeFiles(const fs::path& root, const vector<OutputFile>& files,
                const vector<fs::path>& directories) {
    // Preflight: no overwrite of an existing file, including existing CMake presets.
    for (const auto& f : files) {
        if (fs::exists(root / f.name))
            throw std::runtime_error("Refusing to overwrite existing file: " + (root/f.name).string());
    }
    for (const auto& d : directories) {
        if (fs::exists(root / d) && !fs::is_directory(root / d))
            throw std::runtime_error("Expected directory, found file: " + (root/d).string());
    }
    // Write files atomically where supported: temporary file then rename.
    // If an error happens, undo files created by this invocation.
    vector<fs::path> createdFiles, temporaryFiles;
    vector<fs::path> createdDirs;
    auto mkdirTracked = [&](const fs::path& p) {
        if (fs::exists(p)) {
            if (!fs::is_directory(p)) throw std::runtime_error("Not a directory: " + p.string());
            return;
        }
        vector<fs::path> pending;
        fs::path cursor = p;
        while (!cursor.empty() && !fs::exists(cursor)) { pending.push_back(cursor); cursor=cursor.parent_path(); }
        for (auto it=pending.rbegin(); it!=pending.rend(); ++it) {
            fs::create_directory(*it); createdDirs.push_back(*it);
        }
    };
    try {
        mkdirTracked(root);
        for (const auto& d: directories) mkdirTracked(root / d);
        for (const auto& f: files) {
            fs::path target = root / f.name;
            mkdirTracked(target.parent_path());
            fs::path tmp = target;
            tmp += ".projectgen-tmp";
            if (fs::exists(tmp)) throw std::runtime_error("Temporary file already exists: " + tmp.string());
            temporaryFiles.push_back(tmp);
            {
                std::ofstream out(tmp, std::ios::binary);
                if (!out) throw std::runtime_error("Cannot write " + tmp.string());
                out << f.content;
                out.close();
                if (!out) throw std::runtime_error("Failed writing " + tmp.string());
            }
            if (fs::exists(target)) throw std::runtime_error("Output appeared during generation: " + target.string());
            fs::rename(tmp, target);
            temporaryFiles.pop_back();
            createdFiles.push_back(target);
        }
    } catch (...) {
        for (auto it=temporaryFiles.rbegin(); it!=temporaryFiles.rend(); ++it) { std::error_code ec; fs::remove(*it, ec); }
        for (auto it=createdFiles.rbegin(); it!=createdFiles.rend(); ++it) { std::error_code ec; fs::remove(*it, ec); }
        for (auto it=createdDirs.rbegin(); it!=createdDirs.rend(); ++it) { std::error_code ec; fs::remove(*it, ec); }
        throw;
    }
}

int run() {
    std::cout << "Interactive CMake + Ninja project generator\n"
              << "-------------------------------------------\n"
              << "One-shot generation; files are ordinary editable CMake.\n\n";
    string displayName = prompt("Project name", "MyProject");
    string projectId = identifier(displayName);
    fs::path root = fs::path(prompt("Project root directory", displayName));
    if (root.empty()) throw std::runtime_error("Project root must not be empty");
    string mainId = identifier(prompt("Main executable/subproject ID", projectId));
    string mainFolder = segment(prompt("Main subproject directory name", mainId));
    int cppStandard = 20;
    for (;;) {
        string s=prompt("C++ standard (11, 14, 17, 20, 23, or 26)", "20");
        if (s=="11" || s=="14" || s=="17" || s=="20" || s=="23" || s=="26") { cppStandard=std::stoi(s); break; }
        std::cout << "  Choose one of the listed standards.\n";
    }
    vector<Module> modules;
    std::set<string> ids{mainId};
    std::set<string> folders{mainFolder};
    int count=promptCount("Number of reusable subprojects/modules");
    for (int i=0; i<count; ++i) {
        std::cout << "\n--- Module " << i+1 << " of " << count << " ---\n";
        Module m;
        for (;;) {
            m.id=identifier(prompt("Module target ID"));
            if (ids.insert(m.id).second) break;
            std::cout << "  Target ID collides with an existing ID. Choose another.\n";
        }
        for (;;) {
            m.folder=segment(prompt("Module directory name", m.id));
            if (folders.insert(m.folder).second) break;
            std::cout << "  Directory name already used.\n";
        }
        m.pathCode=segment(prompt("Path code for src/<code>/ and include/<code>/", m.id));
        modules.push_back(std::move(m));
    }
    Module main;
    main.id=mainId;
    main.folder=mainFolder;
    main.main=true;
    main.pathCode=segment(prompt("Main path code for src/<code>/ and include/<code>/", mainId));
    modules.push_back(main);

    if (count>0) {
        std::cout << "\nAvailable module IDs: ";
        for (int i=0; i<count; ++i) std::cout << (i ? ", " : "") << modules[static_cast<size_t>(i)].id;
        std::cout << '\n';
    }
    for (;;) {
        for (auto& m: modules) {
            std::cout << "\nDependencies for " << m.id << (m.main ? " (main executable)" : "") << ":\n";
            if (!m.main) m.publicDeps = askDependencies(m, modules, true);
            string defaultDeps;
            if (m.main) {
                vector<string> all;
                for (const auto& candidate: modules) if (!candidate.main) all.push_back(candidate.id);
                defaultDeps = join(all, ",");
            }
            m.privateDeps = askDependencies(m, modules, false, defaultDeps);
            for (const auto& pub: m.publicDeps) {
                if (std::find(m.privateDeps.begin(), m.privateDeps.end(), pub)!=m.privateDeps.end()) {
                    std::cerr << "Warning: " << m.id << " uses " << pub << " publicly and privately; private duplicate removed.\n";
                    m.privateDeps.erase(std::remove(m.privateDeps.begin(),m.privateDeps.end(),pub),m.privateDeps.end());
                }
            }
        }
        try { (void)dependencyOrder(modules); break; }
        catch (const std::runtime_error& error) {
            std::cout << "  " << error.what() << ". Please re-enter dependencies.\n";
        }
    }

    std::cout << "\n--- Compiler / preset configuration ---\n";
    std::cout << "Compiler definitions are generated even if the compiler is not installed here.\n";
    vector<Compiler> compilers;
    for (const char* nameRaw: {"gcc", "clang", "msvc"}) {
        const string name = nameRaw;
        if (!confirm("Include " + name + " compiler presets?", name=="gcc")) continue;
        Compiler c;
        c.id=name;
        c.msvc=name=="msvc";
        c.command=prompt("  C++ compiler command", name=="gcc" ? "g++" : name=="clang" ? "clang++" : "cl.exe");
        c.extraCompile=prompt("  Extra compile flags (all " + name + " profiles)", "", true);
        c.extraLink=prompt("  Extra link flags (all " + name + " profiles)", "", true);
        c.architectures={"native"};
        if (!c.msvc) {
            if (confirm("  Add x86 (-m32) presets? Requires compatible toolchain/multilib", false)) c.architectures.push_back("x86");
            if (confirm("  Add x64 (-m64) presets? Requires compatible toolchain", false)) c.architectures.push_back("x64");
        } else {
            std::cout << "  MSVC uses the architecture configured by the Visual Studio developer environment.\n";
        }
        c.profiles=selectProfiles(c);
        addCustomProfiles(c);
        if (c.profiles.empty()) {
            std::cout << "  No profiles selected; compiler skipped.\n";
        } else compilers.push_back(std::move(c));
    }
    if (compilers.empty()) throw std::runtime_error("At least one compiler with a profile must be selected");
    const bool withTests = confirm("Include GoogleTest / CTest infrastructure?", true);
    // Check ALL generated target names, including optional tests/main source interface.
    std::map<string, string> names;
    if (withTests) {
        for (const char* reserved : {"gtest", "gtest_main", "gmock", "gmock_main"})
            names.emplace(reserved, "GoogleTest target");
    }
    auto reserveTarget = [&](const string& name, const string& owner) {
        const auto [it, inserted] = names.emplace(name, owner);
        if (!inserted)
            throw std::runtime_error("CMake target collision: '" + name + "' (" + it->second + " vs " + owner + ")");
    };
    for (const auto& m : modules) {
        reserveTarget(m.id, "executable " + m.id);
        if (!m.main || withTests) reserveTarget(m.id + "_sources", "source interface " + m.id);
        if (withTests) reserveTarget(m.id + "_tests", "test executable " + m.id);
    }
    vector<size_t> order=dependencyOrder(modules);
    vector<OutputFile> files;
    vector<fs::path> directories;
    files.push_back({"CMakeLists.txt", rootCMake(projectId,cppStandard,modules,order,withTests)});
    files.push_back({"CMakePresets.json", makePresets(compilers,withTests).dump()});
    if (withTests)
        files.push_back({"cmake/ModuleTests.cmake", moduleTestsCMake()});
    for (const auto& m: modules) {
        fs::path base=m.folder;
        directories.push_back(base / "src" / m.pathCode);
        directories.push_back(base / "include" / m.pathCode);
        directories.push_back(base / "private");
        if (withTests) {
            directories.push_back(base / "tests");
            files.push_back({base / "tests" / ".gitkeep", ""});
        }
        files.push_back({base / "CMakeLists.txt", moduleCMake(m,withTests)});
        files.push_back({base / "main.cpp", mainCpp(m)});
    }
    if (!fs::exists(root / ".gitignore"))
        files.push_back({".gitignore", "build/\nCMakeUserPresets.json\n"});
    else std::cout << "\nExisting .gitignore will be left untouched. Ensure build/ is ignored.\n";
    std::cout << "\n--- Review ---\n";
    std::cout << "Root: " << root.string() << "\nProject: " << projectId << "\nC++: " << cppStandard << '\n';
    for (size_t i: order) std::cout << "  " << modules[i].id << "/ [" << modules[i].folder << "]" << (modules[i].main ? " (main)" : "") << '\n';
    int numPresets=0;
    for (const auto& c: compilers) for (const auto& a: c.architectures)
        for (const auto& p: c.profiles) if (!p.tunedOnlyDefaultArch || a=="native") ++numPresets;
    std::cout << "Build presets: " << numPresets << " (Ninja)\n";
    std::cout << "GoogleTest / CTest support: " << (withTests ? "enabled" : "disabled") << '\n';
    if (!confirm("Create this project?", true)) { std::cout << "Cancelled; no files created.\n"; return 0; }
    writeFiles(root, files, directories);
    std::cout << "\nCreated " << files.size() << " files successfully.\n"
              << "Configure: cmake --list-presets\n"
              << "           cmake --preset <preset-name>\n"
              << "Build:     cmake --build --preset <preset-name>\n";
    if (withTests)
        std::cout << "Test:      ctest --preset <preset-name>  (after adding and building tests)\n";
    std::cout << "Start those commands from: " << root.string() << '\n';
    return 0;
}
int main() {
    try { return run(); }
    catch (const std::exception& error) {
        std::cerr << "\nError: " << error.what() << "\n";
        return 1;
    }
}
