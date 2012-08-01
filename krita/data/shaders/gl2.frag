uniform sampler2D texture0;

varying vec2 out_uv0;

void main() {
    gl_FragColor = texture2D(texture0, out_uv0);
}