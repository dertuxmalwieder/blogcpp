template <typename T>
void ConfigParser::toValue(std::string& str, T& value) const
{
    std::istringstream iss(str);
    iss >> std::boolalpha >> value;
}

template <>
inline void ConfigParser::toValue<std::string>(std::string& str, std::string& value) const
{
    trim(str);
    value = str;
}

template <typename T>
void ConfigParser::toValue(std::istringstream& iss, T& value) const
{
    std::string str;
    if (std::getline(iss, str, ','))
        toValue(str, value);
}

template <typename T, typename... Args>
void ConfigParser::get(const std::string& key, T& value, Args&... args) const
{
    auto found = configMap.find(key);
    if (found != configMap.end())
    {
        std::istringstream iss(found->second);
        toValue(iss, value);
        int dummy[] = { 0, (toValue(iss, args), 0) ... }; // parameter pack expansion
    }
}

template <>
inline void ConfigParser::get<std::string>(const std::string& key, std::string& value) const
{
    auto found = configMap.find(key);
    if (found != configMap.end())
        value = found->second;
}

template <typename T>
void ConfigParser::get(const std::string& key, std::vector<T>& vector) const
{
    auto found = configMap.find(key);
    if (found != configMap.end())
    {
        std::istringstream iss(found->second);
        std::string str;
        while (std::getline(iss, str, ','))
        {
            vector.emplace_back();
            std::istringstream(str) >> std::boolalpha >> vector.back();
        }
    }
}

template <typename T, typename... Args>
void ConfigParser::set(const std::string& key, const T& value, const Args&... args)
{
    std::ostringstream oss;
    oss << std::boolalpha << value;
    int dummy[] = { 0, (oss << ", " << args, 0) ... }; // parameter pack expansion

    if (configMap[key] != oss.str())
    {
        configMap[key] = oss.str();
        modified = true;
    }
}

template <typename T>
void ConfigParser::set(const std::string& key, const std::vector<T>& vector)
{
    std::ostringstream oss;
    oss << std::boolalpha;
    for (auto it = vector.begin(); it != vector.end(); ++it)
    {
        if (it != vector.begin())
            oss << ", ";
        oss << *it;
    }

    if (configMap[key] != oss.str())
    {
        configMap[key] = oss.str();
        modified = true;
    }
}
