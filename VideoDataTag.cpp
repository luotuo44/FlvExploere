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

    }
}



void VideoDataTag::readTagData()
{
    readVideoHeader();

    m_in.seekg(m_tag_data_len-m_read_bytes, std::ios_base::cur);
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


}
