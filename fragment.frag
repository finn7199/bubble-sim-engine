#version 330 core
out vec4 FragColor;


in vec2 TexCoord;

uniform sampler2D bubbleTexture; // Sampler for the bubble texture

void main()
{
    FragColor = texture(bubbleTexture, TexCoord);
    
    // Optional: Discard pixels with low alpha to avoid rendering fully transparent parts
    // if(FragColor.a < 0.1)
    //     discard;
}
