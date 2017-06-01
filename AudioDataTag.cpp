//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license


#include"AudioDataTag.h"


#include<assert.h>

#include<fstream>
#include<iostream>
#include<vector>
#include<string>


#include"helper.h"


using uchar = unsigned char;

namespace FLV
{


AudioDataTag::AudioDataTag(std::ifstream &in)
    : m_in(in)
{
}


void AudioDataTag::parse()
{
    parseHeader();
    parseBody();
    parseTailer();
}


//从第二个字节开始，第一个字节已经被client读取，用于确定是何种tag
void AudioDataTag::parseHeader()
{
    readTagHeader();
    m_start_count_read_bytes = true;
}


void AudioDataTag::parseBody()
{
    readTagData();
}


void AudioDataTag::parseTailer()
{
    readPreTagSize();
}



std::vector<std::vector<std::string>> AudioDataTag::getPrintTable()const
{
    std::vector<std::vector<std::string>> vec = {
        {"ts", "codec type", "sample rate", "sample depth", "sound type", "tag data len"},
        {std::to_string(m_tag_ts), codecTypeString(), sampleRateTypeString(), sampleDepthTypeString(), soundTypeString(), std::to_string(m_tag_data_len)}
    };

    return vec;
}



void AudioDataTag::print(std::ostream &os, const std::string &prefix, const std::string &suffix)
{
    os<<prefix<<"ts = "<<m_tag_ts<<"\tcodec type "<<codecTypeString()<<"\tsample rate = "<<sampleRateTypeString()
      <<"\tsample depth = "<<sampleDepthTypeString()<<"\tsound type ="<<soundTypeString()<<suffix;
}



std::string AudioDataTag::codecTypeString()const
{
    static std::string codec_type_str[] = {
        "unknown",

        "Linear PCM, platform endian",
        "ADPCM",
        "MP3",
        "Linear PCM, little endian",
        "Nellymoser 16 kHz mono",
        "Nellymoser 8 kHz mono",
        "Nellymoser",
        "G.711 A-law logarithmic PCM",
        "G.711 mu-law logarithmic PCM",
        "reserved",
        "AAC",
        "Speex",

        "None",//12 undefined
        "None",//13 undefined

        "MP3 8 KHz",
        "Device-specific sound"
    };

    return codec_type_str[m_codec_type + 1];
}


std::string AudioDataTag::sampleRateTypeString()const
{
    static std::string sample_rate_str[] = {
        "unknown",

        "5.5 kHz",
        "11 kHz",
        "22 kHz",
        "44 kHz"
    };

    return sample_rate_str[m_sample_rate_type + 1];
}


std::string AudioDataTag::sampleDepthTypeString()const
{
    static std::string sample_depth_str[] = {
        "unknown",

        "8-bit samples",
        "16-bit samples"
    };

    return sample_depth_str[m_sample_depth_type + 1];
}



std::string AudioDataTag::soundTypeString()const
{
    static std::string sound_type_str[] = {
        "unknown",

        "Mono sound",
        "Stereo sound"
    };

    return sound_type_str[m_sound_type + 1];
}


//=======================================================================================

void AudioDataTag::readBytes(char *arr, size_t bytes)
{
    m_in.read(arr, bytes);

    if( m_start_count_read_bytes )
        m_read_bytes += bytes;
}


size_t AudioDataTag::readInt(size_t bytes)
{
    if( bytes == 0)
        return 0;

    std::vector<uchar> vec(bytes);
    readBytes(vec.data(), bytes);
    size_t val = Helper::getSizeValue(vec);

    return val;
}


//从第二个字节开始，第一个字节已经被client读取，用于确定是何种tag
void AudioDataTag::readTagHeader()
{
    //读取tag data 长度
    m_tag_data_len = readInt(3);


    //读取tag 时间戳
    std::vector<uchar> ts(4);
    readBytes(ts.data()+1, 3);
    readBytes(ts.data(), 1);
    m_tag_ts = Helper::getSizeValue(ts);


    //读取stream id
    std::vector<uchar> stream_id(3);
    readBytes(stream_id.data(), stream_id.size());
}



void AudioDataTag::readAudioHeader()
{
    char ch;
    readBytes(&ch, 1);

    parseMetaData(ch);
    if( m_codec_type == 10)//AAC
    {
        //需要继续读取
    }
}


void AudioDataTag::readTagData()
{
    readAudioHeader();
    m_in.seekg(m_tag_data_len-m_read_bytes, std::ios_base::cur);
}


void AudioDataTag::readPreTagSize()
{
    size_t prev_tag_size = readInt(4);
    assert(prev_tag_size == m_tag_data_len + 11);

    (void)prev_tag_size;
}



void AudioDataTag::parseMetaData(char ch)
{
    m_codec_type = Helper::getNBits<4>(ch, 4);
    m_sample_rate_type = Helper::getNBits<2>(ch, 2);
    m_sample_depth_type = Helper::getNBits<1>(ch, 1);
    m_sound_type = Helper::getNBits<1>(ch, 0);
}


}
