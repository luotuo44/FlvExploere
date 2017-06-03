//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license


#include"ScriptDataTag.h"


#include<assert.h>
#include<stdlib.h>
#include<time.h>


#include<string>
#include<vector>
#include<fstream>
#include<iostream>
#include<algorithm>
#include<iomanip>
#include<map>
#include<functional>



#include"helper.h"


using uchar = unsigned char;

namespace FLV
{


ScriptDataTag::ScriptDataTag(std::ifstream &in)
    : m_in(in)
{
    std::cout<<std::fixed<<std::setprecision(0);
    m_op =
    {
        {0, [this](){
                std::cout<<this->readDouble();
            }
        },

        {1, [this](){
                std::cout<<this->readInt(1);
            }
        },

        {2, [this](){
                std::cout<<R"(")"<<this->readString(static_cast<char>(2))<<R"(")";
            }
        },

        {3, [this](){
                this->parseObject();
            }
        },

        {4, [this](){
                //do nothing, reserve
            }
        },

        {5, [this](){
                //do nothing, NULL
            }
        },

        {6, [this](){
                //donothing, Undefined
            }
        },

        {7, [this](){
                //do nothing, reserve
            }
        },

        {8, [this](){
                this->parseEcmaArray();
            }
        },

        {9, [this](){
                std::cout<<"++++%$*@#@%*$#*@$%  get type 9*"<<std::endl;
                //this->parseObject();
            }
        },

        {10, [this](){
                this->parseStrictArray();
            }
        },

        {11, [this](){
                auto p = this->readDate();
                time_t ts = static_cast<time_t>(p.first/1000);//单位是毫秒
                std::string str(ctime(&ts));//ends with \n
                str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
                std::cout<<R"(")"<<str<<R"(")";
            }
        },

        {12, [this](){
                std::cout<<this->readString(12)<<std::endl;
            }
        },
    };
}



void ScriptDataTag::parse()
{
    parseHeader();
    parseBody();
    parseTailer();
}


void ScriptDataTag::parseHeader()
{
    readTagHeader();
}

void ScriptDataTag::parseBody()
{
    readTagData();
}

void ScriptDataTag::parseTailer()
{
    readPreTagSize();
}




//从第二个字节开始，第一个字节已经被client读取，用于确定是何种tag
void ScriptDataTag::readTagHeader()
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



void ScriptDataTag::readPreTagSize()
{
    size_t prev_tag_size = readInt(4);
    assert(prev_tag_size == m_tag_data_len + 11);

    (void)prev_tag_size;
}



void ScriptDataTag::readTagData()
{
    std::cout<<"[";
    while(m_read_bytes < m_tag_data_len)
    {
        parseDataValue();
        std::cout<<",\n";
    }
    std::cout<<"]\n";
}



std::string ScriptDataTag::smartIndent()const
{
    return std::string(m_smartindent, ' ');//+ std::to_string(m_smartindent);
}

//===========================================================================

size_t ScriptDataTag::readInt(size_t bytes)
{
    if( bytes == 0)
        return 0;

    std::vector<uchar> vec(bytes);
    readBytes(vec.data(), bytes);
    size_t val = Helper::getSizeValue(vec);

    return val;
}



double ScriptDataTag::readDouble()
{
    static_assert(sizeof(double) == 8, "sizeof(double) not equals to 8");

    double d = 0;
    readBytes(&d, sizeof(d));
    if(Helper::isLittleEndian())
        std::reverse(reinterpret_cast<char*>(&d), reinterpret_cast<char*>(&d)+sizeof(d));

    return d;
}



std::string ScriptDataTag::readString(char type)//short string or long string
{
    assert(type == 2 || type == 12);

    //short string的长度用2个字节表示，long string的长度用4个字节表示
    size_t str_len = readInt(Helper::getNBits<3>(type, 0));

    std::string str;
    if(str_len > 0)
    {
        str.assign(str_len, '\0');
        readBytes(&str[0], str_len);
    }

    return str;
}



std::pair<double, int16_t> ScriptDataTag::readDate()
{
    double ts = readDouble();
    int16_t dt_offset = readInt(2);

    return std::make_pair(ts, dt_offset);
}



void ScriptDataTag::readBytes(char *arr, size_t bytes)
{
    m_in.read(arr, bytes);
    m_read_bytes += bytes;
}



//=========================================================================
bool ScriptDataTag::parseObjectProperty()
{
    std::string property_name = readString(0x02);//short string
    //ojbect是由ObjectProperty数组组成，但没有长度信息，只有一个结束符信息。因此需要判断是否为结束符
    //结束符用00 00 09三个字节表示，结合ObjectProperty的前两个字节表示string长度，可以得出：
    //如果前两个字节为00 00，那么就可以确定到达了结束符，最后一个字节留给父项读取
    if( property_name.empty() )
        return false;

    std::cout<<smartIndent()<<R"(")"<<property_name<<R"(" : )";

    parseDataValue();
    //std::cout<<",";

    return true;
}


void ScriptDataTag::parseObject()
{
    m_smartindent += 4;
    std::cout<<"{\n";



    while(parseObjectProperty())
    {
        //do nothing
        std::cout<<",\n";
    }

    size_t end = readInt(1);
    assert(end == 9);//结束符, 前两个字节在parseObjectProperty==>readString中已经被读取

    std::cout<<smartIndent()<<"}";
    m_smartindent -= 4;
}


void ScriptDataTag::parseEcmaArray()
{
    std::cout<<smartIndent()<<R"({"EcmaArray" : {)";
    m_smartindent += 4;


    size_t array_len = readInt(4);
    for(size_t i = 0; i < array_len; i += 1)
    {
        std::cout<<"\n";
        parseObjectProperty();
        std::cout<<",";
    }

    size_t end_token = readInt(3);
    assert(end_token == 9);
    (void)end_token;


    std::cout<<"\n"<<smartIndent()<<"}\n";
    m_smartindent -= 4;
    std::cout<<smartIndent()<<"}";
}


void ScriptDataTag::parseStrictArray()
{
    std::cout<<"[";
    m_smartindent += 4;


    size_t array_len = readInt(4);
    for(size_t i = 0; i < array_len; i += 1)
    {
        std::cout<<"\n"<<smartIndent();
        parseDataValue();
        std::cout<<",";
    }


    std::cout<<"\n"<<smartIndent()<<"]";
    m_smartindent -= 4;
}


void ScriptDataTag::parseDataValue()
{
    char type;
    readBytes(&type, 1);

    if(m_op.find(type) != m_op.end())
    {
        m_op[type]();
    }
    else
    {
        std::cout<<"unexpection type "<<static_cast<int>(type)<<std::endl;
        exit(1);
    }
}


}
