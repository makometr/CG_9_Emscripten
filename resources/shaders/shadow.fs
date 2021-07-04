#version 330 core

out vec4 fragColor;

void main()
{             
    gl_FragDepth = gl_FragCoord.z;
    fragColor = vec4(vec3(gl_FragCoord.z), 1.0f);
}  