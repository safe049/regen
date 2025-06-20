#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <regex>
#include <cctype>
#include <algorithm>
#include <iomanip>
#include <clocale>
#include <locale>

using namespace std;

// 系统语言检测
bool isChineseLocale() {
    const char* lang = getenv("LANG");
    if (lang == nullptr) lang = getenv("LC_ALL");
    if (lang == nullptr) lang = getenv("LC_MESSAGES");
    
    if (lang != nullptr) {
        string langStr(lang);
        transform(langStr.begin(), langStr.end(), langStr.begin(), ::tolower);
        return langStr.find("zh_cn") != string::npos || 
               langStr.find("zh-tw") != string::npos ||
               langStr.find("zh_hk") != string::npos ||
               langStr.find("zh_sg") != string::npos;
    }
    return false;
}

// 双语文本结构
struct I18NText {
    string en;
    string zh;
    
    string str() const {
        return isChineseLocale() ? zh : en;
    }
};

// 预设正则表达式模式
const map<string, pair<string, string>> PRESET_PATTERNS = {
    {"email", {R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})", 
               "电子邮件模式"}},
    {"url", {R"(https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*))", 
             "URL模式"}},
    {"ip", {R"((\d{1,3}\.){3}\d{1,3})", "IP地址模式"}},
    {"date", {R"(\d{4}-\d{2}-\d{2})", "日期模式 (YYYY-MM-DD)"}},
    {"time", {R"(\d{2}:\d{2}(:\d{2})?)", "时间模式 (HH:MM:SS)"}},
    {"phone", {R"(\+?\d{1,3}[-.\s]?$?\d{3}$?[-.\s]?\d{3}[-.\s]?\d{4})", 
               "电话号码模式"}},
    {"hex", {R"(#?([a-fA-F0-9]{6}|[a-fA-F0-9]{3}))", "十六进制颜色代码"}},
    {"credit-card", {R"(\d{4}[- ]?\d{4}[- ]?\d{4}[- ]?\d{4})", "信用卡号码"}}
};

// 正则表达式组件
struct RegexComponent {
    string pattern;
    I18NText description;
};

// 生成选项
struct GenerationOptions {
    bool matchWholeLine = false;
    bool caseInsensitive = false;
    bool multiline = false;
    bool dotAll = false;
    bool outputExplanation = false;
    string outputFormat = "regex";
    string language = "";
};

// 打印帮助信息
void printHelp() {
    if (isChineseLocale()) {
        cout << "Regen - 正则表达式生成工具\n\n";
        cout << "用法:\n";
        cout << "  regen [选项] [模式]\n";
        cout << "  regen --wizard\n\n";
        cout << "选项:\n";
        cout << "  -p, --preset <name>      使用预设模式 (email, url, ip, date, time, phone, hex, credit-card)\n";
        cout << "  -m, --match-whole        匹配整行 (添加 ^ 和 $)\n";
        cout << "  -i, --case-insensitive   不区分大小写\n";
        cout << "  --multiline              多行模式\n";
        cout << "  --dotall                 dot匹配所有字符(包括换行符)\n";
        cout << "  -e, --explain            输出正则表达式的解释\n";
        cout << "  -f, --format <format>    输出格式 (regex, cpp, python, java, javascript, go, rust)\n";
        cout << "  -w, --wizard             进入向导模式\n";
        cout << "  -h, --help               显示帮助信息\n\n";
        cout << "示例:\n";
        cout << "  regen -p email\n";
        cout << "  regen --wizard\n";
        cout << "  regen -m -i \"\\d+\" \"匹配一个或多个数字\"\n";
    } else {
        cout << "Regen - Regular Expression Generator\n\n";
        cout << "Usage:\n";
        cout << "  regen [options] [pattern]\n";
        cout << "  regen --wizard\n\n";
        cout << "Options:\n";
        cout << "  -p, --preset <name>      Use preset pattern (email, url, ip, date, time, phone, hex, credit-card)\n";
        cout << "  -m, --match-whole        Match whole line (add ^ and $)\n";
        cout << "  -i, --case-insensitive   Case insensitive\n";
        cout << "  --multiline              Multiline mode\n";
        cout << "  --dotall                 Dot matches all characters (including newline)\n";
        cout << "  -e, --explain            Output regex explanation\n";
        cout << "  -f, --format <format>    Output format (regex, cpp, python, java, javascript, go, rust)\n";
        cout << "  -w, --wizard             Enter wizard mode\n";
        cout << "  -h, --help               Show this help message\n\n";
        cout << "Examples:\n";
        cout << "  regen -p email\n";
        cout << "  regen --wizard\n";
        cout << "  regen -m -i \"\\d+\" \"Match one or more digits\"\n";
    }
}

// 获取用户输入
string getInput(const I18NText& prompt, const string& defaultValue = "") {
    cout << prompt.str();
    if (!defaultValue.empty()) {
        cout << " [" << defaultValue << "]";
    }
    cout << ": ";
    
    string input;
    getline(cin, input);
    
    if (input.empty() && !defaultValue.empty()) {
        return defaultValue;
    }
    return input;
}

// 生成正则表达式解释
string generateExplanation(const string& pattern) {
    stringstream explanation;
    
    if (isChineseLocale()) {
        explanation << "正则表达式解释:\n";
    } else {
        explanation << "Regular Expression Explanation:\n";
    }
    
    size_t i = 0;
    while (i < pattern.length()) {
        char c = pattern[i];
        
        switch (c) {
            case '\\':
                if (i + 1 < pattern.length()) {
                    char next = pattern[i + 1];
                    if (isChineseLocale()) {
                        switch (next) {
                            case 'd': explanation << "  \\d - 匹配任何数字 (0-9)\n"; break;
                            case 'w': explanation << "  \\w - 匹配任何单词字符 (字母、数字、下划线)\n"; break;
                            case 's': explanation << "  \\s - 匹配任何空白字符\n"; break;
                            case 'D': explanation << "  \\D - 匹配任何非数字字符\n"; break;
                            case 'W': explanation << "  \\W - 匹配任何非单词字符\n"; break;
                            case 'S': explanation << "  \\S - 匹配任何非空白字符\n"; break;
                            case 'b': explanation << "  \\b - 匹配单词边界\n"; break;
                            case 'B': explanation << "  \\B - 匹配非单词边界\n"; break;
                            case '\\': explanation << "  \\\\ - 匹配反斜杠字符\n"; break;
                            default: explanation << "  \\" << next << " - 匹配转义字符 '" << next << "'\n"; break;
                        }
                    } else {
                        switch (next) {
                            case 'd': explanation << "  \\d - Match any digit (0-9)\n"; break;
                            case 'w': explanation << "  \\w - Match any word character (letter, digit, underscore)\n"; break;
                            case 's': explanation << "  \\s - Match any whitespace character\n"; break;
                            case 'D': explanation << "  \\D - Match any non-digit character\n"; break;
                            case 'W': explanation << "  \\W - Match any non-word character\n"; break;
                            case 'S': explanation << "  \\S - Match any non-whitespace character\n"; break;
                            case 'b': explanation << "  \\b - Match a word boundary\n"; break;
                            case 'B': explanation << "  \\B - Match a non-word boundary\n"; break;
                            case '\\': explanation << "  \\\\ - Match a backslash character\n"; break;
                            default: explanation << "  \\" << next << " - Match escaped character '" << next << "'\n"; break;
                        }
                    }
                    i += 2;
                } else {
                    if (isChineseLocale()) {
                        explanation << "  \\ - 匹配反斜杠字符\n";
                    } else {
                        explanation << "  \\ - Match a backslash character\n";
                    }
                    i++;
                }
                break;
                
            case '^':
                if (isChineseLocale()) {
                    explanation << "  ^ - 匹配行的开头\n";
                } else {
                    explanation << "  ^ - Match the start of a line\n";
                }
                i++;
                break;
                
            case '$':
                if (isChineseLocale()) {
                    explanation << "  $ - 匹配行的结尾\n";
                } else {
                    explanation << "  $ - Match the end of a line\n";
                }
                i++;
                break;
                
            case '.':
                if (isChineseLocale()) {
                    explanation << "  . - 匹配任何单个字符(换行符除外)\n";
                } else {
                    explanation << "  . - Match any single character (except newline)\n";
                }
                i++;
                break;
                
            case '[': {
                size_t end = pattern.find(']', i);
                if (end != string::npos) {
                    string charClass = pattern.substr(i, end - i + 1);
                    if (isChineseLocale()) {
                        explanation << "  " << charClass << " - 匹配字符集中的任意一个字符\n";
                    } else {
                        explanation << "  " << charClass << " - Match any character in the set\n";
                    }
                    i = end + 1;
                } else {
                    if (isChineseLocale()) {
                        explanation << "  [ - 未闭合的字符集\n";
                    } else {
                        explanation << "  [ - Unclosed character set\n";
                    }
                    i++;
                }
                break;
            }
                
            case '(': {
                size_t end = pattern.find(')', i);
                if (end != string::npos) {
                    string group = pattern.substr(i, end - i + 1);
                    if (isChineseLocale()) {
                        explanation << "  " << group << " - 捕获组\n";
                    } else {
                        explanation << "  " << group << " - Capturing group\n";
                    }
                    i = end + 1;
                } else {
                    if (isChineseLocale()) {
                        explanation << "  ( - 未闭合的捕获组\n";
                    } else {
                        explanation << "  ( - Unclosed capturing group\n";
                    }
                    i++;
                }
                break;
            }
                
            case '{': {
                size_t end = pattern.find('}', i);
                if (end != string::npos) {
                    string quantifier = pattern.substr(i, end - i + 1);
                    if (isChineseLocale()) {
                        explanation << "  " << quantifier << " - 量词，指定前面元素的重复次数\n";
                    } else {
                        explanation << "  " << quantifier << " - Quantifier, specifies how many times the preceding element can occur\n";
                    }
                    i = end + 1;
                } else {
                    if (isChineseLocale()) {
                        explanation << "  { - 未闭合的量词\n";
                    } else {
                        explanation << "  { - Unclosed quantifier\n";
                    }
                    i++;
                }
                break;
            }
                
            case '*':
                if (isChineseLocale()) {
                    explanation << "  * - 匹配前面的元素零次或多次\n";
                } else {
                    explanation << "  * - Match the preceding element zero or more times\n";
                }
                i++;
                break;
                
            case '+':
                if (isChineseLocale()) {
                    explanation << "  + - 匹配前面的元素一次或多次\n";
                } else {
                    explanation << "  + - Match the preceding element one or more times\n";
                }
                i++;
                break;
                
            case '?':
                if (isChineseLocale()) {
                    explanation << "  ? - 匹配前面的元素零次或一次\n";
                } else {
                    explanation << "  ? - Match the preceding element zero or one time\n";
                }
                i++;
                break;
                
            case '|':
                if (isChineseLocale()) {
                    explanation << "  | - 或操作符\n";
                } else {
                    explanation << "  | - OR operator\n";
                }
                i++;
                break;
                
            default:
                if (isalnum(c)) {
                    if (isChineseLocale()) {
                        explanation << "  " << c << " - 匹配字符 '" << c << "'\n";
                    } else {
                        explanation << "  " << c << " - Match character '" << c << "'\n";
                    }
                } else {
                    if (isChineseLocale()) {
                        explanation << "  " << c << " - 特殊字符\n";
                    } else {
                        explanation << "  " << c << " - Special character\n";
                    }
                }
                i++;
                break;
        }
    }
    
    return explanation.str();
}

// 根据语言生成代码片段
string generateCodeSnippet(const string& pattern, const string& language, const GenerationOptions& options) {
    stringstream code;
    
    string escapedPattern;
    for (char c : pattern) {
        if (c == '\\') {
            escapedPattern += "\\\\";
        } else if (c == '"') {
            escapedPattern += "\\\"";
        } else {
            escapedPattern += c;
        }
    }
    
    string modifiers;
    if (options.caseInsensitive) modifiers += "i";
    if (options.multiline) modifiers += "m";
    if (options.dotAll) modifiers += "s";
    
    if (language == "cpp") {
        code << "// C++ Regular Expression\n";
        code << "#include <regex>\n\n";
        code << "std::regex pattern(R\"(" << pattern << ")\");\n";
        if (!modifiers.empty()) {
            code << "std::regex_constants::syntax_option_type flags = ";
            bool first = true;
            if (options.caseInsensitive) {
                code << "std::regex_constants::icase";
                first = false;
            }
            if (options.multiline) {
                if (!first) code << " | ";
                code << "std::regex_constants::multiline";
                first = false;
            }
            if (options.dotAll) {
                if (!first) code << " | ";
                code << "std::regex_constants::ECMAScript"; // C++ 没有直接的 dotall 标志
            }
            code << ";\n";
            code << "std::regex pattern(R\"(" << pattern << ")\", flags);\n";
        }
    } else if (language == "python") {
        code << "# Python Regular Expression\n";
        code << "import re\n\n";
        code << "pattern = re.compile(r\"" << escapedPattern << "\"";
        if (!modifiers.empty()) {
            code << ", re.";
            bool first = true;
            if (options.caseInsensitive) {
                code << "IGNORECASE";
                first = false;
            }
            if (options.multiline) {
                if (!first) code << " | re.";
                code << "MULTILINE";
                first = false;
            }
            if (options.dotAll) {
                if (!first) code << " | re.";
                code << "DOTALL";
            }
        }
        code << ")\n";
    } else if (language == "java") {
        code << "// Java Regular Expression\n";
        code << "import java.util.regex.Pattern;\n\n";
        code << "Pattern pattern = Pattern.compile(\"" << escapedPattern << "\"";
        if (!modifiers.empty()) {
            code << ", ";
            bool first = true;
            if (options.caseInsensitive) {
                code << "Pattern.CASE_INSENSITIVE";
                first = false;
            }
            if (options.multiline) {
                if (!first) code << " | ";
                code << "Pattern.MULTILINE";
                first = false;
            }
            if (options.dotAll) {
                if (!first) code << " | ";
                code << "Pattern.DOTALL";
            }
        }
        code << ");\n";
    } else if (language == "javascript") {
        code << "// JavaScript Regular Expression\n";
        code << "const pattern = /" << pattern << "/" << modifiers << ";\n";
    } else if (language == "go") {
        code << "// Go Regular Expression\n";
        code << "import \"regexp\"\n\n";
        code << "pattern := regexp.MustCompile(`" << pattern << "`)\n";
    } else if (language == "rust") {
        code << "// Rust Regular Expression\n";
        code << "use regex::Regex;\n\n";
        code << "let pattern = Regex::new(r\"" << pattern << "\").unwrap();\n";
    } else {
        return "";
    }
    
    return code.str();
}

// 向导模式
void wizardMode() {
    if (isChineseLocale()) {
        cout << "Regen 向导模式 - 交互式正则表达式生成\n";
        cout << "------------------------------------\n\n";
    } else {
        cout << "Regen Wizard Mode - Interactive Regular Expression Generator\n";
        cout << "------------------------------------------------------------\n\n";
    }
    
    vector<RegexComponent> components;
    GenerationOptions options;
    
    // 选择模式
    if (isChineseLocale()) {
        cout << "选择生成模式:\n";
        cout << "1. 使用预设模式\n";
        cout << "2. 自定义构建模式\n";
        cout << "3. 组合多个模式\n";
    } else {
        cout << "Select generation mode:\n";
        cout << "1. Use preset pattern\n";
        cout << "2. Custom build pattern\n";
        cout << "3. Combine multiple patterns\n";
    }
    
    string choice = getInput(isChineseLocale() ? 
        I18NText{"请输入选择(1-3)", "Enter your choice (1-3)"} : 
        I18NText{"Enter your choice (1-3)", "请输入选择(1-3)"}, 
        "1");
    
    if (choice == "1") {
        // 预设模式
        if (isChineseLocale()) {
            cout << "\n可用的预设模式:\n";
        } else {
            cout << "\nAvailable preset patterns:\n";
        }
        
        for (const auto& preset : PRESET_PATTERNS) {
            cout << "  " << left << setw(12) << preset.first << " - " << preset.second.first;
            if (isChineseLocale()) {
                cout << " (" << preset.second.second << ")";
            }
            cout << "\n";
        }
        
        string presetName = getInput(isChineseLocale() ? 
            I18NText{"\n输入预设模式名称", "\nEnter preset pattern name"} : 
            I18NText{"\nEnter preset pattern name", "\n输入预设模式名称"});
        
        if (PRESET_PATTERNS.find(presetName) != PRESET_PATTERNS.end()) {
            components.push_back({PRESET_PATTERNS.at(presetName).first, 
                isChineseLocale() ? 
                    I18NText{"预设模式: " + presetName, "Preset pattern: " + presetName} : 
                    I18NText{"Preset pattern: " + presetName, "预设模式: " + presetName}});
        } else {
            if (isChineseLocale()) {
                cerr << "错误: 未知的预设模式 '" << presetName << "'\n";
            } else {
                cerr << "Error: Unknown preset pattern '" << presetName << "'\n";
            }
            return;
        }
    } else if (choice == "2") {
        // 自定义模式
        if (isChineseLocale()) {
            cout << "\n自定义正则表达式构建\n";
        } else {
            cout << "\nCustom Regular Expression Builder\n";
        }
        
        string pattern = getInput(isChineseLocale() ? 
            I18NText{"输入正则表达式模式", "Enter regular expression pattern"} : 
            I18NText{"Enter regular expression pattern", "输入正则表达式模式"});
        
        string description = getInput(isChineseLocale() ? 
            I18NText{"输入模式描述(可选)", "Enter pattern description (optional)"} : 
            I18NText{"Enter pattern description (optional)", "输入模式描述(可选)"}, "");
        
        components.push_back({pattern, 
            isChineseLocale() ? 
                I18NText{description.empty() ? "自定义模式" : description, 
                         description.empty() ? "Custom pattern" : description} : 
                I18NText{description.empty() ? "Custom pattern" : description, 
                         description.empty() ? "自定义模式" : description}});
    } else if (choice == "3") {
        // 组合模式
        if (isChineseLocale()) {
            cout << "\n组合多个正则表达式模式\n";
        } else {
            cout << "\nCombine Multiple Regular Expression Patterns\n";
        }
        
        while (true) {
            if (isChineseLocale()) {
                cout << "\n当前模式: ";
            } else {
                cout << "\nCurrent pattern: ";
            }
            
            if (components.empty()) {
                if (isChineseLocale()) {
                    cout << "无\n";
                } else {
                    cout << "None\n";
                }
            } else {
                for (const auto& comp : components) {
                    cout << comp.pattern;
                }
                cout << "\n";
            }
            
            if (isChineseLocale()) {
                cout << "\n1. 添加预设模式\n";
                cout << "2. 添加自定义模式\n";
                cout << "3. 完成构建\n";
            } else {
                cout << "\n1. Add preset pattern\n";
                cout << "2. Add custom pattern\n";
                cout << "3. Finish building\n";
            }
            
            string subChoice = getInput(isChineseLocale() ? 
                I18NText{"请输入选择(1-3)", "Enter your choice (1-3)"} : 
                I18NText{"Enter your choice (1-3)", "请输入选择(1-3)"}, 
                "3");
            
            if (subChoice == "1") {
                // 添加预设
                if (isChineseLocale()) {
                    cout << "\n可用的预设模式:\n";
                } else {
                    cout << "\nAvailable preset patterns:\n";
                }
                
                for (const auto& preset : PRESET_PATTERNS) {
                    cout << "  " << left << setw(12) << preset.first << " - " << preset.second.first;
                    if (isChineseLocale()) {
                        cout << " (" << preset.second.second << ")";
                    }
                    cout << "\n";
                }
                
                string presetName = getInput(isChineseLocale() ? 
                    I18NText{"\n输入预设模式名称", "\nEnter preset pattern name"} : 
                    I18NText{"\nEnter preset pattern name", "\n输入预设模式名称"});
                
                if (PRESET_PATTERNS.find(presetName) != PRESET_PATTERNS.end()) {
                    components.push_back({PRESET_PATTERNS.at(presetName).first, 
                        isChineseLocale() ? 
                            I18NText{"预设模式: " + presetName, "Preset pattern: " + presetName} : 
                            I18NText{"Preset pattern: " + presetName, "预设模式: " + presetName}});
                } else {
                    if (isChineseLocale()) {
                        cerr << "错误: 未知的预设模式 '" << presetName << "'\n";
                    } else {
                        cerr << "Error: Unknown preset pattern '" << presetName << "'\n";
                    }
                }
            } else if (subChoice == "2") {
                // 添加自定义
                string pattern = getInput(isChineseLocale() ? 
                    I18NText{"输入正则表达式模式", "Enter regular expression pattern"} : 
                    I18NText{"Enter regular expression pattern", "输入正则表达式模式"});
                
                string description = getInput(isChineseLocale() ? 
                    I18NText{"输入模式描述(可选)", "Enter pattern description (optional)"} : 
                    I18NText{"Enter pattern description (optional)", "输入模式描述(可选)"}, "");
                
                components.push_back({pattern, 
                    isChineseLocale() ? 
                        I18NText{description.empty() ? "自定义模式" : description, 
                                 description.empty() ? "Custom pattern" : description} : 
                        I18NText{description.empty() ? "Custom pattern" : description, 
                                 description.empty() ? "自定义模式" : description}});
            } else if (subChoice == "3") {
                break;
            }
        }
    }
    
    // 组合所有模式
    string finalPattern;
    for (const auto& comp : components) {
        finalPattern += comp.pattern;
    }
    
    // 设置选项
    if (isChineseLocale()) {
        cout << "\n正则表达式选项:\n";
    } else {
        cout << "\nRegular Expression Options:\n";
    }
    
    options.matchWholeLine = getInput(isChineseLocale() ? 
        I18NText{"匹配整行(添加 ^ 和 $)? (y/n)", "Match whole line (add ^ and $)? (y/n)"} : 
        I18NText{"Match whole line (add ^ and $)? (y/n)", "匹配整行(添加 ^ 和 $)? (y/n)"}, 
        "n") == "y";
    
    options.caseInsensitive = getInput(isChineseLocale() ? 
        I18NText{"不区分大小写? (y/n)", "Case insensitive? (y/n)"} : 
        I18NText{"Case insensitive? (y/n)", "不区分大小写? (y/n)"}, 
        "n") == "y";
    
    options.multiline = getInput(isChineseLocale() ? 
        I18NText{"多行模式? (y/n)", "Multiline mode? (y/n)"} : 
        I18NText{"Multiline mode? (y/n)", "多行模式? (y/n)"}, 
        "n") == "y";
    
    options.dotAll = getInput(isChineseLocale() ? 
        I18NText{"dot匹配所有字符(包括换行符)? (y/n)", "Dot matches all characters (including newline)? (y/n)"} : 
        I18NText{"Dot matches all characters (including newline)? (y/n)", "dot匹配所有字符(包括换行符)? (y/n)"}, 
        "n") == "y";
    
    options.outputExplanation = getInput(isChineseLocale() ? 
        I18NText{"输出正则表达式解释? (y/n)", "Output regex explanation? (y/n)"} : 
        I18NText{"Output regex explanation? (y/n)", "输出正则表达式解释? (y/n)"}, 
        "y") == "y";
    
    // 输出格式
    if (isChineseLocale()) {
        cout << "\n输出格式选项:\n";
        cout << "1. 纯正则表达式\n";
        cout << "2. 带语言代码片段\n";
    } else {
        cout << "\nOutput Format Options:\n";
        cout << "1. Plain regular expression\n";
        cout << "2. With language code snippet\n";
    }
    
    string formatChoice = getInput(isChineseLocale() ? 
        I18NText{"请输入选择(1-2)", "Enter your choice (1-2)"} : 
        I18NText{"Enter your choice (1-2)", "请输入选择(1-2)"}, 
        "1");
    
    if (formatChoice == "2") {
        if (isChineseLocale()) {
            cout << "\n可用的语言:\n";
            cout << "  cpp - C++\n";
            cout << "  python - Python\n";
            cout << "  java - Java\n";
            cout << "  javascript - JavaScript\n";
            cout << "  go - Go\n";
            cout << "  rust - Rust\n";
        } else {
            cout << "\nAvailable languages:\n";
            cout << "  cpp - C++\n";
            cout << "  python - Python\n";
            cout << "  java - Java\n";
            cout << "  javascript - JavaScript\n";
            cout << "  go - Go\n";
            cout << "  rust - Rust\n";
        }
        
        options.language = getInput(isChineseLocale() ? 
            I18NText{"输入语言", "Enter language"} : 
            I18NText{"Enter language", "输入语言"});
        options.outputFormat = "code";
    }
    
    // 应用选项
    if (options.matchWholeLine) {
        finalPattern = "^" + finalPattern + "$";
    }
    
    // 输出结果
    if (isChineseLocale()) {
        cout << "\n生成结果:\n";
        cout << "------------------------------------\n";
        cout << "正则表达式: " << finalPattern << "\n";
    } else {
        cout << "\nGenerated Result:\n";
        cout << "------------------------------------------------------------\n";
        cout << "Regular Expression: " << finalPattern << "\n";
    }
    
    if (options.outputExplanation) {
        cout << "\n" << generateExplanation(finalPattern) << "\n";
    }
    
    if (options.outputFormat == "code" && !options.language.empty()) {
        string code = generateCodeSnippet(finalPattern, options.language, options);
        if (!code.empty()) {
            cout << "\n" << code << "\n";
        }
    }
    
    if (isChineseLocale()) {
        cout << "------------------------------------\n";
    } else {
        cout << "------------------------------------------------------------\n";
    }
}

// 直接参数模式
void directMode(const vector<string>& args) {
    string pattern;
    string description;
    GenerationOptions options;
    bool usePreset = false;
    string presetName;
    
    // 解析参数
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "-h" || args[i] == "--help") {
            printHelp();
            return;
        } else if (args[i] == "-w" || args[i] == "--wizard") {
            wizardMode();
            return;
        } else if (args[i] == "-p" || args[i] == "--preset") {
            if (i + 1 < args.size()) {
                usePreset = true;
                presetName = args[++i];
            } else {
                if (isChineseLocale()) {
                    cerr << "错误: --preset 需要参数\n";
                } else {
                    cerr << "Error: --preset requires an argument\n";
                }
                return;
            }
        } else if (args[i] == "-m" || args[i] == "--match-whole") {
            options.matchWholeLine = true;
        } else if (args[i] == "-i" || args[i] == "--case-insensitive") {
            options.caseInsensitive = true;
        } else if (args[i] == "--multiline") {
            options.multiline = true;
        } else if (args[i] == "--dotall") {
            options.dotAll = true;
        } else if (args[i] == "-e" || args[i] == "--explain") {
            options.outputExplanation = true;
        } else if (args[i] == "-f" || args[i] == "--format") {
            if (i + 1 < args.size()) {
                options.outputFormat = args[++i];
                if (options.outputFormat != "regex") {
                    options.language = options.outputFormat;
                    options.outputFormat = "code";
                }
            } else {
                if (isChineseLocale()) {
                    cerr << "错误: --format 需要参数\n";
                } else {
                    cerr << "Error: --format requires an argument\n";
                }
                return;
            }
        } else if (pattern.empty()) {
            pattern = args[i];
        } else if (description.empty()) {
            description = args[i];
        }
    }
    
    // 获取模式
    if (usePreset) {
        if (PRESET_PATTERNS.find(presetName) != PRESET_PATTERNS.end()) {
            pattern = PRESET_PATTERNS.at(presetName).first;
            description = isChineseLocale() ? 
                ("预设模式: " + presetName) : 
                ("Preset pattern: " + presetName);
        } else {
            if (isChineseLocale()) {
                cerr << "错误: 未知的预设模式 '" << presetName << "'\n";
            } else {
                cerr << "Error: Unknown preset pattern '" << presetName << "'\n";
            }
            return;
        }
    } else if (pattern.empty()) {
        if (isChineseLocale()) {
            cerr << "错误: 需要提供正则表达式模式\n";
        } else {
            cerr << "Error: Regular expression pattern is required\n";
        }
        printHelp();
        return;
    }
    
    // 应用选项
    if (options.matchWholeLine) {
        pattern = "^" + pattern + "$";
    }
    
    // 输出结果
    if (isChineseLocale()) {
        cout << "生成结果:\n";
        cout << "------------------------------------\n";
        cout << "正则表达式: " << pattern << "\n";
        if (!description.empty()) {
            cout << "描述: " << description << "\n";
        }
    } else {
        cout << "Generated Result:\n";
        cout << "------------------------------------------------------------\n";
        cout << "Regular Expression: " << pattern << "\n";
        if (!description.empty()) {
            cout << "Description: " << description << "\n";
        }
    }
    
    if (options.outputExplanation) {
        cout << "\n" << generateExplanation(pattern) << "\n";
    }
    
    if (options.outputFormat == "code" && !options.language.empty()) {
        string code = generateCodeSnippet(pattern, options.language, options);
        if (!code.empty()) {
            cout << "\n" << code << "\n";
        }
    }
    
    if (isChineseLocale()) {
        cout << "------------------------------------\n";
    } else {
        cout << "------------------------------------------------------------\n";
    }
}

int main(int argc, char* argv[]) {
    // 设置本地化
    setlocale(LC_ALL, "");
    
    vector<string> args(argv, argv + argc);
    
    if (argc == 1) {
        printHelp();
        return 0;
    }
    
    // 检查是否是向导模式
    for (const auto& arg : args) {
        if (arg == "-w" || arg == "--wizard") {
            wizardMode();
            return 0;
        }
    }
    
    // 直接参数模式
    directMode(args);
    
    return 0;
}