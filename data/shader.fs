#version 330 core
out vec4 FragColor;

in vec2 TexPos;

uniform sampler2D Texture;
 
void main()
{
	FragColor = texture(Texture, TexPos);
	// vec4(1.0f, 1.0f, 1.0f, 1.0f);
}