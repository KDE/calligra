/*
 * Vertex shader for handling scaling
 */
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

attribute vec3 vertex;
attribute vec2 uv0;

varying vec2 out_uv0;

void main()
{
    gl_Position = vec4(vertex, 1.0) * ((modelMatrix * viewMatrix) * projectionMatrix);
    out_uv0 = uv0;
}
