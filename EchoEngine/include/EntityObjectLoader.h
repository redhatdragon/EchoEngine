#pragma once
#include "EntityObject.h"
#include "Name.h"
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>

namespace EntityObjectLoader {
    static std::string errorStr = "";

    inline void removeCharsFromString(std::string& str, const char* charsToRemove) {
        for (unsigned int i = 0; i < strlen(charsToRemove); ++i)
            str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
    }
    inline std::vector<std::string> split(const std::string& str, const std::string& delim) {
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
    inline bool isDigits(const std::string& s) {
        for (char c : s) if (!isdigit(c)) return false;
        return true;
    }



    inline void cleanText(std::string& text) {
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
        removeCharsFromString(text, " \n\r");
    }

    inline bool isTokenString(const std::string& token) {
        unsigned int count = 0;
        if (token[0] != '"') return false;
        for (char c : token) {
            if (c == '"')
                count++;
        }
        if(count == 2)
            return true;
        errorStr += "Statement should only have two \"\n";
        return false;
    }
    inline bool isTokenArray(const std::string& token) {
        unsigned int left = 0, right = 0;
        if (token[0] != '[') return false;
        for (char c : token) {
            if (c == '[')
                left++;
            if (c == ']')
                right++;
        }
        if (left != 1) {
            errorStr = "Statement should have exactly one [";
            return false;
        }
        if (right != 1) {
            errorStr += "Statement should have exactly one ]";
            return false;
        }
        return true;
    }
    inline bool isTokenNULL(const std::string& token) {
        if (token == "NULL")
            return true;
        return false;
    }

    inline ComponentObject createComponentInteger(const std::string& name, const std::string& value) {
        ComponentObject retValue = ComponentObject();
        int num = atoi(value.c_str());
        retValue.setInt((uint32_t)num);
        retValue.name = name;
        return retValue;
    }
    inline ComponentObject createComponentString(const std::string& name, const std::string& value) {
        ComponentObject retValue = ComponentObject();
        std::string cleanedValue = value;
        removeCharsFromString(cleanedValue, "\"");
        retValue.setString(cleanedValue.c_str());
        retValue.name = name;
        return retValue;
    }
    inline ComponentObject createComponentArray(const std::string& name, const std::string& value) {
        ComponentObject retValue = ComponentObject();
        std::string cleanedValue = value;
        removeCharsFromString(cleanedValue, "[]");
        std::vector<std::string> elements = split(cleanedValue, ",");
        std::vector<uint32_t> values;
        for (auto& element : elements) {
            if (isTokenString(element)) {
                Name nameElement = element;
                values.push_back(nameElement.getID());
                continue;
            }
            if (isDigits(element)) {
                int num = atoi(element.c_str());
                values.push_back((uint32_t)num);
                continue;
            }
            errorStr += "Statement's array has an invalid element, elements may only be of type integer or string\n";
            return retValue;
        }
        retValue.setArray(values);
        retValue.name = name;
        //uint32_t* arrayElement1 = (uint32_t*)retValue.data;
        //uint32_t* arrayElement2 = arrayElement1 + 1;
        return retValue;
    }
    inline ComponentObject createComponentNULL(std::string& name) {
        ComponentObject retValue = ComponentObject();
        retValue.setNULL();
        retValue.name = name;
        return retValue;
    }

    inline ComponentObject createComponentObjectFromString(const std::string& str) {
        errorStr = "";
        //ComponentObject retValue = ComponentObject();
        std::vector<std::string> tokens = split(str, "=");
        //std::cout << tokens[0] << std::endl;
        //std::cout << tokens[1] << std::endl;

        if (tokens.size() != 2) {
            errorStr += "Statement should have one = symbol\n";
            return ComponentObject();
        }
        if (isTokenArray(tokens[1]))
            return createComponentArray(tokens[0], tokens[1]);
        if (isDigits(tokens[1]))
            return createComponentInteger(tokens[0], tokens[1]);
        if (isTokenString(tokens[1]))
            return createComponentString(tokens[0], tokens[1]);
        if (isTokenNULL(tokens[1]))
            return createComponentNULL(tokens[0]);

        errorStr += "Value is not of array, integer, string, or NULL type\n";
        return ComponentObject();

        /*if (tokens.size() != 2) {
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
        return retValue;*/
    }
    inline EntityObject createEntityObjectFromString(std::string str) {
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

    inline EntityObject createEntityObjectFromFile(std::string &filename) {
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