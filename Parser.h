//
// Created by Brandon on 7/15/2015.
//

#ifndef EM_SCHEDULER_PARSER_H
#define EM_SCHEDULER_PARSER_H

#include <string>
#include <vector>
#include <stdlib.h>

class Parser
{
public:
    static std::vector<std::string> stringSplit(std::string delimiter,std::string& toSplit);
    static std::string stringReplaceAll(std::string delimiter,std::string replacement, std::string& toReplace);
    static std::string stringToLower(std::string& input);
    static int parseInt(std::string& numstring);
};


#endif //EM_SCHEDULER_PARSER_H
