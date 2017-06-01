//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#ifndef AUDIODATATAG_H
#define AUDIODATATAG_H


#include<stddef.h>


#include<string>
#include<vector>
#include<iosfwd>


namespace FLV
{

class AudioDataTag
{
public:
    explicit AudioDataTag(std::ifstream &in);
    AudioDataTag(const AudioDataTag &)=delete;
    AudioDataTag& operator = (const AudioDataTag &)=delete;

    AudioDataTag(AudioDataTag &&)=default;


    void parse();
    void parseHeader();
    void parseBody();
    void parseTailer();


    std::vector<std::vector<std::string>> getPrintTable()const;
    void print(std::ostream &os, const std::string &prefix, const std::string &suffix);


    size_t tagDataLength()const { return m_tag_data_len; }
    size_t tagTs()const { return m_tag_ts; }



    int codecType()const { return m_codec_type; }
    int sampleRateType()const { return m_sample_rate_type; }
    int sampleDepthType()const { return m_sample_depth_type; }
    int soundType()const { return m_sound_type; }


    std::string codecTypeString()const;
    std::string sampleRateTypeString()const;
    std::string sampleDepthTypeString()const;
    std::string soundTypeString()const;


private:
    void readAudioHeader();

    void readTagHeader();
    void readTagData();
    void readPreTagSize();



private:
    void readBytes(char *arr, size_t bytes);
    template<typename T>
    void readBytes(T *arr, size_t bytes) { readBytes(reinterpret_cast<char*>(arr), bytes); }

    size_t readInt(size_t bytes);


private:
    void parseMetaData(char ch);

private:
    std::ifstream &m_in;

    bool m_start_count_read_bytes = false;
    size_t m_read_bytes = 0;
    size_t m_tag_data_len = 0;
    size_t m_tag_ts = 0;

    int m_codec_type = -1;
    int m_sample_rate_type = -1;
    int m_sample_depth_type = -1;
    int m_sound_type = -1;
};

}

#endif // AUDIODATATAG_H

