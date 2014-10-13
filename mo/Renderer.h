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

namespace mo {

    struct ProgramData {
        ogli::Program program;
        ogli::Uniform mvp;
        ogli::Uniform mv;
        ogli::Uniform texture;
        ogli::Uniform lightmap;
        ogli::Uniform opacity;
        ogli::Uniform light_position;
        ogli::Uniform color;
    };

    /*!
     * The class that talks to OpenGL, and renders Model objects.
     */
    class Renderer {
    public:

        typedef std::pair<unsigned int, ogli::ArrayBuffer> ArrayPair;
        typedef std::pair<unsigned int, ogli::ElementArrayBuffer> ElementPair;
        typedef std::pair<unsigned int, ogli::TextureBuffer> TexturePair;
        typedef std::pair<std::string, ProgramData> ProgramPair;

        Renderer();
        void addProgram(const std::string name);
        void addProgram(const std::string path, const std::string vertex_shader_source, const std::string fragment_shader_source);
        
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
        void render(const Model & model, const glm::mat4 transform, const glm::mat4 view, const glm::mat4 projection, const float opacity = 1.0f, const std::string program_name = "standard", const glm::vec3 light_position = glm::vec3(0.0f, 0.0f, 0.0f));
        
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
         * Clears the screen and the depth buffer.
         * @param color
         */
        void clear(const glm::vec3 color);
        
        void clean(){
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
        ogli::Attribute position_attribute_3P3N2UV2UV_;
        ogli::Attribute normal_attribute_3P3N2UV2UV_;
        ogli::Attribute uv_attribute_3P3N2UV2UV_;
        ogli::Attribute uv_lightmap_3P3N2UV2UV_;
        
        std::map<std::string, ProgramData> programs_;
        std::map<unsigned int, ogli::TextureBuffer> textures_;
        std::map<unsigned int, ogli::ArrayBuffer> array_buffers_;
        std::map<unsigned int, ogli::ElementArrayBuffer> element_array_buffers_;

    };
}
#endif	/* OGLI_RENDERER_H */

