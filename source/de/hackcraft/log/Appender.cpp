#include "Appender.h"

#include <iostream>

Appender::Appender() {
}

Appender::~Appender() {
}

void Appender::append(const char* loggerOutput) {
    std::cout << loggerOutput;
}

