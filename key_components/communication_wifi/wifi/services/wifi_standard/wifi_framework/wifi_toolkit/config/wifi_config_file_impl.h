/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_WIFI_CONFIG_FILE_IMPL_H
#define OHOS_WIFI_CONFIG_FILE_IMPL_H
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include "wifi_config_file_spec.h"
#include "wifi_log.h"

namespace OHOS {
namespace Wifi {
/**
 * @Description Remove head and tail space
 *
 * @param str - String
 */
static inline void TrimString(std::string &str)
{
    int i = 0;
    int j = static_cast<int>(str.length()) - 1;
    while (i < static_cast<int>(str.length()) && str[i] == ' ') {
        ++i;
    }
    while (j >= 0 && str[j] == ' ') {
        --j;
    }
    str = ((i > j) ? "" : str.substr(i, j - i + 1));
}

/**
 * @Description Delete comment message begin with ; and #
 *
 * @param str - String
 */
static inline void DelComment(std::string &str)
{
    std::string::size_type i = 0;
    for (; i < str.length(); ++i) {
        if (str[i] == ';' || str[i] == '#') {
            str = str.substr(0, i);
            break;
        }
    }
    return;
}

template<typename T>
class WifiConfigFileImpl {
public:
    /**
     * @Description Set the config file path
     *
     * @param fileName - file name
     * @return int - 0 success
     */
    int SetConfigFilePath(const std::string &fileName);

    /**
     * @Description read and parses the network section of ini config file, need call SetConfigFilePath first
     *
     * @return int - 0 Success; >0 parse failed
     */
    int ReadNetworkSection(T &item, std::ifstream &fs, std::string &line);

    /**
     * @Description read and parses the networks of ini config file, need call SetConfigFilePath first
     *
     * @return int - 0 Success; >0 parse failed
     */
    int ReadNetwork(T &item, std::ifstream &fs, std::string &line);

    /**
     * @Description read and parses the ini config file, need call SetConfigFilePath first
     *
     * @return int - 0 Success; -1 file not exist
     */
    int LoadConfig();

    /**
     * @Description Save config to file
     *
     * @return int - 0 Success; -1 Failed
     */
    int SaveConfig();

    /**
     * @Description Get config values
     *
     * @param results - output config values
     * @return int - 0 Success, -1 Failed
     */
    int GetValue(std::vector<T> &results);

    /**
     * @Description Get config values
     *
     * @return config values
     */
    const std::vector<T>& GetValue() const;

    /**
     * @Description Set the config value
     *
     * @param values - input config values
     * @return int - 0 Success, -1 Failed
     */
    int SetValue(const std::vector<T> &values);

private:
    std::string mFileName;
    std::vector<T> mValues;
};

template<typename T>
int WifiConfigFileImpl<T>::SetConfigFilePath(const std::string &fileName)
{
    mFileName = fileName;
    return 0;
}

template<typename T>
int WifiConfigFileImpl<T>::ReadNetworkSection(T &item, std::ifstream &fs, std::string &line)
{
    int sectionError = 0;
    while (std::getline(fs, line)) {
        TrimString(line);
        if (line.empty()) {
            continue;
        }
        if (line[0] == '<' && line[line.length() - 1] == '>') {
            return sectionError;
        }
        std::string::size_type npos = line.find("=");
        if (npos == std::string::npos) {
            LOGE("Invalid config line");
            sectionError++;
            continue;
        }
        std::string key = line.substr(0, npos);
        std::string value = line.substr(npos + 1);
        TrimString(key);
        TrimString(value);
        /* template function, needing specialization */
        sectionError += SetTClassKeyValue(item, key, value);
    }
    LOGE("Section config not end correctly");
    sectionError++;
    return sectionError;
}

template<typename T>
int WifiConfigFileImpl<T>::ReadNetwork(T &item, std::ifstream &fs, std::string &line)
{
    int networkError = 0;
    while (std::getline(fs, line)) {
        TrimString(line);
        if (line.empty()) {
            continue;
        }
        if (line[0] == '<' && line[line.length() - 1] == '>') {
            networkError += ReadNetworkSection(item, fs, line);
        } else if (line.compare("}") == 0) {
            return networkError;
        } else {
            LOGE("Invalid config line");
            networkError++;
        }
    }
    LOGE("Network config not end correctly");
    networkError++;
    return networkError;
}

template<typename T>
int WifiConfigFileImpl<T>::LoadConfig()
{
    if (mFileName.empty()) {
        LOGE("File name is empty.");
        return -1;
    }
    std::ifstream fs(mFileName.c_str());
    if (!fs.is_open()) {
        LOGE("Loading config file: %{public}s, fs.is_open() failed!", mFileName.c_str());
        return -1;
    }
    mValues.clear();
    T item;
    std::string line;
    int configError;
    while (std::getline(fs, line)) {
        TrimString(line);
        if (line.empty()) {
            continue;
        }
        if (line[0] == '[' && line[line.length() - 1] == '{') {
            ClearTClass(item); /* template function, needing specialization */
            configError = ReadNetwork(item, fs, line);
            if (configError > 0) {
                LOGE("Parse network failed.");
                continue;
            }
            mValues.push_back(item);
        }
    }
    fs.close();
    return 0;
}

template<typename T>
int WifiConfigFileImpl<T>::SaveConfig()
{
    if (mFileName.empty()) {
        LOGE("File name is empty.");
        return -1;
    }
    FILE* fp = fopen(mFileName.c_str(), "w");
    if (!fp) {
        LOGE("Save config file: %{public}s, fopen() failed!", mFileName.c_str());
        return -1;
    }
    std::ostringstream ss;
    for (std::size_t i = 0; i < mValues.size(); ++i) {
        T &item = mValues[i];
        /*
         * here use template function GetTClassName OutTClassString, needing
         * specialization.
         */
        ss << "[" << GetTClassName<T>() << "_" << (i + 1) << "] {" << std::endl;
        ss << OutTClassString(item) << std::endl;
        ss << "}" << std::endl;
    }
    std::string content = ss.str();
    int ret = fwrite(content.c_str(), 1, content.length(), fp);
    if (ret != (int)content.length()) {
        LOGE("Save config file: %{public}s, fwrite() failed!", mFileName.c_str());
    }
    (void)fflush(fp);
    (void)fsync(fileno(fp));
    (void)fclose(fp);
    mValues.clear(); /* clear values */
    return 0;
}

template<typename T>
int WifiConfigFileImpl<T>::GetValue(std::vector<T> &results)
{
    /*
     * swap, WifiConfigFileImpl not saved this config when next use, call LoadConfig first
     */
    std::swap(results, mValues);
    return 0;
}

template <typename T>
const std::vector<T>& WifiConfigFileImpl<T>::GetValue() const
{
    return mValues;
}

template <typename T>
int WifiConfigFileImpl<T>::SetValue(const std::vector<T> &results)
{
    mValues = results;
    return 0;
}
}  // namespace Wifi
}  // namespace OHOS
#endif