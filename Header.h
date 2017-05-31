//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#ifndef HEADER_H
#define HEADER_H

#include<stddef.h>

#include<iosfwd>

namespace FLV
{

class Header
{
public:
    explicit Header(std::ifstream &in);
    Header(const Header &)=delete;
    Header& operator = (const Header &)=delete;
    Header(Header &&)=default;


public:
    void parse();

    int version()const { return m_version; }
    bool hasVideoStream()const { return m_has_video_stream; }
    bool hasAudioStream()const { return m_has_audio_stream; }
    size_t headerSize()const { return m_header_size; }

private:
    std::ifstream &m_in;

    int m_version = 1;
    bool m_has_video_stream = 0;
    bool m_has_audio_stream = 0;
    size_t m_header_size = 0;
};


}

#endif // HEADER_H

