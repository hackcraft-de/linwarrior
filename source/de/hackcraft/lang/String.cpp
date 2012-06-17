#include "String.h"


String::String() {
}

String::String(const char* s) {
    value = s;
}

String::~String() {
    //cout << "~String()";
}

bool String::equals(String* str) {
    if (str == NULL) return false;
    //std::cout << value << " ?= " << str->value << "\n";
    return (value.compare(str->value) == 0);
}

int String::hashCode() {
    int code = 0;
    int n = value.size();
    for (int i = 0; i < n; i++) {
            unsigned char c = value.at(i);
            code += ((code << 5) - code) + c;
    }
    return code;
}

String* String::toString() {
    return this;
}

const char* String::c_str() {
    return value.c_str();
}

