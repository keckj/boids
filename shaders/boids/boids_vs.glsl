#version 330

/*uniform mat4 modelMatrix = mat4(1,0,0,0,
                                0,1,0,0,
                                0,0,1,0,
                                0,0,0,1);*/
layout(std140) uniform projectionView {
	mat4 projectionMatrix;
	mat4 viewMatrix;
	vec3 cameraPos;
	vec3 cameraDir;
	vec3 cameraUp;
	vec3 cameraRight;
};
//uniform ivec2 screenSize;
uniform float boidSize = 10.0;

in vec3 vertexPosition;
//in int boidType;

//out int fBoidType;

/*const float minPointScale = 0.1;
const float maxPointScale = 1.0;
const float maxDistance   = 1.0;*/

void main(void)
{
    //fBoidType = boidType;

    vec3 centeredPos = vertexPosition - vec3 (.5,.5,.5); // [0,1] -> [-.5,.5]
    float cameraDist = distance(centeredPos, cameraPos);
    //float pointScale = 1.0 - (cameraDist / maxDistance);
    float pointScale = 1.0/cameraDist;
    //pointScale = max(pointScale, minPointScale);
    //pointScale = min(pointScale, maxPointScale);
    //float size = screenSize[0]*pointScale;
    float size = boidSize * pointScale;
    gl_PointSize = max(size, 2.0);

    gl_Position = projectionMatrix * viewMatrix * vec4(centeredPos, 1.0);
}