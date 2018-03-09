//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license


#include<stdint.h>


#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<iterator>
#include<iomanip>


#include"Header.h"
#include"ScriptDataTag.h"
#include"helper.h"
#include"AudioDataTag.h"
#include"VideoDataTag.h"
#include"MarkdownPrinter.h"


using uchar = unsigned char;


void printAVCConfigurationRecord(const FLV::AVCConfigurationRecord &avc_cfg_rd)
{
    std::cerr<<"configurationVersion = "<<static_cast<int>(avc_cfg_rd.configurationVersion())<<std::endl;
    std::cerr<<"AVCProfileIndication = "<<static_cast<int>(avc_cfg_rd.AVCProfileIndication())<<std::endl;
    std::cerr<<"profile_compatibility = "<<static_cast<int>(avc_cfg_rd.profile_compatibility())<<std::endl;
    std::cerr<<"AVCLevelIndication = "<<static_cast<int>(avc_cfg_rd.AVCLevelIndication())<<std::endl;

    auto& sps_data = avc_cfg_rd.sequenceParameterSetNALUnit();
    auto& pps_data = avc_cfg_rd.pictureParameterSetNALUnit();

    std::cerr<<std::hex<<std::showbase;
    for(auto &sps : sps_data)
    {
        std::copy(sps.begin(), sps.end(), std::ostream_iterator<int>(std::cerr, ", "));
        std::cerr<<std::endl;
    }

    for(auto &pps : pps_data)
    {
        std::copy(pps.begin(), pps.end(), std::ostream_iterator<int>(std::cerr, ", "));
        std::cerr<<std::endl;
    }
}


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


    size_t last_dts = 0;
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
                const std::vector<uchar> &rd = vd.videoRealData();
                if(vd.avcPacketType() == 0)
                    printAVCConfigurationRecord(FLV::AVCConfigurationRecord(rd.begin(), rd.end()));

                MP::printTable(vd.getPrintTable());
                //std::copy(rd.begin(), rd.begin()+4, std::ostream_iterator<uint32_t>(std::cerr, ", "));
                //std::cerr<<",  size = "<<rd.size()-4<<std::endl;
                //std::cerr<<"dts = "<<90*vd.tagTs()<<"\t diff "<<90*(vd.tagTs()-last_dts)<<"\t ";
                //std::cerr<<"pts = "<<90*(vd.tagTs() + vd.compositionTime())<<std::endl;
                last_dts = vd.tagTs();
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

