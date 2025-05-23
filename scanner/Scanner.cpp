#include <iostream>
#include <fstream>
#include <map>



bool isNumber(const std::string& token) {
    if (token.empty()) return false;
    size_t start = 0;
    int dotCount = 0;

    if (token[0] == '-') {
        if (token.size() == 1) return false;
        start = 1;
    }
    
    for (size_t i = start; i < token.size(); ++i) {
        if (token[i] == '.') {
            dotCount++;
            if (dotCount > 1) return false;
        }
        else if (!std::isdigit(token[i])) return false;
    }
    return true;
}

bool isIdentifier(const std::string& token) {
    if (token.empty()) return false;
    if (!std::isalpha(token[0]) && token[0] != '_') return false;
    for (char c : token) if (!std::isalnum(c) && c != '_') return false;
    return true;
}

void processToken(const std::string& token, std::ofstream& outFile, std::map<std::string, char> tokenMap) {
    
    if (!token.empty() && token[0] == '"') {
        if (token.size() < 2 || token.back() != '"') {
            std::cerr << "Ошибка: незакрытый строковый литерал: " << token << std::endl;
            return;
        }
        outFile << 's';
        return;
    }
    
    auto it = tokenMap.find(token);
    if (it != tokenMap.end()) {
        outFile << it->second;
        return;
    }
    
    if (isNumber(token)) {
        outFile << 'c';
        return;
    }
    
    if (isIdentifier(token)) {
        outFile << 'i';
        return;
    }

    std::cerr << "Неизвестный токен: " << token << std::endl;
}

int main() {

    std::ifstream inFile("code.txt");
    std::ofstream outFile("code_without_comments.txt");    

    bool in_block_comment = false;
    std::string line;

    while (std::getline(inFile, line)) {
        std::string clean_line;
        for (size_t i = 0; i < line.size(); ) {
            if (in_block_comment) {
                if (i + 1 < line.size() && line[i] == '*' && line[i + 1] == '/') {
                    in_block_comment = false;
                    i += 2;
                } else i++;
            } else {
                if (i + 1 < line.size() && line[i] == '/' && line[i + 1] == '/') break;
                if (i + 1 < line.size() && line[i] == '/' && line[i + 1] == '*') {
                    in_block_comment = true;
                    i += 2;
                } else clean_line += line[i++];
            }
        }
        if (!clean_line.empty() && !in_block_comment) outFile << clean_line << '\n';
    }

    inFile.close();
    outFile.close();

    std::ifstream input("keywords.txt");
    std::string line2;

    std::map<std::string, char> tokenMap;
    std::string currentSection;
    
    while (getline(input, line2)) {

        line2.erase(0, line2.find_first_not_of(" \t"));
        line2.erase(line2.find_last_not_of(" \t") + 1);

        if (line2.empty()) continue;

        if (line2.back() == ':') {
            currentSection = line2.substr(0, line2.size() - 1);
            continue;
        }

        if      (currentSection ==    "keywords") tokenMap[line2] = 'k';
        else if (currentSection ==   "operators") tokenMap[line2] = 'o';
        else if (currentSection == "punctuators") tokenMap[line2] = 'p';
    }

    std::ifstream Input("code_without_comments.txt");
    std::ofstream Output("tokens.txt");
    
    std::string token;
    char c;
    
    while (Input.get(c)) {
        if (std::isspace(c)) {
            if (!token.empty()) {
                processToken(token, Output, tokenMap);
                token.clear();
            }
        }
        else token += c;
    }
    
    if (!token.empty()) processToken(token, Output, tokenMap);
    
    Input.close();
    Output.close();

    return 0;
}
