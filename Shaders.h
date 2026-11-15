#pragma once

#if defined(GRAPHICS_API_OPENGL_ES2) || defined(GRAPHICS_API_OPENGL_ES3)

inline const char* CRT_FRAGMENT_SHADER = R"(#version 100
precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform float time;
uniform float scanlineIntensity;
uniform float curvature;
uniform float bloom;

void main()
{
    vec2 uv = fragTexCoord;
    vec2 dc = uv - 0.5;
    float dist = dot(dc, dc);
    uv = uv + dc * (dist * curvature);

    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec4 col = texture2D(texture0, uv);

    float scanline = sin(uv.y * 224.0 * 3.14159265 * 2.0);
    float scanMod = 1.0 - (scanlineIntensity * (0.5 + 0.5 * scanline));
    col.rgb *= scanMod;

    float subpixel = mod(floor(uv.x * 640.0 * 3.0), 3.0);
    vec3 mask = vec3(0.9, 0.9, 0.9);
    if (subpixel < 1.0) mask = vec3(1.15, 0.88, 0.88);
    else if (subpixel < 2.0) mask = vec3(0.88, 1.15, 0.88);
    else mask = vec3(0.88, 0.88, 1.15);
    col.rgb = mix(col.rgb, col.rgb * mask, bloom);

    float vx = uv.x * (1.0 - uv.x);
    float vy = uv.y * (1.0 - uv.y);
    float vig = clamp(16.0 * vx * vy, 0.0, 1.0);
    col.rgb *= (0.78 + 0.22 * vig);

    gl_FragColor = col * fragColor;
}
)";

inline const char* FLAG_FRAGMENT_SHADER = R"(#version 100
precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;
uniform float time;

void main()
{
    vec2 uv = fragTexCoord;
    float ripple = sin(uv.x * 12.0 - time * 6.0) * 0.04 * (uv.x + 0.2);
    float rippleY = cos(uv.x * 8.0 - time * 5.0) * 0.02 * (uv.x + 0.2);
    uv.y += ripple;
    uv.x += rippleY;

    vec4 col = texture2D(texture0, uv);
    float shadow = 1.0 + (cos(uv.x * 12.0 - time * 6.0) * 0.15 * uv.x);
    col.rgb *= shadow;

    gl_FragColor = col * fragColor;
}
)";

#else

inline const char* CRT_FRAGMENT_SHADER = R"(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform float time;
uniform float scanlineIntensity;
uniform float curvature;
uniform float bloom;

void main()
{

    vec2 uv = fragTexCoord;
    vec2 dc = uv - 0.5;
    float dist = dot(dc, dc);
    uv = uv + dc * (dist * curvature);

    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec4 col = texture(texture0, uv);

    float scanline = sin(uv.y * 560.0 * 3.14159265 * 2.0);
    float scanMod = 1.0 - (scanlineIntensity * (0.5 + 0.5 * scanline));
    col.rgb *= scanMod;

    float subpixel = mod(floor(uv.x * 1920.0 * 3.0), 3.0);
    vec3 mask = vec3(0.9, 0.9, 0.9);
    if (subpixel < 1.0) mask = vec3(1.15, 0.88, 0.88);
    else if (subpixel < 2.0) mask = vec3(0.88, 1.15, 0.88);
    else mask = vec3(0.88, 0.88, 1.15);
    col.rgb *= mix(vec3(1.0), mask, bloom);

    float vx = uv.x * (1.0 - uv.x);
    float vy = uv.y * (1.0 - uv.y);
    float vig = clamp(16.0 * vx * vy, 0.0, 1.0);
    col.rgb *= (0.78 + 0.22 * vig);

    finalColor = col * fragColor;
}
)";

inline const char* FLAG_FRAGMENT_SHADER = R"(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float time;

void main()
{
    vec2 uv = fragTexCoord;

    float ripple = sin(uv.x * 12.0 - time * 6.0) * 0.04 * (uv.x + 0.2);
    float rippleY = cos(uv.x * 8.0 - time * 5.0) * 0.02 * (uv.x + 0.2);
    uv.y += ripple;
    uv.x += rippleY;

    vec4 col = texture(texture0, uv);

    float shadow = 1.0 + (cos(uv.x * 12.0 - time * 6.0) * 0.15 * uv.x);
    col.rgb *= shadow;

    finalColor = col * fragColor;
}
)";

#endif
