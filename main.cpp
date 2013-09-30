#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#include <vector>
#include <iostream>

#include "config.hpp"
#include "shaderFunctions.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "readPNG.hpp"

#define M_PI       3.14159265358979323846

/// Matrizes da GLM
glm::mat4 mProjection,mView,mModel;

/// Identificadores do programa e do vertex shader e fragment shader
GLuint p,v,f;

/// Localização de Atributos de Vértice
GLuint vertexLoc,vertexNormal,vertexST,tangentLoc;

/// Localização de Variáveis Uniformes
GLuint nMatriz,modelView,projection,ks;

std::vector<GLushort> indicesSphere;

static GLuint nmTexturas[2];

/// Identificadores do Vertex Array Objects
GLuint vao[2];

///Configura os valores das variáveis uniformes.
void setUniforms();

void setupLight(){
    GLuint lightPosition = glGetUniformLocation(p, "lightPosition");
    glUniform4fv(lightPosition, 1,glm::value_ptr(mView*glm::vec4(0.0f, 7.0f, 15.0f,1.0)));
}

void setupTexture(const char * normalMap,const char * texture){
    int width,height;
    glGenTextures(2, nmTexturas);
    glActiveTexture(GL_TEXTURE0);
    png_texture_load(normalMap,&width,&height,nmTexturas[0]);
    glUniform1i(glGetUniformLocation(p, "normalMap"), 0);
    glActiveTexture(GL_TEXTURE1);
    png_texture_load(texture,&width,&height,nmTexturas[1]);
    glUniform1i(glGetUniformLocation(p, "texture"), 1);
}

void destroyTexture(){
    glDeleteTextures(2,nmTexturas);
}

void desenhaPlano(){
    GLubyte indices[4];
    /// Configura as texturas - Normal Map e Color map
    setupTexture("texturas/nmBricks.png","texturas/texBricks.png");

    /// Seta o índice de reflexão especular
    glUniform4fv(ks, 1,glm::value_ptr(glm::vec4(0.8f,0.8f,0.8f,0.0))); ///Seta Componente Especular da Luz

    /// Índices dos vértices que serão utilizados
	GLubyte indicesPlano[4] = {0,1,2,3};
    memcpy(indices, indicesPlano, 4*sizeof(GLubyte));

    glBindVertexArray(vao[0]);
    glDrawElements(GL_TRIANGLE_STRIP,4,GL_UNSIGNED_BYTE,indices);
    destroyTexture();
}

void desenhaEsfera(){
    ///Seta o coeficiente de reflexão especular do material
    glUniform4fv(ks, 1,glm::value_ptr(glm::vec4(1.0f,1.0f,1.0f,1.0)));

    ///Configura Textura - Normal map e Color map
    setupTexture("texturas/nmSphere2.png","texturas/texSphere.png");
    glBindVertexArray(vao[1]);

    float d=2*10.f/10;
    float p0 = -10.f + d/2;

    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++){
            mModel = glm::translate(glm::mat4(1.0f),glm::vec3(p0 + i*d,0.5f,p0 + j*d));
            setUniforms();
            glDrawElements(GL_QUADS, indicesSphere.size(), GL_UNSIGNED_SHORT, &indicesSphere[0]);
            mModel = glm::mat4(1.0f);
            setUniforms();
        }

    destroyTexture();
}

void configPlano(){
    GLuint buffer[4];

    // Posição dos vértices
    float planoCoord[] = {
            -10.0f, 0.0f, -10.0f,
            10.0f, 0.0f, -10.0f,
            -10.0f, 0.0f, 10.0f,
            10.0f, 0.0f, 10.0f};

    // Normais
    float normais[] = {
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f};

    // Cordenada de Textura
    float st[] = {
            3.0f,3.0f,
            3.0f,0.0f,
            0.0f,3.0f,
            0.0f,0.0f};

    // Vetor Tangente
    float tangent[] = {
            1.0f,0.0f,0.0f,
            1.0f,0.0f,0.0f,
            1.0f,0.0f,0.0f,
            1.0f,0.0f,0.0f};

    // Bind no primeiro VAO
    glBindVertexArray(vao[0]);
    // Gera 4 slots de buffer
    glGenBuffers(4, buffer);
    // Copia posições dos vertex para o buffer - vertex position
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planoCoord), planoCoord, GL_STATIC_DRAW);
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    // Vertex Normal
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normais), normais, GL_STATIC_DRAW);
    glVertexAttribPointer(vertexNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexNormal);

    // Coordenada de Textura
    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(st), st, GL_STATIC_DRAW);
    glVertexAttribPointer(vertexST, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexST);

    // Vetor tangente
    glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tangent), tangent, GL_STATIC_DRAW);
    glVertexAttribPointer(tangentLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(tangentLoc);
}

void configEsfera(){
        std::vector<GLfloat> vertices;
        std::vector<GLfloat> normals;
        std::vector<GLfloat> tangent;
        std::vector<GLfloat> texcoords;

        float raio = 0.5;
        unsigned int rings = 50;
        unsigned int sectors = 50;

        float const R = 1/(float)(rings-1); // 0 a rings
        float const S = 1/(float)(sectors-1); // 0 a sectors
        unsigned r, s;

        vertices.resize(rings * sectors * 3);
        normals.resize(rings * sectors * 3);
        tangent.resize(rings * sectors * 3);
        texcoords.resize(rings * sectors * 2);

        std::vector<GLfloat>::iterator v = vertices.begin();
        std::vector<GLfloat>::iterator n = normals.begin();
        std::vector<GLfloat>::iterator t = texcoords.begin();
        std::vector<GLfloat>::iterator tang = tangent.begin();

        for(r = 0; r < rings; r++)
            for(s = 0; s < sectors; s++) {

                float phi = M_PI * r * R;
                float theta = 2 * M_PI * s * S;

                float const x = cos( theta ) * sin( phi );
                float const y = cos( phi );
                float const z = sin( theta ) * sin ( phi );

                ///Textura s,t
                *t++ = s*S;
                *t++ = r*R;

                ///Vertex position
                *v++ = x * raio;
                *v++ = y * raio;
                *v++ = z * raio;

                ///Derivada de x,y,z em relação a s - Vetor tangente
                *tang++ = -sin(theta)*sin(phi)*2*M_PI*S;
                *tang++ =  0;
                *tang++ = cos(theta)*sin(phi)*2*M_PI*S;

                ///Vetor Normal
                *n++ = x;
                *n++ = y;
                *n++ = z;
            }

        indicesSphere.resize(rings * sectors * 4);
        std::vector<GLushort>::iterator i = indicesSphere.begin();
        for(r = 0; r < rings-1; r++) for(s = 0; s < sectors-1; s++) {
            *i++ = r * sectors + s;
            *i++ = r * sectors + (s+1);
            *i++ = (r+1) * sectors + (s+1);
            *i++ = (r+1) * sectors + s;
        }

        GLuint buffer[4];
        glBindVertexArray(vao[1]);
        // Gera 4 slots de buffer
        glGenBuffers(4, buffer);
        // Copia dados para o buffer - posição
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(vertexLoc);
        // Copia dados para o buffer - normais
        glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
        glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(vertexNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(vertexNormal);
        //Copia dados para o buffer - coordenada de Textura
        glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
        glBufferData(GL_ARRAY_BUFFER, texcoords.size()*sizeof(GLfloat), &texcoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(vertexST, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(vertexST);
        //Copia dados para o buffer - vetor tangente
        glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
        glBufferData(GL_ARRAY_BUFFER, tangent.size()*sizeof(GLfloat), &tangent[0], GL_STATIC_DRAW);
        glVertexAttribPointer(tangentLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(tangentLoc);
}

void setupBuffers() {
    //Instancia 2 Vertex Array Object
    glGenVertexArrays(2, vao);

    //Configura o Plano
    configPlano();
    //configura a Esfera
    configEsfera();
}

void setUniforms() {
    ///Model-View-Projection
    glUniformMatrix4fv(modelView,  1, false, glm::value_ptr(mView * mModel));
    glUniformMatrix4fv(projection,  1, false, glm::value_ptr(mProjection));

    ///Matriz de transformação de normais
    glm::mat4 normalMatrix =  glm::transpose(glm::inverse(mView * mModel));
    glUniformMatrix4fv(nMatriz,  1, false, glm::value_ptr(normalMatrix));
}

void getLocations(){
    ///Armazena a localização das variáveis de atributo do vértice
    vertexLoc = glGetAttribLocation(p,"position");
    vertexNormal = glGetAttribLocation(p,"vertexNormal");
    vertexST = glGetAttribLocation(p,"coordinateTexture");
    tangentLoc = glGetAttribLocation(p,"vertexTanget");

    ///Armazena a localização das variáveis uniformes
    modelView = glGetUniformLocation(p, "modelView");
    projection = glGetUniformLocation(p, "projection");
    nMatriz = glGetUniformLocation(p, "normalMatrix");
    ks = glGetUniformLocation(p,"ks");
}

void Display(void) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::vec3 pos = glm::vec3(0.0f, 7.0f, 15.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    mView = glm::lookAt(pos,target,up);

    glUseProgram(p);
    setUniforms();

    setupLight();

    desenhaPlano();

    desenhaEsfera();

    glutSwapBuffers();
}

void Resize(int w, int h) {

    float ratio;

    if(h == 0)
        h = 1;

    /// Viewport é toda a janela
    glViewport(0, 0, w, h);

    ratio = (1.0f * w) / h;
    mProjection = glm::perspective(50.0f, ratio, 1.0f, 100.0f);
}

static void init(){

    GLenum err = glewInit();

    if(GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        exit(-1);
    }

    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glEnable(GL_DEPTH_TEST);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,0);
    glutInitWindowSize(800,600);
    glutCreateWindow("Cena");

    glutDisplayFunc(Display);
    glutReshapeFunc(Resize);

    glewInit();
    if (!glewIsSupported("GL_VERSION_2_1")){
        printf("OpenGL 2.1 nao suportado\n");
        exit(1);
    }

    init();

    p = setupShaders();

    getLocations();

    setupBuffers();

    glutMainLoop();
}
