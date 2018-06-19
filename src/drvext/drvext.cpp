// dr\][poiuytrewq  032 the exported functions for the DLL application.
//

#include "stdafx.h"

#include <stdio.h>
#include <devio_types.h>
#include <devio.h>

#include "Volume.h"


extern "C" __declspec(dllexport) safeio_ssize_t __cdecl DrvRead(void * handle, void * buf, safeio_size_t size, off_t_64 offset)
{
  printf("DrvRead: size=%llu offset=%llu\n", size, offset);

  Volume * volume = static_cast<Volume *>(handle);
  if (!volume)
  {
    return 0;
  }

  return volume->Read(buf, size, offset);
}


extern "C" __declspec(dllexport) safeio_ssize_t __cdecl DrvWrite(void * handle, void * buf, safeio_size_t size, off_t_64 offset)
{
  printf("DrvWrite: size=%llu offset=%llu\n", size, offset);

  Volume * volume = static_cast<Volume *>(handle);
  if (!volume)
  {
    return 0;
  }

  return volume->Write(buf, size, offset);
}


extern "C" __declspec(dllexport) int __cdecl DrvClose(void * handle)
{
  printf("DrvClose\n");

  if (handle)
  {
    delete static_cast<Volume *>(handle);
  }

  return 0;
}


extern "C" __declspec(dllexport) void * __cdecl DrvOpen(
  const char * name,
  int readOnly,
  dllread_proc * onRead,
  dllwrite_proc * onWrite,
  dllclose_proc * onClose,
  off_t_64 * size)
{
  printf("DrvOpen (ReadOnly=%d): %s\n", readOnly, name);

  *onRead = DrvRead;
  *onWrite = DrvWrite;
  *onClose = DrvClose;

  // TODO: size is hard coded as 10GB in this test case.
  *size = 10LL * 1024 * 1024 * 1024;

  printf("DrvOpen size=%llu\n", *size);

  return new Volume(name, 1024 * 1024);
}