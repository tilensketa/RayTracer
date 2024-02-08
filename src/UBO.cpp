#include <glad/glad.h>

#include "UBO.h"

void UBO::init(const Data &data) {
  glGenBuffers(1, &mID);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, mID);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Data), &data, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void UBO::update(const Data &newData) {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, mID);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Data), &newData);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void UBO::bind() { glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mID); }

void UBO::unbind() { glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); }

void UBO::clean() {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  glDeleteBuffers(1, &mID);
}
