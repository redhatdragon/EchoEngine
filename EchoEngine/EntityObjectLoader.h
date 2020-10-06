#pragma once
#include "EntityObject.h"
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>

namespace EntityObjectLoader {
    std::string errorStr = "";

    void removeCharsFromString(std::string& str, const char* charsToRemove) {
        for (unsigned int i = 0; i < strlen(charsToRemove); ++i)
            str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
    }
    std::vector<std::string> split(const std::string& str, const std::string& delim) {
        std::vector<std::string> tokens;
        size_t prev = 0, pos = 0;
        do {
            pos = str.find(delim, prev);
            if (pos == std::string::npos) pos = str.length();
            std::string token = str.substr(prev, pos - prev);
            if (!token.empty()) tokens.push_back(token);
            prev = pos + delim.length();
        } while (pos < str.length() && prev < str.length());
        return tokens;
    }
    bool isDigits(const std::string& s) {
        for (char c : s) if (!isdigit(c)) return false;
        return true;
    }



    void cleanText(std::string& text) {
        {
            size_t currentIndex = 0;
            while (true) {
                size_t pos = text.find("/#", currentIndex);
                if (pos == std::string::npos) break;
                size_t endPos = text.find("#/", pos);
                if (endPos == std::string::npos) {
                    size_t lineCount = 0;
                    for (size_t i = 0; i < pos; i++)
                        if (text[i] == '\n')
                            lineCount++;
                    std::cout << "Error no matching #/ at line " << lineCount + 1 << std::endl;
                    return;
                }
                currentIndex = endPos + 1;
            }
        }
        {
            size_t currentIndex = 0;
            while (true) {
                size_t pos = text.find("/#");
                if (pos == std::string::npos) break;
                size_t endPos = text.find("#/", pos);
                text.erase(pos, (endPos + 2) - pos);
            }
        }
        {
            size_t currentIndex = 0;
            while (true) {
                size_t pos = text.find("#");
                if (pos == std::string::npos) break;
                size_t endPos = text.find("\n", pos);
                text.erase(pos, (endPos + 1) - pos);
            }
        }
        removeCharsFromString(text, " \n");
    }

    ComponentObject createComponentObjectFromString(const std::string& str) {
        ComponentObject retValue = ComponentObject();
        std::vector<std::string> tokens = split(str, "=");
        //std::cout << tokens[0] << std::endl;
        //std::cout << tokens[1] << std::endl;
        if (tokens.size() != 2) {
            errorStr += "Statement should have one = symbol\n";
            return retValue;
        }
        retValue.name = tokens[0];
        if (tokens[1][0] == '[') {
            size_t len = tokens[1].size();
            if (tokens[1][len - 1] != ']') {
                errorStr += "Expecting ] at end of array";
                return retValue;
            }
            std::string arr = tokens[1];
            removeCharsFromString(arr, "[");
            removeCharsFromString(arr, "]");
            std::vector<std::string> valuesStrings = split(arr, ",");
            std::vector<uint32_t> numbers;
            for (const std::string& v : valuesStrings) {
                if (isDigits(v) == false) {
                    errorStr += "Array value is not a digit";
                    return retValue;
                }
                numbers.push_back((uint32_t)atoi(v.c_str()));
            }
            retValue.setArrayInt(numbers);
        }
        if (tokens[1][0] == '"') {
            std::vector<std::string> quoteTokens = split(tokens[1], "\"");
            if (quoteTokens.size() != 1) {
                errorStr += "Improperly formated string value\n";
                return retValue;
            }
            retValue.setString(quoteTokens[0].c_str());
            return retValue;
        }
        if (isDigits(tokens[1])) {
            int num = atoi(tokens[1].c_str());
            retValue.setInt((uint32_t)num);
        }
        //throw;
        return retValue;
    }
    EntityObject createEntityObjectFromString(std::string str) {
        cleanText(str);
        EntityObject retValue = EntityObject();
        size_t currentPos = 0;
        uint32_t statementCount = 0;
        while (true) {
            statementCount++;
            size_t newPos = str.find(';', currentPos);
            if (newPos == std::string::npos) break;
            std::string statement = str.substr(currentPos, newPos - currentPos);
            //std::cout << statement << std::endl;
            retValue.components.push_back(createComponentObjectFromString(statement));
            currentPos = newPos + 1;
        }
        return retValue;
    }

    EntityObject createEntityObjectFromFile(std::string filename) {
        std::string line;
        std::ifstream myfile(filename);
        std::string fileText = "";
        if (myfile.is_open()) {
            while (getline(myfile, line))
                fileText += line + "\n";
            myfile.close();
        }
        auto v = createEntityObjectFromString(fileText);
        return v;
    }
}