#ifndef DATA_H
#define DATA_H

#include <string>

struct Header
{
  int size;
};

struct Data
{
  int         number;
  std::string firstStr;
  std::string secondStr;
};

#endif