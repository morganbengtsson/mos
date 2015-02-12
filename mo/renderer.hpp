/* 
 * File:   Renderer.h
 * Author: morgan
 *
 * Created on February 15, 2014, 2:37 PM
 */

#ifndef OGLI_RENDERER_H
#define	OGLI_RENDERER_H

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
        ogli::Program program;
        ogli::Uniform mvp;
        ogli::Uniform mv;
        ogli::Uniform normal_matrix;
        ogli::Uniform texture;
        ogli::Uniform lightmap;
        ogli::Uniform normalmap;
        ogli::Uniform material_ambient_color;
        ogli::Uniform material_diffuse_color;
        ogli::Uniform material_specular_color;
        ogli::Uniform material_specular_exponent;
        ogli::Uniform opacity;
        ogli::Uniform light_position;        
        ogli::Uniform light_diffuse_color;
        ogli::Uniform light_specular_color;
        ogli::Uniform has_texture;
        ogli::Uniform has_lightmap;
        ogli::Uniform has_normalmap;
        ogli::Uniform selected;
        ogli::Uniform time;
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

        void render_target_reset(){
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void render_target(RenderTarget target){
            if (frame_buffers_.find(target.id()) == frame_buffers_.end()) {
                auto id = ogli::create_frame_buffer();


                glBindFramebuffer(GL_FRAMEBUFFER, id);

                // The texture we're going to render to
                //GLuint renderedTexture;
                //glGenTextures(1, &renderedTexture);
                auto t = ogli::createTexture(nullptr, 1024, 768);

                // "Bind" the newly created texture : all future texture functions will modify this texture
                glBindTexture(GL_TEXTURE_2D, t);

                // Give an empty image to OpenGL ( the last "0" )
                glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, 1024, 768, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

                // Poor filtering. Needed !
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                textures_.insert(TexturePair(target.texture->id(), t));
                // The depth buffer
                GLuint depthrenderbuffer;
                glGenRenderbuffers(1, &depthrenderbuffer);
                glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

                // Set "renderedTexture" as our colour attachement #0
                glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t, 0);

                // Set the list of draw buffers.
                GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
                glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
                frame_buffers_.insert(FrameBufferPair(target.id(), id));

                if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
                    throw std::runtime_error("Framebuffer is incomplete.");
                }

            }

            auto fb = frame_buffers_.at(target.id());
            glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
            glViewport(0,0,1024,768);
        }

    private:

        VertexAttributes vertex_attributes_;
        ParticleAttributes particle_attributes_;

        std::map<std::string, VertexProgramData> vertex_programs_;
        std::map<std::string, ParticleProgramData> particle_programs_;
        std::map<unsigned int, ogli::TextureBuffer> textures_;
        std::map<unsigned int, ogli::ArrayBuffer> array_buffers_;
        std::map<unsigned int, ogli::ElementArrayBuffer> element_array_buffers_;
        std::map<unsigned int, ogli::FrameBuffer> frame_buffers_;


    };
}
#endif	/* OGLI_RENDERER_H */

