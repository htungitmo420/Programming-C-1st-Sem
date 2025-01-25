#include "number.h"
#include <cstring>
#include <stdexcept>
#include <string>

uint239_t ConvertToZeroShift(uint239_t value){
    for (int i = 0; i < 17; i++)
    {
        uint8_t temp = value.data[i];
        value.data[i] = value.data[34 - i];
        value.data[34 - i] = temp;
    }
    uint239_t result = uint239_t();
    uint64_t shift = 0;
    for (int i = 0; i < 35; i++){
        uint64_t bit = (value.data[i] >> 7) & 1;
        shift = shift | (bit << i);
    } 

    shift = shift % 245;
    for (uint32_t i = 0; i < 245; i++) {
        uint8_t bit = (value.data[i / 7] >> (i % 7)) & 1;
        uint64_t newPos = (i - shift) % 245;
        result.data[newPos / 7] = result.data[newPos / 7] | (bit << (newPos % 7));
    }
    return result;
}

uint239_t FromInt(uint32_t value, uint32_t shift) {
    uint32_t temp = shift;
    uint239_t value_239 = uint239_t();
    int block = 0;
    while (value != 0)
    {
        value_239.data[block] = value & 127;
        value = value >> 7;
        block++;
    }

    uint239_t des = uint239_t();
    
    shift = shift % 245;
    for (uint32_t i = 0; i < 245; i++)
    {
        uint8_t bit = (value_239.data[i / 7] >> (i % 7)) & 1;
        uint64_t new_pos = (i + shift) % 245;
        des.data[new_pos / 7] = des.data[new_pos / 7] | (bit << (new_pos % 7));
    }

    block = 0;
    while (temp != 0)
    {
        uint8_t bit = (temp & 1) << 7;
        des.data[block] = des.data[block] | bit;
        temp = temp >> 1;
        block++;
    }

    for (int i = 0; i < 17; i++)
    {
        uint8_t temp = des.data[i];
        des.data[i] = des.data[34 - i];
        des.data[34 - i] = temp;
    }
    return des;
}

std::string DivideByTwo(std::string& number){
    std::string result;
    int remainder = 0;

    for (char digit : number) {
        int current = remainder * 10 + (digit - '0');  
        result.push_back(current / 2 + '0');            
        remainder = current % 2;                        
    }

    while (result[0] == '0' && result.length() > 1){
        result = result.substr(1);
    }

    return result;
}

std::string BigNumberToBinary(std::string number) {
    std::string binary;

    while (number != "0") {
        int lastDigit = number.back() - '0';
        binary.push_back((lastDigit % 2) + '0');  
        number = DivideByTwo(number);                   
    }

    int n = binary.length(); 

    for (int i = 0; i < n / 2; i++) {
        char temp = binary[i];                
        binary[i] = binary[n - i - 1];          
        binary[n - i - 1] = temp;             
    }

    return binary;
}


uint239_t FromString(const char* str, uint32_t shift) {

    uint32_t temp = shift;
    uint239_t string239 = uint239_t();

    std::string strBinary = str;
    strBinary = BigNumberToBinary(strBinary);
    for (int i = strBinary.length() - 1, j = 0; i >= 0 ; i--, j++){
        uint8_t bit = strBinary[i] - 48;
        string239.data[j / 7] = string239.data[j / 7] | (bit << (j % 7));
    }

    uint239_t des = uint239_t();
    shift = shift % 245;
    for (uint32_t i = 0; i < 245; i++) {
        uint8_t bit = (string239.data[i / 7] >> (i % 7)) & 1;
        uint64_t newPos = (i + shift) % 245;
        des.data[newPos / 7] = des.data[newPos / 7] | (bit << (newPos % 7));
    }

    int block = 0;
    
    while (temp != 0) {
        uint8_t bit = (temp & 1) << 7;
        des.data[block] = des.data[block] | bit;
        temp = temp >> 1;
        block++;
    }
    for (int i = 0; i < 17; i++)
    {
        uint8_t temp = des.data[i];
        des.data[i] = des.data[34 - i];
        des.data[34 - i] = temp;
    }
    return des;
}

uint239_t operator+(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t left = ConvertToZeroShift(lhs);
    uint239_t right = ConvertToZeroShift(rhs);
    uint239_t result;
    uint8_t carry = 0;

    for (int i = 0; i < 35; ++i) {
        uint8_t left_byte = left.data[i] & 127;
        uint8_t right_byte = right.data[i] & 127;
        uint16_t sum = left_byte + right_byte + carry;
        result.data[i] = sum & 127;
        carry = (sum >> 7) & 1;
    }
    for (int i = 0; i < 17; i++)
    {
        uint8_t temp = result.data[i];
        result.data[i] = result.data[34 - i];
        result.data[34 - i] = temp;
    }
    return result;
}

uint239_t operator-(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t left = ConvertToZeroShift(lhs);
    uint239_t right = ConvertToZeroShift(rhs);
    uint239_t result;
    int8_t borrow = 0;

    for (int i = 0; i < 35; ++i) {
        uint8_t left_byte = left.data[i] & 127;
        uint8_t right_byte = right.data[i] & 127;
        int16_t diff = left_byte - right_byte - borrow;
        if (diff < 0) {
            diff += 128;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result.data[i] = diff & 127;
    }
    
    for (int i = 0; i < 17; i++) {
        uint8_t temp = result.data[i];
        result.data[i] = result.data[34 - i];
        result.data[34 - i] = temp;
    }

    return result;
}

uint239_t operator*(const uint239_t& lhs, const uint239_t& rhs) { 
    uint239_t left = ConvertToZeroShift(lhs);
    uint239_t right = ConvertToZeroShift(rhs);
    uint239_t result = uint239_t();
    
    for (int i = 0; i < 35; ++i) {
        uint8_t left_byte = left.data[i] & 127;
        uint16_t carry = 0;
        for (int j = 0; j < 35 && i + j < 35; ++j) {
            uint8_t right_byte = right.data[j] & 127;
            uint16_t product = left_byte * right_byte + carry + result.data[i + j];
            result.data[i + j] = product & 127;
            carry = product >> 7;
        }

        if (i + 35 < 35) {
            result.data[i + 35] += carry;
        }
    }

    for (int i = 0; i < 17; i++) {
        uint8_t temp = result.data[i];
        result.data[i] = result.data[34 - i];
        result.data[34 - i] = temp;
    }

    return result;
}

std::string ToString(uint239_t value){
    std::string result = "";
    for (int i = 0; i < 35; i++){
        for (int j = 0; j < 7; j++){
            char bit = ((value.data[i] >> j) & 1) + '0';
            result = bit + result;
        }
    }
    while (result[0] == '0' && result.length() > 1){
        result = result.substr(1);
    }
    return result;
}

bool IsGreaterOrEqual(const std::string& a, const std::string& b) {
    if (a.length() != b.length()) {
        return a.length() > b.length();
    }
    return a >= b;
}

std::string SubtractBinary(const std::string& a, const std::string& b) {
    std::string result = "";
    int carry = 0, i = a.length() - 1, j = b.length() - 1;
    
    while (i >= 0 || j >= 0 || carry) {
        int bitA = i >= 0 ? a[i--] - '0' : 0;
        int bitB = j >= 0 ? b[j--] - '0' : 0;
        int sum = bitA - bitB - carry;
        
        if (sum < 0) {
            sum += 2;
            carry = 1;
        } else {
            carry = 0;
        }
        result = std::to_string(sum) + result;
    }
    
    while (result[0] == '0' && result.length() > 1){
        result = result.substr(1);
    }
    return result;
}

uint239_t operator/(const uint239_t& lhs, const uint239_t& rhs) {

    std::string left = ToString(ConvertToZeroShift(lhs));
    std::string right = ToString(ConvertToZeroShift(rhs));

    if (right == "0") {
        throw std::runtime_error("Division by zero");
    }

    std::string quotient = "";
    std::string remainder = "";

    for (size_t i = 0; i < left.size(); ++i) {
        remainder += left[i]; 

        if (remainder[0] == '0' && remainder.length() > 1) {
            remainder = remainder.substr(1);
        }

        if (IsGreaterOrEqual(remainder, right)) {
            remainder = SubtractBinary(remainder, right);
            quotient += '1';
        } else {
            quotient += '0';
        }
    }
    uint239_t result = uint239_t();
    for (int i = quotient.length() - 1, j = 0; i >= 0 ; i--, j++){
        uint8_t bit = quotient[i] - 48;
        result.data[j / 7] = result.data[j / 7] | (bit << (j % 7));
    }

    for (int i = 0; i < 17; i++) {
        uint8_t temp = result.data[i];
        result.data[i] = result.data[34 - i];
        result.data[34 - i] = temp;
    }
    return result;
}


bool operator==(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t left = ConvertToZeroShift(lhs);
    uint239_t right = ConvertToZeroShift(rhs);
    for (int i = 0; i < 35; i++) {
        if (left.data[i] != right.data[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const uint239_t& lhs, const uint239_t& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& stream, const uint239_t& value) {
    for (int i = 34; i >= 0; --i) {
        stream << value.data[i];
    }
    return stream;
}

uint64_t GetShift(const uint239_t& value) {
    uint239_t temp_value = value;
    for (int i = 0; i < 17; i++)
    {
        uint8_t temp = temp_value.data[i];
        temp_value.data[i] = temp_value.data[34 - i];
        temp_value.data[34 - i] = temp;
    }
    uint64_t shift = 0;
    for (int i = 0; i < 35; i++){
        uint64_t bit = (value.data[i] >> 7) & 1;
        shift = shift | (bit << i);
    }
    return shift;
}