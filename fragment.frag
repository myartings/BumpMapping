#version 120

uniform sampler2D texture; ///Textura
uniform sampler2D normalMap; ///Normal Map

varying vec2 mapCoord; ///cordenada de textura

varying vec3 dirLuzTangent; /// Direção da luz - Espaço Tangente
varying vec3 dirCameraTangent; /// Direção da Câmera - Espaço Tangente
varying vec3 halfVector; /// HalfVector - Espaço Tangente

uniform vec4 ks; // Componente Especular do Material
const float Ns = 128.0; // expoente especular

vec3 expand (vec3 v){
    return (v - 0.5) * 2;
}

void main() {
  vec3 normal = normalize(expand(texture2D(normalMap,mapCoord).xyz));
  vec4 colorMap = texture2D(texture,mapCoord);

  float NdotL = max(dot(dirLuzTangent, normal), 0.0);

  vec4 Ia = vec4(0.2,0.2,0.2,1.0) * colorMap; ///Componente ambiente
  vec4 Id = vec4(0.8,0.8,0.8,1.0) * colorMap * NdotL ; ///Componente difusa
  vec4 Is = vec4(0.0,0.0,0.0,1.0); ///Componente especular

  if(NdotL>0){
    Is = (0.8,0.8,0.8,0.8) * ks * pow(max(dot(reflect(-dirLuzTangent,normal),normal),0), Ns);
  }
  gl_FragColor = Ia + Id + Is;
  //gl_FragColor = vec4(dirCameraTangent,1.0);
}
