//attribute vec4 position;
//attribute vec2 texCoord;
//
//uniform mat4 modelMatrix;
//uniform mat4 viewMatrix;
//uniform mat4 projectionMatrix;
//
//varying vec2 vTex;
//varying vec2 vPos;   // world-space XY
//
//void main() {
//    vec4 world = modelMatrix * position;
//    vPos = world.xy;       // pass world coordinates
//    vTex = texCoord;
//    gl_Position = projectionMatrix * viewMatrix * world;
//}
