#include "StdAfx.h"
#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#include "sample_rate_converter.h"
#include "exception.h"
#include "code_converter.h"

#ifdef _DEBUG
#pragma comment(lib,"src_d.lib")
#else
#pragma comment(lib,"src.lib")
#endif
namespace sf {


sample_rate_converter::sample_rate_converter(double input_rate,double output_rate,int channel,int converter_type) : state_(0)
{
  int error = 0;
  state_ = src_new(converter_type,channel,&error);
  if(error)
  {
    throw sf::exception((wchar_t*)(sf::code_converter<char,wchar_t>(src_strerror(error))));
  }
  ZeroMemory(&data_,sizeof(SRC_DATA));
  data_.src_ratio = output_rate / input_rate;
}

void sample_rate_converter::apply(float * in,float * out,size_t size)
{
  data_.data_in = in;
  data_.data_out = out;
  data_.input_frames = data_.output_frames = size;
  src_process(state_,&data_);
};

sample_rate_converter::~sample_rate_converter()
{
  if(state_)
  {
    src_delete(state_);
  }
}
}
