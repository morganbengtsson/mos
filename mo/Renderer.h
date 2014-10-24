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

#include "Vertex.h"
#include "Texture2D.h"
#include "Model.h"
#include "Particles.h"
#include "Particle.h"

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
        ogli::Uniform texture;
        ogli::Uniform lightmap;
        ogli::Uniform opacity;
        ogli::Uniform light_position;
        ogli::Uniform color;
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
        typedef std::pair<std::string, VertexProgramData> VertexProgramPair;
        typedef std::pair<std::string, ParticleProgramData> ParticleProgramPair;

        Renderer();
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
        void render(const Model & model, 
                    const glm::mat4 transform, 
                    const glm::mat4 view, 
                    const glm::mat4 projection, 
                    const float opacity = 1.0f, 
                    const std::string program_name = "standard", 
                    const glm::vec3 light_position = glm::vec3(0.0f, 0.0f, 0.0f));
        
        void render(const Model & model, 
                    const glm::mat4 view, 
                    const glm::mat4 projection,
                    const float opacity = 1.0f,
                    const std::string program_name = "standard"){
            render(model, glm::mat4(1.0f), view, projection, opacity, program_name);
        }
        
        
        template<class It>
        void render(It begin, It end, const glm::mat4 transform, const glm::mat4 view, 
                    const glm::mat4 projection, const float opacity = 1.0f, 
                    const std::string program_name = "standard", 
                    const glm::vec3 light_position = glm::vec3(0.0f, 0.0f, 0.0f)){
            for (auto it = begin; it != end; ++it){
                render(*it, transform, view, projection, opacity, program_name, light_position);
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
        void render(Particles & particles, 
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

        virtual ~Renderer();
    private:
        
        VertexAttributes vertex_attributes_;
        ParticleAttributes particle_attributes_;
        
        std::map<std::string, VertexProgramData> vertex_programs_;
        std::map<std::string, ParticleProgramData> particle_programs_;
        std::map<unsigned int, ogli::TextureBuffer> textures_;
        std::map<unsigned int, ogli::ArrayBuffer> array_buffers_;
        std::map<unsigned int, ogli::ElementArrayBuffer> element_array_buffers_;

    };
}
#endif	/* OGLI_RENDERER_H */

