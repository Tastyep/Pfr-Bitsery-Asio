#ifndef DATA_H
#define DATA_H

#include <string>

struct Header
{
  int size;
};

struct Data
{
  Header      header;
  int         number;
  std::string str;
};

#endif
