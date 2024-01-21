#ifndef DATA_H
#define DATA_H

#include <string>
#include <vector>

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

struct LargeData
{
  bool              large;
  std::vector<Data> children;
};

#endif
