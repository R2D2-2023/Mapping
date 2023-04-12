#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

//manual resolution set here
const float renderWidth = 800;
const float renderHeight = 800;

//kernel size for gausian filter
const int kernelSize = 12;

void main()
{
    vec3 texelColor = vec3(0,0,0);

    float total = 0;

    //apply kernel to pixel

    for (int x = -kernelSize; x <= kernelSize; x++)
    {
        for(int y = -kernelSize; y <= kernelSize; y++)
        {
            float strength = 1.0f / (length(vec2(x,y))+1);
            texelColor += texture(texture0, fragTexCoord + vec2(x/renderWidth,y/renderHeight), 0.0).rgb * strength;

            total += strength;
        }
    }


    //get average
    texelColor.rgb /= total;

    //get higher value (average value is increadible low otherwise)
    texelColor.rgb *= 4;

    float alpha = texelColor.r;

    finalColor = vec4(texelColor, alpha);
}