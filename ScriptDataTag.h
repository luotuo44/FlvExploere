//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#ifndef SCRIPTDATATAG_H
#define SCRIPTDATATAG_H

#include<stddef.h>
#include<stdint.h>

#include<iosfwd>
#include<utility>
#include<map>
#include<functional>


namespace FLV
{

class ScriptDataTag
{
public:
    explicit ScriptDataTag(std::ifstream &in);

    ScriptDataTag(const ScriptDataTag &)=delete;
    ScriptDataTag& operator = (const ScriptDataTag &)=delete;
    ScriptDataTag(ScriptDataTag &&)=default;

    void parse();
    void parseHeader();
    void parseBody();
    void parseTailer();


    size_t tagDataLength()const { return m_tag_data_len; }
    size_t tagTs()const { return m_tag_ts; }

private:
    void readTagHeader();
    void readTagData();
    void readPreTagSize();


    std::string smartIndent()const;

private:
    size_t readInt(size_t bytes);
    double readDouble();
    std::string readString(char type);//short string or long string
    std::pair<double, int16_t> readDate();


    void readBytes(char *arr, size_t bytes);
    template<typename T>
    void readBytes(T *arr, size_t bytes) { readBytes(reinterpret_cast<char*>(arr), bytes); }


    bool parseObjectProperty();
    void parseObject();
    void parseEcmaArray();
    void parseStrictArray();

    void parseDataValue();



private:
    std::ifstream &m_in;
    size_t m_read_bytes = 0;
    size_t m_tag_data_len = 0;
    size_t m_tag_ts = 0;
    size_t m_smartindent = 0;

    std::map<int, std::function<void ()>> m_op;
};


}

#endif // SCRIPTDATATAG_H

