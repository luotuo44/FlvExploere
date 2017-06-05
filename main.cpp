//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license


#include<stdint.h>


#include<iostream>
#include<fstream>
#include<string>
#include<vector>


#include"Header.h"
#include"ScriptDataTag.h"
#include"helper.h"
#include"AudioDataTag.h"
#include"VideoDataTag.h"
#include"MarkdownPrinter.h"


using uchar = unsigned char;


int main(int argc, char **argv)
{
    if(argc < 2)
    {
        std::cout<<"usage: "<<argv[0]<<" flv_filename [print_tag_num(default 100, -1 means print all tag)]"<<std::endl;
        return 0;
    }

    std::string filename(argv[1]);

    std::ifstream in(filename.c_str(), std::ios::binary);
    if(!in)
    {
        std::cout<<"fail to open "<<filename<<std::endl;
        return -1;
    }


    size_t print_tag_num = argc > 2 ? std::stoi(argv[2]) : 100;


    FLV::Header header(in);
    header.parse();

    std::vector<StringVector> header_table = {
        {"filetype", "version", "hasVideoStream", "hasAudioStream"},
        {"FLV", std::to_string(header.version()), std::to_string(header.hasVideoStream()), std::to_string(header.hasAudioStream())},
    };

    MP::printTitle(1, "FLV Header");
    MP::printTable(header_table);


    std::vector<uchar> vec(4);
    in.read(reinterpret_cast<char*>(vec.data()), vec.size());
    size_t first_prev_len = FLV::Helper::getSizeValue(vec);
    (void)first_prev_len;


    size_t count = 0;
    char type;
    while(in.read(&type, 1) && count < print_tag_num)
    {
        ++count;
        switch(type)
        {
            case 0x08:{
                MP::printTitle(2, "AudioData Tag");
                FLV::AudioDataTag ad(in);
                ad.parse();
                MP::printTable(ad.getPrintTable());
                break;
            }

            case 0x09: {
                MP::printTitle(2, "VideoData Tag");
                FLV::VideoDataTag vd(in);
                vd.parse();
                MP::printTable(vd.getPrintTable());
                break;
            }

            case 0x12: {
                MP::printTitle(2, "ScriptData Tag");
                FLV::ScriptDataTag sd(in);
                std::cout<<"```json"<<std::endl;
                sd.parse();
                std::cout<<"```"<<std::endl;
                break;
            }
        }
    }



    in.close();

    return 0;
}

