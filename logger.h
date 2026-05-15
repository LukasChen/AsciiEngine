#pragma once
#include <iostream>
#include <fstream>
#include <string>

class Logger {
public:
    static void init(const std::string& filename) {
        m_logFile.open(filename, std::ios::out | std::ios::trunc);
        if (!m_logFile.is_open()) {
            std::cerr << "Failed to open log file: " << filename << std::endl;
        }
    }
    static void log(const std::string& message) {
        if (!m_logFile.is_open()) {
            return;
        }
        m_logFile << message << std::endl;
    }

    static void close() {
        if (m_logFile.is_open()) {
            m_logFile.close();
        }
    }
private:
    inline static std::ofstream m_logFile;
};
