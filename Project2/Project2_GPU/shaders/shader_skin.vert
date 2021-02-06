#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

#define MAX_JOINTS 50

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in int I;
layout (location = 3) in ivec4 jointIdx;
layout (location = 4) in vec4 jointWeight;

// Uniform variables
uniform mat4 viewProj;
uniform mat4 model;
//uniform int jointNum;
uniform mat4 posMat[MAX_JOINTS];
uniform mat4 normMat[MAX_JOINTS];

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. 
out vec3 fragNormal;


void main()
{
    mat4 transPosMat = mat4(0.0);
    mat4 transNormMat = mat4(0.0);
   
    // switch(3)
    // {
    //     case 3:
    //         transPosMat += posMat[jointIdx.w] * jointWeight.w;
    //         transNormMat += normMat[jointIdx.w] * jointWeight.w;
    //     case 2:
    //         transPosMat += posMat[jointIdx.z] * jointWeight.z;
    //         transNormMat += normMat[jointIdx.z] * jointWeight.z;
    //     case 1:
    //         transPosMat += posMat[jointIdx.y] * jointWeight.y;
    //         transNormMat += normMat[jointIdx.y] * jointWeight.y;
    //     case 0:
    //         transPosMat += posMat[jointIdx.x] * jointWeight.x;
    //         transNormMat += normMat[jointIdx.x] * jointWeight.x;
    // }
    transPosMat += posMat[jointIdx.w] * jointWeight.w;
    transNormMat += normMat[jointIdx.w] * jointWeight.w;
    transPosMat += posMat[jointIdx.z] * jointWeight.z;
    transNormMat += normMat[jointIdx.z] * jointWeight.z;
    transPosMat += posMat[jointIdx.y] * jointWeight.y;
    transNormMat += normMat[jointIdx.y] * jointWeight.y;
    transPosMat += posMat[jointIdx.x] * jointWeight.x;
    transNormMat += normMat[jointIdx.x] * jointWeight.x;

    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = viewProj * model * transPosMat * vec4(position, 1.0); //
    //gl_Position = viewProj * model * vec4(position, 1.0) * float(jointIdx.x); //

    // for shading
	fragNormal = vec3(model * transNormMat * vec4(normal, 0)); //

    // vec3 transPos = vec3(0.0);
    // vec3 transNorm = vec3(0.0);
    // switch(I)
    // {
    //     case 3:
    //         transPos += vec3(jointWeight.w * posMat[jointIdx.w] * vec4(position, 1.0));
    //         transNorm += vec3(jointWeight.w * normMat[jointIdx.w] * vec4(normal, 0));
    //     case 2:
    //         transPos += vec3(jointWeight.z * posMat[jointIdx.z] * vec4(position, 1.0));
    //         transNorm += vec3(jointWeight.z * normMat[jointIdx.z] * vec4(normal, 0));
    //     case 1:
    //         transPos += vec3(jointWeight.y * posMat[jointIdx.y] * vec4(position, 1.0));
    //         transNorm += vec3(jointWeight.y * normMat[jointIdx.y] * vec4(normal, 0));
    //     case 0:
    //         transPos += vec3(jointWeight.x * posMat[jointIdx.x] * vec4(position, 1.0));
    //         transNorm += vec3(jointWeight.x * normMat[jointIdx.x] * vec4(normal, 0));
    // }
    
    // // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    // gl_Position = viewProj * model * vec4(transPos, 1.0); //

    // // for shading
	// fragNormal = vec3(model * vec4(transNorm, 0)); //
}