//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#ifndef MARKDOWNPRINTER_H
#define MARKDOWNPRINTER_H


#include<vector>
#include<string>


using StringVector = std::vector<std::string>;

namespace MP
{


void printTable(const std::vector<StringVector> &vec);

void printTitle(size_t level, const std::string &context);


}


#endif // MARKDOWNPRINTER_H




