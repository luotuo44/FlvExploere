//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#include"MarkdownPrinter.h"

#include<assert.h>

#include<iostream>
#include<algorithm>
#include<iterator>
#include<sstream>
#include<stdexcept>



namespace MP
{


static std::string join(const std::vector<std::string> &vec, const std::string &delim)
{
    std::string str;
    do
    {
        if(vec.empty())
            break;


        std::stringstream ss;
        std::copy(vec.begin(), std::prev(vec.end()), std::ostream_iterator<std::string>(ss, delim.c_str()));

        ss<<vec.back();
        str = ss.str();
    }while(0);


    return str;
}


void printTable(const std::vector<StringVector> &vec)
{
    if(vec.empty())
        return ;

    auto it = std::adjacent_find(vec.begin(), vec.end(), [](const StringVector &lh, const StringVector &rh){
       return lh.size() != rh.size();
    });

    if(it != vec.end())
        throw std::runtime_error("vec should be a table that each row should has the same column");



    const StringVector &header = vec[0];
    std::cout<<"|"<<join(header, "|")<<"|"<<std::endl;

    std::cout<<join(StringVector(header.size()+1, "|"), ":--:")<<std::endl;

    std::for_each(std::next(vec.begin()), vec.end(), [](const StringVector &vec){
        std::cout<<"|"<<join(vec, "|")<<"|"<<std::endl;
    });
}



void printTitle(size_t level, const std::string &context)
{
    if(level == 0)
        ++level;

    std::cout<<std::string(level, '#')<<" "<<context<<std::endl;

}


}

