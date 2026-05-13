#pragma once
#include <string_view>
#include <charconv>
#include <string>

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

class Parser {
public:
    template<typename T>
    static bool parse(std::string_view& sv, T& out) {
        return ParseTraits<T>::parse(sv, out);
    }
};