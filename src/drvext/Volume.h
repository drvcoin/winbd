#pragma once

#include <string>
#include <stdint.h>


class Volume
{
public:
  explicit Volume(const char * name, size_t blockSize);
  ~Volume();

  ssize_t Read(void * buf, ssize_t size, size_t offset);

  ssize_t Write(void * buf, ssize_t size, size_t offset);

private:

  std::string name;

  size_t blockSize;
};

