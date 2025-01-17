#pragma once
#include<iostream>
#include <fstream>
#include<iomanip>
#include<string>
#include<sstream>
#include <cctype>
#include <openssl/sha.h>
#include <cmath>
#include <stdexcept>

string readFile(const string& filePath) {
    ifstream file(filePath);

    if (!file.is_open()) {
        cout << "Error reading file!" << endl;
        return "";  //return an empty string to indicate an error
    }

    stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

int calculateUniqueChars(int totalNodes) {
    //function to calculate the minimum number of characters needed for uniqueness
    int uniqueChars = static_cast<int>(std::ceil(std::log2(totalNodes) / 4.0));

    return uniqueChars;
}

int calculateUniqueChars(const std::string& totalNodesStr) {
    // Validate the string to ensure it represents a non-negative integer
    for (char c : totalNodesStr) {
        if (!std::isdigit(c)) {
            throw std::invalid_argument("Input string must represent a non-negative integer.");
        }
    }

    // Handle special case for 0 and 1 node
    if (totalNodesStr == "0" || totalNodesStr == "1") {
        return 1;
    }

    // Compute the number of digits in the big integer
    int numDigits = totalNodesStr.length();

    // Calculate the approximate base-2 logarithm of the number
    double log2Estimate = (numDigits - 1) * std::log2(10)
        + std::log2(totalNodesStr[0] - '0');

    // Compute the unique characters required
    int uniqueChars = static_cast<int>(std::ceil(log2Estimate / 4.0));

    return uniqueChars;
}

std::string Hash(const std::string& input) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);

    //convert the hash to a hex string
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        ss << std::setw(2) << static_cast<unsigned int>(hash[i]);
    }

    return ss.str();
}

std::string getLastSubstring(const std::string& sha1Hash, int length) {
    if (length > static_cast<int>(sha1Hash.length())) {
        return sha1Hash; //return entire hash if length is greater
    }

    //calculate the starting index for the last substring
    int startIndex = static_cast<int>(sha1Hash.length()) - length;

    //extract the last substring
    std::string lastSubstring = sha1Hash.substr(static_cast<size_t>(startIndex), static_cast<size_t>(length));

    return lastSubstring;
}

std::string padString(const std::string& str, int desiredLength) {
    if (str.length() >= desiredLength) {
        return str; //string is already long enough
    }

    //calculate the number of leading zeros needed
    int zerosToAdd = desiredLength - str.length();

    std::string paddedString = std::string(zerosToAdd, '0') + str;

    return paddedString;
}


std::string subtractHexadecimalStrings(const std::string& hex1, const std::string& hex2) {
    // Ensure hex1 is greater than or equal to hex2
    if (hex1 < hex2) {
        std::cerr << "Error in hexa subtraction" << std::endl;
        return "";
    }

    //pad the strings with leading zeros
    size_t maxLength = hex1.length();
    std::string paddedHex2 = padString(hex2, maxLength);

    //perform hexadecimal subtraction
    std::string result;
    int borrow = 0;

    for (int i = maxLength - 1; i >= 0; --i) {
        int digit1 = (hex1[i] >= '0' && hex1[i] <= '9') ? (hex1[i] - '0') : (std::toupper(hex1[i]) - 'A' + 10);
        int digit2 = (paddedHex2[i] >= '0' && paddedHex2[i] <= '9') ? (paddedHex2[i] - '0') : (std::toupper(paddedHex2[i]) - 'A' + 10);

        int difference = digit1 - digit2 - borrow;

        if (difference < 0) {
            difference += 16;
            borrow = 1;
        }
        else {
            borrow = 0;
        }

        char diffDigit = (difference < 10) ? ('0' + difference) : ('a' + difference - 10);
        result = diffDigit + result;
    }

    //remove leading zeros from the result
    size_t firstNonZero = result.find_first_not_of('0');
    result = (firstNonZero != std::string::npos) ? result.substr(firstNonZero) : "0";

    return result;
}

std::string addHexadecimalStrings(const std::string& hex1, const std::string& hex2) {
    //pad the strings with leading zeros
    size_t maxLength = std::max(hex1.length(), hex2.length());
    std::string paddedHex1 = padString(hex1, maxLength);
    std::string paddedHex2 = padString(hex2, maxLength);

    // Perform hexadecimal addition
    std::string result;
    int carry = 0;

    for (int i = maxLength - 1; i >= 0; --i) {
        int digit1 = (paddedHex1[i] >= '0' && paddedHex1[i] <= '9') ? (paddedHex1[i] - '0') : (std::toupper(paddedHex1[i]) - 'A' + 10);
        int digit2 = (paddedHex2[i] >= '0' && paddedHex2[i] <= '9') ? (paddedHex2[i] - '0') : (std::toupper(paddedHex2[i]) - 'A' + 10);

        int sum = digit1 + digit2 + carry;
        carry = sum / 16;
        sum %= 16;

        char sumDigit = (sum < 10) ? ('0' + sum) : ('a' + sum - 10);
        result = sumDigit + result;
    }

    //if there is a carry after the loop, add it to the result
    if (carry > 0) {
        result = (char)('0' + carry) + result;
    }

    return result;
}


std::string powerOfTwoToHex(int exponent) {
    if (exponent < 0) {
        return "Invalid input. Exponent must be non-negative.";
    }

    if (exponent == 0) {
        return "1";
    }

    std::string result = "2";

    for (int i = 1; i < exponent; ++i) {
        result = addHexadecimalStrings(result, result);
    }

    return result;
}
