#version 330 core
in vec2 coords;
uniform sampler2D frame;
out vec3 color;
void main(){
  color = texture( frame, coords ).rgb;
}