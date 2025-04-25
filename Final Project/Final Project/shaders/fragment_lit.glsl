// fragment_lit.glsl
uniform sampler2D diffuse;
uniform vec2 lightPosition;
uniform float ambient;

varying vec2 texCoordVar;
varying vec2 varPosition;

float attenuate(float dist, float a, float b)
{
     return 1.0 / (1.0 + (a * dist) + (b * dist  * dist));
}

void main()
{
    // The brightness is directly based on the distance between the light source's
    // location and the pixel's location
    float dist       = distance(lightPosition, varPosition);
//    float brightness = attenuate(distance(lightPosition, varPosition), 1.0, 0.0);
    float brightness = max(ambient, attenuate(dist, 0.3, 0.0));
    
//    vec4 color = texture2D(diffuse, texCoordVar);
//    gl_FragColor = vec4(color.rgb * brightness, color.a);
    
    vec4 texColour   = texture2D(diffuse, texCoordVar);
    gl_FragColor     = vec4(texColour.rgb * brightness, texColour.a);
}
