#ifndef STREAM_EXCEPTION_HPP
#define STREAM_EXCEPTION_HPP

#include <exception>

class StreamException : public std::runtime_error {
  public:
    StreamException(std::string message) 
      : std::runtime_error(message.c_str()) {}
};

#endif
