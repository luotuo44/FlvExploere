//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license


#include"VideoDataTag.h"

#include<assert.h>

#include<vector>
#include<string>
#include<fstream>
#include<iostream>

#include"helper.h"

using uchar = unsigned char;

namespace FLV
{

VideoDataTag::VideoDataTag(std::ifstream &in)
    : m_in(in)
{
}



void VideoDataTag::parse()
{
    parseHeader();
    parseBody();
    parseTailer();
}


void VideoDataTag::parseHeader()
{
    readTagHeader();
    m_start_count_read_bypes = true;
}


void VideoDataTag::parseBody()
{
    readTagData();
}


void VideoDataTag::parseTailer()
{
    readPreTagSize();
}


std::vector<std::vector<std::string>> VideoDataTag::getPrintTable()const
{
    std::vector<std::vector<std::string>> vec = {
        {"ts", "frame type", "codec type", "tag data len"},
        {std::to_string(m_tag_ts), frameTypeString(), codecTypeString(), std::to_string(m_tag_data_len)}
    };

    if(m_codec_type == 7)//AVC
    {
        vec[0].insert(vec[0].end(), {"AVCPacketType", "CompositionTime"});
        vec[1].insert(vec[1].end(), {std::to_string(m_avc_packet_type), std::to_string(m_composition_time)});
    }

    return vec;
}


void VideoDataTag::print(std::ostream &os, const std::string &prefix, const std::string &suffix)
{
    os<<prefix<<"ts = "<<m_tag_ts<<"\tframe type = "<<frameTypeString()<<"\tcodec type = "<<codecTypeString()<<suffix;
}




std::string VideoDataTag::frameTypeString()const
{
    static std::string frame_type_str[] = {
        "unknown",
        "undefined",//0

        "key frame (for AVC, a seekable frame)",//1
        "inter frame (for AVC, a non-seekable frame)",//2
        "disposable inter frame (H.263 only)",
        "generated key frame (reserved for server use only)",
        "video info/command frame"
    };

    return frame_type_str[m_frame_type + 1];
}



std::string VideoDataTag::codecTypeString()const
{
    static std::string codec_type_str[] = {
        "unknown",
        "undefined",
        "undefined",

        "Sorenson H.263",//2
        "Screen video",
        "On2 VP6",
        "On2 VP6 with alpha channel",
        "Screen video version 2",
        "AVC"//7
    };

    return codec_type_str[m_codec_type + 1];
}



//=======================================================================


void VideoDataTag::readBytes(char *arr, size_t bytes)
{
    m_in.read(arr, bytes);

    if( m_start_count_read_bypes )
        m_read_bytes += bytes;
}


size_t VideoDataTag::readInt(size_t bytes)
{
    if( bytes == 0)
        return 0;

    std::vector<uchar> vec(bytes);
    readBytes(vec.data(), bytes);
    size_t val = Helper::getSizeValue(vec);

    return val;
}



//从第二个字节开始，第一个字节已经被client读取，用于确定是何种tag
void VideoDataTag::readTagHeader()
{
    //读取tag data 长度
    m_tag_data_len = readInt(3);//************************************
    //std::cout<<"tag_data_len = "<<m_tag_data_len<<std::endl;


    //读取tag 时间戳
    std::vector<uchar> ts(4);
    readBytes(ts.data()+1, 3);
    readBytes(ts.data(), 1);
    m_tag_ts = Helper::getSizeValue(ts);
    //std::cout<<"tag ts = "<<m_tag_ts<<std::endl;


    //读取stream id
    std::vector<uchar> stream_id(3);
    readBytes(stream_id.data(), stream_id.size());
}


void VideoDataTag::readVideoHeader()
{
    char ch;
    readBytes(&ch, 1);

    parseMetaData(ch);
    if( m_codec_type == 7 )//AVC
    {
        readBytes(&ch, 1);
        m_avc_packet_type = ch;
        m_composition_time = readInt(3);
    }
}



void VideoDataTag::readTagData()
{
    readVideoHeader();

    size_t data_len = m_tag_data_len - m_read_bytes;
    m_video_real_data.resize(data_len);
    m_in.read(reinterpret_cast<char*>(m_video_real_data.data()), data_len);

    //m_in.seekg(m_tag_data_len-m_read_bytes, std::ios_base::cur);
}



void VideoDataTag::readPreTagSize()
{
    size_t prev_tag_size = readInt(4);
    assert(prev_tag_size == m_tag_data_len + 11);

    (void)prev_tag_size;
}



void VideoDataTag::parseMetaData(char ch)
{
    m_frame_type = Helper::getNBits<4>(ch, 4);
    m_codec_type = Helper::getNBits<4>(ch, 0);
}




//----------------------------------------------------------------

std::vector<uchar> AVCConfigurationRecord::readNBytes(size_t num_bytes)
{
    if(m_read_pos + num_bytes > m_origin_data.size())
        throw std::out_of_range("out of read range");

    std::vector<uchar> vec(m_origin_data.begin()+m_read_pos, m_origin_data.begin()+m_read_pos+num_bytes);
    m_read_pos += num_bytes;

    return vec;
}


//https://www.jianshu.com/p/e1e417eee2e7
void AVCConfigurationRecord::parse()
{
    m_configuration_version = readNBytesToTT<uchar>(1);
    m_avc_profile_indication = readNBytesToTT<uchar>(1);
    m_profile_compatibility = readNBytesToTT<uchar>(1);
    m_avc_level_indication = readNBytesToTT<uchar>(1);


    uchar tmp = readNBytesToTT<uchar>(1);
    size_t first_reserved_bit = Helper::getNBits<6>(tmp, 2);
    if(first_reserved_bit != 0x3F)//111111
        throw std::runtime_error("reserve bit not equals to 111111");

    uchar length_size_minus_one = Helper::getNBits<2>(tmp, 0);
    (void)length_size_minus_one;

    tmp = readNBytesToTT<uchar>(1);
    size_t second_reserved_bit = Helper::getNBits<3>(tmp, 5);
    if(second_reserved_bit != 7)
        throw std::runtime_error("reserve bit not equals to 111");

    size_t num_of_sequence_parameter_sets = Helper::getNBits<5>(tmp, 0);
    m_sequence_parameter_set_nal_unit.reserve(num_of_sequence_parameter_sets);
    for(size_t i = 0; i < num_of_sequence_parameter_sets; ++i)
    {
        size_t len = readNBytesToTT<size_t>(2);
        m_sequence_parameter_set_nal_unit.push_back(readNBytes(len));
    }


    size_t num_of_picture_parameter_sets = readNBytesToTT<size_t>(1);
    m_picture_parameter_set_nal_unit.reserve(num_of_picture_parameter_sets);
    for(size_t i = 0; i < num_of_picture_parameter_sets; ++i)
    {
        size_t len = readNBytesToTT<size_t>(2);
        m_picture_parameter_set_nal_unit.push_back(readNBytes(len));
    }
}

}
