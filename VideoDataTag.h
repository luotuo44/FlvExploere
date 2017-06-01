//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#ifndef VIDEODATATAG_H
#define VIDEODATATAG_H


#include<vector>
#include<string>
#include<iosfwd>


namespace FLV
{

class VideoDataTag
{
public:
    explicit VideoDataTag(std::ifstream &in);
    VideoDataTag(const VideoDataTag &)=delete;
    VideoDataTag& operator = (const VideoDataTag &)=delete;
    VideoDataTag(VideoDataTag &&)=default;


    void parse();
    void parseHeader();
    void parseBody();
    void parseTailer();


    std::vector<std::vector<std::string>> getPrintTable()const;
    void print(std::ostream &os, const std::string &prefix, const std::string &suffix);


    int frameType()const { return m_frame_type; }
    int codecType()const { return m_codec_type; }

    std::string frameTypeString()const;
    std::string codecTypeString()const;


private:
    void readVideoHeader();

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

    bool m_start_count_read_bypes = false;
    size_t m_read_bytes = 0;
    size_t m_tag_data_len = 0;
    size_t m_tag_ts = 0;


    int m_frame_type = -1;
    int m_codec_type = -1;
};

}

#endif // VIDEODATATAG_H

