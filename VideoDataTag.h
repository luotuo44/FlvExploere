//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#ifndef VIDEODATATAG_H
#define VIDEODATATAG_H


#include<vector>
#include<string>
#include<iosfwd>
#include<stdexcept>


using uchar = unsigned char;

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

    size_t tagTs()const { return m_tag_ts; }

    int frameType()const { return m_frame_type; }
    int codecType()const { return m_codec_type; }
    int avcPacketType()const { return m_avc_packet_type; }
    int compositionTime()const { return m_composition_time; }
    const std::vector<uchar>& videoRealData()const { return m_video_real_data; }

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


    int m_avc_packet_type = 0;
    int m_composition_time = 0;
    std::vector<uchar> m_video_real_data;
};



//------------------------------------------------------------

class AVCConfigurationRecord
{
public:
    template<typename InputIterator>
    AVCConfigurationRecord(InputIterator begin, InputIterator end);

    void parse();

public:
    uchar configurationVersion()const { return m_configuration_version; }
    uchar AVCProfileIndication()const { return m_avc_profile_indication; }
    uchar profile_compatibility()const { return m_profile_compatibility; }
    uchar AVCLevelIndication()const { return m_avc_level_indication; }
    const std::vector<std::vector<uchar>>& sequenceParameterSetNALUnit()const {return m_sequence_parameter_set_nal_unit; }
    const std::vector<std::vector<uchar>>& pictureParameterSetNALUnit()const {return m_picture_parameter_set_nal_unit; }

private:
    template<typename TT>
    TT readNBytesToTT(size_t num_bytes);
    std::vector<uchar> readNBytes(size_t num_bytes);

private:
    uchar m_configuration_version = 1;
    uchar m_avc_profile_indication;
    uchar m_profile_compatibility;
    uchar m_avc_level_indication;

    uchar m_length_size_minus_one;

    std::vector<std::vector<uchar>> m_sequence_parameter_set_nal_unit;
    std::vector<std::vector<uchar>> m_picture_parameter_set_nal_unit;

    std::vector<uchar> m_origin_data;
    size_t m_read_pos = 0;
};


template<typename InputIterator>
AVCConfigurationRecord::AVCConfigurationRecord(InputIterator begin, InputIterator end)
    : m_origin_data(begin, end)
{
    parse();
}


template<typename TT>
TT AVCConfigurationRecord::readNBytesToTT(size_t num_bytes)
{
    if(m_read_pos + num_bytes > m_origin_data.size())
        throw std::out_of_range("out of read range");

    TT ret = 0;
    for(size_t i = 0; i < num_bytes; ++i)
    {
        ret = (ret<<8) + m_origin_data[m_read_pos++];
    }

    return ret;
}

}

#endif // VIDEODATATAG_H

