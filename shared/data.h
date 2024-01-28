#ifndef DATA_H
#define DATA_H

#include <boost/describe/enum.hpp>
#include <string>
#include <vector>

struct Header
{
  int size;
};

enum class Color
{
  red,
  green,
  blue
};
BOOST_DESCRIBE_ENUM(Color, red, green, blue)

struct Data
{
  int         number;
  std::string firstStr;
  std::string secondStr;
  short       shortNumber{0};
  Color       color{Color::red};
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
