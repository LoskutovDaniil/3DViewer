attribute highp vec4 qt_Vertex;
uniform highp mat4 qt_ModelViewProjectionMatrix;

void main(void)
{
    gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
}
