//
// Created by Brandon on 7/15/2015.
//

#include "Parser.h"
#include <string>
#include <vector>
#include <stdlib.h>

std::vector<std::string> Parser::stringSplit(std::string delimiter,std::string& toSplit)
{
    std::vector<std::string> output;
    std::string::size_type mindex,newdex;  //Will hold the (minimum index-1) to start searching from
    for(mindex = 0;newdex != std::string::npos;mindex = newdex + delimiter.length())
    {
        newdex = toSplit.find(delimiter, mindex);
        output.push_back(toSplit.substr(mindex, newdex - mindex));
    }
    return output;
}

std::string Parser::stringReplaceAll(std::string delimiter,std::string replacement, std::string& toReplace)
{
    std::string output = toReplace; //nab a local copy
    std::string::size_type index = 0;
    //While it can still find the delimiter, do replacement operations
    while((index = output.find(delimiter,index)) != std::string::npos)
    {
        output.erase(index,delimiter.length());
        output.insert(index,replacement);
        index += replacement.length();            //This moves the index up so there's no recursion if the delimiter is found in the replacement
    }
    return output;
}

std::string Parser::stringToLower(std::string& input)
{
    std::string output="";
    std::string::size_type length= input.length();
    for(std::string::size_type i = 0; i<length; i++)
    {
        output += (char)tolower(input[i]);
    }

    return output;
}

int Parser::parseInt(std::string& numstring)
{
    char* endPtr;
    if((numstring.find("0x")==std::string::npos))
    {
        return strtol(numstring.c_str(),&endPtr,10);
    }
    return strtol(numstring.c_str(),&endPtr,16);
}
