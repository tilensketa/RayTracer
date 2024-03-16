#pragma once

#include "Data.h"

class UBO {
public:
  UBO() = default;

  void init(const Data &data);

  void update(const Data &newData);
  void bind();
  void unbind();
  void clean();

private:
  unsigned int mID;
  int mBindingIndex = 0;
};
