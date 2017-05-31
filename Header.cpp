//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license


#include"Header.h"


#include<string>
#include<exception>
#include<stdexcept>
#include<fstream>
#include<iostream>


#include"helper.h"


using uchar = unsigned char;

namespace FLV
{

Header::Header(std::ifstream &in)
    : m_in(in)
{
}


void Header::parse()
{
    std::string format(3, '\0');
    m_in.read(&format[0], 3);
    if(format != "FLV")
    {
        throw std::invalid_argument("not FLV format");
    }


    char version;
    m_in.read(&version, 1);
    m_version = static_cast<int>(version);


    char flag;
    m_in.read(&flag, 1);
    m_has_video_stream = Helper::getNBits<1>(flag, 0);
    m_has_audio_stream = Helper::getNBits<1>(flag, 2);


    uchar header_size_arr[4];
    m_in.read(reinterpret_cast<char*>(header_size_arr), 4);
    m_header_size = Helper::getSizeValue(header_size_arr);
}

}
