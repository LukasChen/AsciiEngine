#pragma once
#include <string_view>
#include <charconv>
#include <string>
#include <cctype>

template<typename T>
struct ParseTraits;

template<>
struct ParseTraits<std::string> {
    static bool parse(std::string_view& sv, std::string& out) {
        while (!sv.empty() && std::isspace(sv.front())) sv.remove_prefix(1);
        size_t endPos = sv.find_first_of(' ');
        out = std::string(sv.substr(0, endPos));
        if (endPos != std::string_view::npos) {
            sv.remove_prefix(endPos);
        } else {
            sv.remove_prefix(sv.size());
        }
        return true;
    }
};

template<>
struct ParseTraits<float> {
    static bool parse(std::string_view& sv, float& out) {
        while(!sv.empty() && (std::isspace(sv.front()) || sv.front() == ',')) sv.remove_prefix(1);
        const char* begin = sv.data();
        const char* end = begin + sv.size();
        auto result = std::from_chars(begin, end, out);
        if (result.ec == std::errc()) {
            sv.remove_prefix(result.ptr - begin);
            return true;
        }
        return false;
    }
};

template<>
struct ParseTraits<Vec3> {
    static bool parse(std::string_view& sv, Vec3& out) {
        return ParseTraits<float>::parse(sv, out.x) &&
               ParseTraits<float>::parse(sv, out.y) &&
               ParseTraits<float>::parse(sv, out.z);
    }
};

template<>
struct ParseTraits<bool> {
    static bool hasPrefix(std::string_view value, std::string_view prefix) {
        return value.size() >= prefix.size() && value.substr(0, prefix.size()) == prefix;
    }
    static bool parse(std::string_view& sv, bool& out) {
        while (!sv.empty() && (std::isspace(static_cast<unsigned char>(sv.front())) || sv.front() == ',')) {
            sv.remove_prefix(1);
        }

        if (hasPrefix(sv, "true")) {
            out = true;
            sv.remove_prefix(4);
            return true;
        }

        if (hasPrefix(sv, "false")) {
            out = false;
            sv.remove_prefix(5);
            return true;
        }

        if (hasPrefix(sv, "1")) {
            out = true;
            sv.remove_prefix(1);
            return true;
        }

        if (hasPrefix(sv, "0")) {
            out = false;
            sv.remove_prefix(1);
            return true;
        }

        return false;
    }
};

class Parser {
public:
    template<typename T>
    static bool parse(std::string_view& sv, T& out) {
        return ParseTraits<T>::parse(sv, out);
    }
};