#include "median_filter.h"
 
median_filter::median_filter(int size)
    :
        size(size)
{
    for( float i = 0; i < this->size; i++)
        this->window.push_back(i);
}
 
float median_filter::process(float value)
{
  this->window.pop_front();
  this->window.push_back(value); 
  std::deque<float> temp(this->window); 
  std::sort(temp.begin(), temp.end()); 
  return temp[this->size/2];
}