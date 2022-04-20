#version 330 core
out vec4 FragColor;

in vec2 TexPos;

uniform sampler2D Texture;
 
void main()
{
	FragColor = texture(Texture, TexPos);
	// FragColor = vec4(1, -TexPos.x, -TexPos.y, 1.0f);
}