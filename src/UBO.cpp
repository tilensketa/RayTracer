#include "UBO.h"

#include <glad/glad.h>

void UBO::init(Data data) {
  glGenBuffers(1, &ID);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ID);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Data), &data, GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ID);
}

void UBO::update(Data newData) {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ID);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Data), &newData,
               GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ID);
}

void UBO::bind() { glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ID); }

void UBO::unbind() { glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); }

void UBO::clean() { glDeleteBuffers(1, &ID); }
