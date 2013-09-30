#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include "textfile.h"

void CompileShader(GLuint id){
    GLint status;

    glCompileShader(id);

    glGetShaderiv(id, GL_COMPILE_STATUS, &status);

    if (!status) {
        GLint len;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);

        char* message = (char*) malloc(len*sizeof(char));

        glGetShaderInfoLog(id, len, 0, message);
        printf(message);
        free(message);
    }
}

void LinkProgram(GLuint id){
    GLint status;

    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &status);

    if (!status) {
        GLint len;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
        char* message = (char*) malloc(len*sizeof(char));
        glGetProgramInfoLog(id, len, 0, message);
        printf(message);
        free(message);
        exit(EXIT_FAILURE);
    }
}

GLuint setupShaders() {

    ///Nomes do Shaders
    char vertexFileName[] = "vertex.vert";
    char fragmentFileName[] = "fragment.frag";

    ///Ponteiros para o source do vertex shader e do fragment shader
    char *vs = NULL,*fs = NULL;

    ///Criação do Vertex Shader
    GLuint v = glCreateShader(GL_VERTEX_SHADER);

    if (v==0){
        printf("Erro ao criar o Vertex Shader");
        exit(EXIT_FAILURE);
    }

    ///Criação do Fragment Shader
    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

    if (f==0){
        printf("Erro ao criar o Fragment Shader");
        exit(EXIT_FAILURE);
    }

    ///Leitura dos arquivos com o código dos shaders
    vs = textFileRead(vertexFileName);
    fs = textFileRead(fragmentFileName);

    const char * vv = vs;
    const char * ff = fs;

    glShaderSource(v, 1, &vv,NULL);
    glShaderSource(f, 1, &ff,NULL);

    free(vs);free(fs);
    ///Compila ambos os shadders
    CompileShader(v);
    CompileShader(f);
    ///Criação do programa
    GLuint p = glCreateProgram();

    if (p==0){
        printf("Erro ao criar o programa");
        exit(EXIT_FAILURE);
    }
    ///Vincula os shadders ao programa
    glAttachShader(p,v);
    glAttachShader(p,f);
    ///Linka o programa
    LinkProgram(p);

    return(p);
}
