#pragma once

#include "Utils.h"

class UBO {
public:
  UBO() = default;
  void init(Data data);

  void update(Data newData);
  void bind();
  void unbind();
  void clean();

private:
  unsigned int ID;
};
