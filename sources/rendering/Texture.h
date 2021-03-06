//
// Created by louis on 27/10/16.
//

#ifndef RENDUOPENGL_TEXTURE_H
#define RENDUOPENGL_TEXTURE_H

#include <map>
#include <glad/glad.h>
#include <memory>

class Texture {

public :

    static Texture load(const std::string & path);
    static void clearCache();

    Texture(int width, int height, const unsigned char* pixel,
            GLint internalFormat = GL_RGBA, GLenum format = GL_RGBA);
    Texture(const Texture & other);
    ~Texture();

    GLuint getID() {return this->id;}
private :
    //TODO déplacer ça dans Context
    static std::map<std::string, Texture*> pathToIDMap;

    std::string path;
    GLuint id = 0;
    std::shared_ptr<int> instance_count;
};


#endif //RENDUOPENGL_TEXTURE_H
