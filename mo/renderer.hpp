/* 
 * File:   Renderer.h
 * Author: morgan
 *
 * Created on February 15, 2014, 2:37 PM
 */

#ifndef MO_RENDERER_H
#define	MO_RENDERER_H

#include <ogli/ogli.h>
#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <tuple>

#include "vertex.hpp"
#include "texture2d.hpp"
#include "model.hpp"
#include "particles.hpp"
#include "particle.hpp"
#include "light.hpp"
#include "render_target.hpp"

namespace mo {

struct ParticleProgramData{
    ogli::Program program;
    ogli::Uniform mvp;
    ogli::Uniform mv;
};

struct VertexProgramData {
    unsigned int program;
    int mvp;
    int mv;
    int normal_matrix;
    int texture;
    int lightmap;
    int normalmap;
    int material_ambient_color;
    int material_diffuse_color;
    int material_specular_color;
    int material_specular_exponent;
    int opacity;
    int light_position;
    int light_diffuse_color;
    int light_specular_color;
    int has_texture;
    int has_lightmap;
    int has_normalmap;
    int selected;
    int time;
};

struct VertexAttributes{
    VertexAttributes():
        position(0, 3, "position", sizeof (Vertex), sizeof (glm::vec3), 0),
        normal(1, 3, "normal", sizeof (Vertex), sizeof (glm::vec3), sizeof (glm::vec3)),
        uv_texture(2, 2, "uv", sizeof (Vertex), sizeof (glm::vec2), sizeof (glm::vec3) + sizeof (glm::vec3)),
        uv_lightmap(3, 2, "uv_lightmap", sizeof(Vertex), sizeof(glm::vec2), sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2))
    {
    }
    ogli::Attribute position;
    ogli::Attribute normal;
    ogli::Attribute uv_texture;
    ogli::Attribute uv_lightmap;
};

struct ParticleAttributes{
    ParticleAttributes():
        position(0, 3, "position", sizeof(Particle), sizeof(glm::vec3), 0),
        color(1, 4, "color", sizeof(Particle), sizeof(glm::vec3), sizeof(glm::vec3))
    {
    }
    ogli::Attribute position;
    ogli::Attribute color;
};

/*!
     * The class that talks to OpenGL, and renders Model objects.
     */
class Renderer {
public:

    typedef std::pair<unsigned int, ogli::ArrayBuffer> ArrayPair;
    typedef std::pair<unsigned int, ogli::ElementArrayBuffer> ElementPair;
    typedef std::pair<unsigned int, ogli::TextureBuffer> TexturePair;
    typedef std::pair<unsigned int, ogli::FrameBuffer> FrameBufferPair;
    typedef std::pair<std::string, VertexProgramData> VertexProgramPair;
    typedef std::pair<std::string, ParticleProgramData> ParticleProgramPair;

    Renderer();
    virtual ~Renderer();
    void add_program(const std::string name);
    void add_vertex_program(const std::string path,
                            const std::string vertex_shader_source,
                            const std::string fragment_shader_source);
    void add_particle_program(const std::string name,
                              const std::string vs_source,
                              const std::string fs_source);
    /**
     * Renders a Model object.
     *
     * @param model.
     * @param Additional transform matrix.
     * @param View matrix of the camera
     * @param Projection matrix of the camera
     * @param Custom opacity of the object.
     * @param Program_name, either "text" or "standard"
     * @param Position of one ortho light.
     */
    void update(const Model & model,
                const glm::mat4 transform,
                const glm::mat4 view,
                const glm::mat4 projection,
                const float opacity = 1.0f,
                const std::string program_name = "standard",
                const Light & light = Light(),
                const float time = 0.0f);
    /*
        void render(const Model & model,
                    const glm::mat4 view,
                    const glm::mat4 projection,
                    const float opacity = 1.0f,
                    const std::string program_name = "standard"){
            render(model, glm::mat4(1.0f), view, projection, opacity, program_name);
        }
         * */


    template<class It>
    void update(It begin, It end, const glm::mat4 transform, const glm::mat4 view,
                const glm::mat4 projection, const float opacity = 1.0f,
                const std::string program_name = "standard",
                const Light & light = Light(), const float time = 0.0f){
        for (auto it = begin; it != end; ++it){
            update(*it, transform, view, projection, opacity, program_name, light, time);
        }
    }


    /**
         * Renders particles.
         *
         * @param Particles object.
         * @param View matrix.
         * @param Projection matrix.
         * @param Custom opacity of the object.
         */
    void update(Particles & particles,
                const glm::mat4 view,
                const glm::mat4 projection);

    /**
         * Clears the screen and the depth buffer.
         * @param color
         */
    void clear(const glm::vec3 color);

    void clear_buffers(){
        for (auto & texture : textures_) {
            glDeleteTextures(1, &texture.second.id);
        }
        textures_.clear();

        for (auto & ab : array_buffers_) {
            glDeleteBuffers(1, &ab.second.id);
        }
        array_buffers_.clear();

        for (auto & eab : element_array_buffers_){
            glDeleteBuffers(1, &eab.second.id);
        }
        element_array_buffers_.clear();
    }

    void render_target_reset(unsigned int width, unsigned int height){
        glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFBO);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint readFBO;
    GLuint drawFBO;

    void render_target(RenderTarget target){
        if (frame_buffers_.find(target.id()) == frame_buffers_.end()) {

            glGenFramebuffers(1, &readFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, readFBO);

            GLuint renderedTexture;
            glGenTextures(1, &renderedTexture);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderedTexture);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_SRGB8_ALPHA8, target.texture->width(), target.texture->height(), GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, renderedTexture, 0);

            GLuint depthrenderbuffer;
            glGenRenderbuffers(1, &depthrenderbuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, target.texture->width(), target.texture->height());
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

            GLuint screenTexture;
            glGenTextures(1, &screenTexture);
            glBindTexture(GL_TEXTURE_2D, screenTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, target.texture->width(), target.texture->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            glGenFramebuffers(1, &drawFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, drawFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

            // Always check that our framebuffer is ok
            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
                throw std::runtime_error("Framebuffer incomplete.");
            }
            //textures_.insert(TexturePair(target.texture->id(), ogli::TextureBuffer{renderedTexture}));
            textures_.insert(TexturePair(target.texture->id(), ogli::TextureBuffer{screenTexture}));
            frame_buffers_.insert(FrameBufferPair(target.id(), ogli::FrameBuffer{readFBO}));


            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        auto fb = frame_buffers_.at(target.id());
       //std::cout << fb.id << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
        //std::cout << target.texture->width() << " " << target.texture->height() << std::endl;
        //glViewport(0,0,target.texture->width(), target.texture->height());
    }

private:

    unsigned int create_shader(const std::string source, const unsigned int type);
    bool check_shader(const unsigned int shader);
    bool check_program(const unsigned int program);

    VertexAttributes vertex_attributes_;
    ParticleAttributes particle_attributes_;

    std::map<std::string, VertexProgramData> vertex_programs_;
    std::map<std::string, ParticleProgramData> particle_programs_;
    std::map<unsigned int, ogli::TextureBuffer> textures_;
    std::map<unsigned int, ogli::ArrayBuffer> array_buffers_;
    std::map<unsigned int, ogli::ElementArrayBuffer> element_array_buffers_;
    std::map<unsigned int, ogli::FrameBuffer> frame_buffers_;

    std::map<unsigned int, unsigned int> array_buffers2_;
    std::map<unsigned int, unsigned int> element_array_buffers2_;
    std::map<unsigned int, unsigned int> vertex_arrays_;


};
}
#endif	/* MO_RENDERER_H */

