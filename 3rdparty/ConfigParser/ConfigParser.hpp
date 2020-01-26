#pragma once

#include <string>
#include <sstream>
#include <ios> // boolalpha
#include <vector>
#include <unordered_map>
#include <utility> // pair

class ConfigParser {
  public:
    ConfigParser(const std::string& filename);
    ~ConfigParser();

    template <typename T, typename... Args>
    void get(const std::string& key, T& value, Args&... args) const;
    template <typename T>
    void get(const std::string& key, std::vector<T>& vector) const;

    template <typename T, typename... Args>
    void set(const std::string& key, const T& value, const Args&... args);
    template <typename T>
    void set(const std::string& key, const std::vector<T>& vector);

  private:
    typedef std::pair<std::string, std::string> KeyValuePair;

    template <typename T>
    void toValue(std::string& str, T& value) const;
    template <typename T>
    void toValue(std::istringstream& iss, T& value) const;

    void trim(std::string& string) const; // trim whitespace from string
    KeyValuePair parse(const std::string& line) const;

  private:
    bool modified = false;
    std::string filename;
    std::unordered_map<std::string, std::string> configMap;
};

#include "ConfigParser.inl"
