#ifdef GL_ES
precision lowp float; // mediump
#endif

varying vec2 v_texCoord;

uniform sampler2D normalBuffer;

const float ABS_OFFSET = 0.025;

void main()
{
    vec4 distor = texture2D(normalBuffer, v_texCoord);
    vec3 normal = normalize(2.0 * distor.xyz - 1.0);
    
    vec2 uv = v_texCoord + normal.xy * (ABS_OFFSET * distor.a);
    vec4 color = texture2D(CC_Texture0, uv);
    
    gl_FragColor = color;
}
