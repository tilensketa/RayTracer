#pragma once

class Quad {
public:
  Quad();

  void draw();
  void clean();

private:
  unsigned int mVBO;
  unsigned int mVAO;
  unsigned int mEBO;
};
