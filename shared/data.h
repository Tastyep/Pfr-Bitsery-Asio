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

struct SubTest
{
  std::string       value;
  std::vector<Data> datas = {{.number = 1}};
};

struct Test
{
  SubTest subTest;
};

struct LargeData
{
  bool              large;
  std::vector<Data> children;
  Test              test;
};

#endif
