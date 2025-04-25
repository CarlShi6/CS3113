////#ifdef GL_ES
////precision mediump float;
////#endif
////
////// your texture
////uniform sampler2D diffuse;
////
////// ——— globals ———
////uniform float ambient;   // base light (0–1)
////
////// spot‐light struct
////struct Spot {
////    vec2  pos;         // world xy
////    vec2  dir;         // normalized
////    float cosCutoff;   // cos(half-angle)
////    float a;           // linear fall-off
////    float b;           // quadratic fall-off
////};
////uniform Spot spot1;
////uniform Spot spot2;
////
////// pass in from vertex
////varying vec2 vTex;
////varying vec2 vPos;
////
////float attenuate(float d, float a, float b) {
////    return 1.0 / (1.0 + a*d + b*d*d);
////}
////
////float spotFactor(Spot s) {
////    vec2 L = vPos - s.pos;
////    float d = length(L);
////    vec2 Ln = L / max(d,0.0001);
////    float cosT = dot(Ln, s.dir);
////    if (cosT < s.cosCutoff) return 0.0;
////    return attenuate(d, s.a, s.b) * cosT;
////}
////
////void main() {
////    // start with global ambient
////    float light = ambient;
////    // add both cones
////    light += spotFactor(spot1);
////    light += spotFactor(spot2);
////    // clamp to max 1
////    light = clamp(light, 0.0, 1.0);
////
////    vec4 base = texture2D(diffuse, vTex);
////    gl_FragColor = vec4(base.rgb * light, base.a);
////}
//
//#ifdef GL_ES
//precision mediump float;
//#endif
//
//uniform float ambient;
//
//void main() {
//    // show a grey ramp based on ambient: if ambient=0.2 you get 20% grey
//    gl_FragColor = vec4(vec3(ambient), 1.0);
//}
