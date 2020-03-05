#pragma once

#include <string>
#include <vector>

class StringUtils {
public:
  static void split(std::vector<std::string> &results, const std::string &input,
                    char character) {
    size_t previous = 0;
    for (size_t i = 0, sz = input.size(); i < sz; i++) {
      if (input[i] == character) {
        results.emplace_back(&input[previous], &input[i]);
        previous = i + 1;
      }
    }
    results.emplace_back(&input[previous], &input[input.size()]);
  }

  template <typename T> static T lexical_cast(const std::string &input) {
    uint64_t n = 0;
    bool neg = input[0] == '-';
    const char *s = input.c_str();
    for (size_t j = neg ? 1 : 0, ssz = input.size(); j < ssz; j++) {
      char c = s[j] - '0';
      n = n * 10 + c;
    }
#pragma warning(push)
#pragma warning(disable : 4146)
    return neg ? -static_cast<T>(n) : static_cast<T>(n);
#pragma warning(pop)
  }

  static void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    [](int ch) { return !std::isspace(ch); }));
  }

  // trim from end (in place)
  static void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](int ch) { return !std::isspace(ch); })
                .base(),
            s.end());
  }
};