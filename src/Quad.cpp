#include <glad/glad.h>

#include "Quad.h"

Quad::Quad() {
  GLfloat vertices[] = {
      -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
  };
  GLuint indices[] = {0, 1, 2, 2, 1, 3};

  // Generate and bind Vertex Array Object
  glGenVertexArrays(1, &mVAO);
  glBindVertexArray(mVAO);

  // Generate and bind Vertex Buffer Object
  glGenBuffers(1, &mVBO);
  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Generate and bind Element Buffer Object
  glGenBuffers(1, &mEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Set up vertex attributes
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
                        (GLvoid *)0);
  glEnableVertexAttribArray(0);

  // Unbind VAO
  glBindVertexArray(0);
}

void Quad::draw() {
  glBindVertexArray(mVAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Quad::clean() {
  glDeleteVertexArrays(1, &mVAO);
  glDeleteBuffers(1, &mVBO);
  glDeleteBuffers(1, &mEBO);
}
