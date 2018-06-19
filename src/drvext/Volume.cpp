#include "stdafx.h"
#include <assert.h>
#include <stdio.h>
#include <algorithm>
#include "Volume.h"


Volume::Volume(const char * name, size_t blockSize)
  : name(name ? name : "")
  , blockSize(blockSize)
{
  assert(!this->name.empty());
  assert(blockSize > 0);

  if (!CreateDirectoryA(name, nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
  {
    printf("Failed to create directory at '%s'\n", this->name.c_str());
  }
}


Volume::~Volume()
{
}


ssize_t Volume::Read(void * buf, ssize_t size, size_t offset)
{
  size_t block = offset / this->blockSize;
  size_t blockPos = offset % this->blockSize;
  ssize_t remaining = size;
  size_t cur = offset;
  uint8_t * ptr = static_cast<uint8_t *>(buf);

  ssize_t result = 0;
  
  char filename[_MAX_PATH];

  while (remaining > 0)
  {
    size_t target = std::min<size_t>(this->blockSize - blockPos, remaining); 
    
    sprintf(filename, "%s\\%llu.bin", this->name.c_str(), static_cast<long long unsigned>(block));

    FILE * file = fopen(filename, "rb");
    if (file)
    {
      if (blockPos > 0)
      {
        fseek(file, blockPos, SEEK_CUR);
      }

      size_t bytes = fread(ptr, 1, target, file);

      fclose(file);

      if (bytes < target)
      {
        memset(ptr, 0, target - bytes);
      }
    }
    else
    {
      memset(ptr, 0, target);
    }

    result += target;
    ptr += target;
    remaining -= target;

    blockPos = 0;
  }

  return result;
}


ssize_t Volume::Write(void * buf, ssize_t size, size_t offset)
{
  size_t block = offset / this->blockSize;
  size_t blockPos = offset % this->blockSize;
  ssize_t remaining = size;
  size_t cur = offset;
  uint8_t * ptr = static_cast<uint8_t *>(buf);

  ssize_t result = 0;

  char filename[_MAX_PATH];

  while (remaining > 0)
  {
    size_t target = std::min<size_t>(this->blockSize - blockPos, remaining);

    sprintf(filename, "%s\\%llu.bin", this->name.c_str(), static_cast<long long unsigned>(block));

    FILE * file = fopen(filename, "rb+");
    if (!file)
    {
      file = fopen(filename, "wb+");
    }

    if (file)
    {
      if (blockPos > 0)
      {
        if (fseek(file, blockPos, SEEK_CUR) != 0)
        {
          fseek(file, 0, SEEK_END);
          auto filesize = ftell(file);
          if (filesize > blockPos)
          {
            printf("ERROR: failed to seek file '%s' with size %llu\n", filename, blockPos);
            fclose(file);
            break;
          }
          else if (blockPos > filesize)
          {
            uint8_t * tmp = new uint8_t[blockPos - filesize];
            memset(tmp, 0, blockPos - filesize);
            size_t bytes = fwrite(tmp, 1, blockPos - filesize, file);
            delete[] tmp;
            if (bytes < blockPos - filesize)
            {
              printf("ERROR: failed to fill in empty part for file '%s'\n", filename);
              fclose(file);
              break;
            }
          }
        }
      }

      size_t bytes = fwrite(ptr, 1, target, file);

      fclose(file);

      result += bytes;
      ptr += bytes;
      remaining -= bytes;

      if (bytes < target)
      {
        printf("ERROR: failed to write file '%s' with expected size.\n", filename);
        break;
      }
    }
    else
    {
      printf("ERROR: failed to open file '%s'\n", filename);
    }

    blockPos = 0;
  }

  return result;
}