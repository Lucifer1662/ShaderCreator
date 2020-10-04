#version 330 core
out vec4 FragColor;

in vec2 uv;
uniform sampler2D blockTexture;



void main()
{
    FragColor = texture(blockTexture, uv);// vec4(uv, 0, 1);//vertexColor+vec4(0.1f);
} 