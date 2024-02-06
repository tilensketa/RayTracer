#version 430

layout(location = 0) in vec2 inPosition;

out vec2 TexCoord;

void main() {
  gl_Position = vec4(inPosition, 0.0f, 1.0f);
  TexCoord = inPosition;
}
