#version 120

uniform mat4 modelView; ///Matriz ModelView
uniform mat4 projection; ///Matriz de projeção
uniform mat4 normalMatrix; ///Normal para o espaço do olho
uniform vec4 lightPositionEye; ///Posição da fonte de luz

attribute vec2 coordinateTexture;///Cordenada de textura

attribute vec3 position; ///Posição do vértice

attribute vec3 vertexNormal; ///Vetor Normal
attribute vec3 vertexTanget; ///Vetor Tangente

varying vec2 mapCoord; ///Cordenada de textura
varying vec3 dirCameraTangent; ///Vetor direção da luz - Espaço Tangente
varying vec3 dirLuzTangent; ///Vetor direção da câmera - Espaço Tangente
varying vec3 halfVector; /// HalfVector - Espaço Tangente

void main(){
    vec4 posVertexEye = modelView * vec4(position, 1.0);
	mapCoord = coordinateTexture;

    vec3 normalVertexEye = normalize((normalMatrix * vec4(vertexNormal,0.0)).xyz);
    vec3 tangentVertexEye = normalize((normalMatrix * vec4(vertexTanget,0.0)).xyz);

	mat3 TBN_Matrix = transpose(mat3(tangentVertexEye,cross(normalVertexEye,tangentVertexEye),normalVertexEye));
    vec3 dirLuz = normalize(vec3(lightPositionEye - posVertexEye));

	dirLuzTangent= normalize(TBN_Matrix * dirLuz);
	dirCameraTangent = normalize(TBN_Matrix*(-vec3(posVertexEye)));
    halfVector = TBN_Matrix * normalize(normalize(vec3(posVertexEye)) + dirLuz);

    gl_Position = projection * posVertexEye;
}
