/* 
 * File:   Renderer.cpp
 * Author: morgan
 * 
 * Created on February 15, 2014, 2:37 PM
 */

#include <ogli/util.h>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/random.hpp>

#include "Renderer.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Texture2D.h"
#include "Model.h"

namespace mo {

    Renderer::Renderer() :    
    position_attribute_3P3N2UV_(0, 3, "position", sizeof (Vertex), sizeof (glm::vec3), 0),
    normal_attribute_3P3N2UV_(1, 3, "normal", sizeof (Vertex), sizeof (glm::vec3), sizeof (glm::vec3)),
    uv_attribute_3P3N2UV_(2, 2, "uv", sizeof (Vertex), sizeof (glm::vec2), sizeof (glm::vec3) + sizeof (glm::vec3)) {
        ogli::init(); // Should this be done int ogli or mo?

        glEnable(GL_DEPTH_TEST);
#ifdef __ANDROID__
#else
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif
        glDepthFunc(GL_LEQUAL);
        glDepthMask(true);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        std::string standard_vertex_source = "#ifdef GL_ES\n"
                "precision mediump float;\n"
                "precision mediump int;\n"
                "#else\n"
                "#version 120\n"
                "#endif\n"
                "uniform mat4 model;\n"
                "uniform mat4 model_view_projection;\n"
                "uniform mat4 model_view;\n"
                "attribute vec3 position;\n"
                "attribute vec3 normal;\n"
                "attribute vec2 uv;\n"
                "varying vec3 fragment_position;\n"
                "varying vec3 fragment_normal\n;"                
                "varying vec2 fragment_uv;\n"
                "varying mat3 normal_matrix;\n"
                "void main()\n"
                "{\n"
                "    fragment_uv = uv;\n"
                "    fragment_position = (model_view * vec4(position, 1.0)).xyz;\n"
                "    normal_matrix = mat3(model_view);\n"
                "    fragment_normal = normal_matrix * normal;\n"
                "    gl_Position = model_view_projection * vec4(position, 1.0);\n"
                "}\n";
        
        std::string standard_fragment_source = "#ifdef GL_ES\n"
                "precision mediump float;\n"
                "precision mediump int;\n"
                "#else\n"
                "#version 120\n"
                "#endif\n"               
                "uniform vec4 color;\n"
                "uniform float opacity;\n"
                "uniform sampler2D texture;\n"
                "uniform vec3 light_position;\n"
                "varying vec3 fragment_position;\n"
                "varying vec3 fragment_normal;\n"
                "varying vec2 fragment_uv;\n"                               
                
                "void main() {\n"
                    "vec3 normal = fragment_normal;\n"
                    "vec3 surface_to_light = normalize(light_position - fragment_position);\n"
                    "float diff = max(dot(normal, surface_to_light), 0.0);\n"
                    "diff = clamp(diff, 0.0, 1.0);\n"
                    "float intensity = dot(normal, surface_to_light) / (length(surface_to_light) * length(normal));\n"
                    "float dist = length(surface_to_light);\n"
                    //"float att = 1.0 / (1.0 + 0.1*dist + 0.01*dist*dist);\n"
                    "intensity = clamp(intensity, 0.0, 1.0);\n"                    
                    "vec4 diffuse = texture2D(texture, fragment_uv).rgba;\n"
                    "vec3 ambient = vec3(0.1, 0.1, 0.1);\n"
                     
                    "gl_FragColor = diffuse;\n"
                    //"gl_FragColor = vec4((diffuse.rgb + 0.5 * diff) * color.rgb, diffuse.a * opacity);\n"
                    
                //"gl_FragColor = vec4(diff, diff, diff, opacity);\n"
                    //"gl_FragColor = vec4((intensity*diffuse.rgb) + ambient, diffuse.a * opacity);\n"
                
                "}\n";
        addProgram("standard", standard_vertex_source, standard_fragment_source);

        std::string text_vertex_source = "#ifdef GL_ES\n"
                "precision mediump float;\n"
                "precision mediump int;\n"
                "#else\n"
                "#version 120\n"
                "#endif\n"
                "uniform mat4 model_view_projection;\n"
                "uniform mat4 model_view;\n"
                "attribute vec3 position;\n"
                "attribute vec2 uv;\n"

                "varying vec2 v_position;\n"
                "varying vec2 v_uv;\n"

                "void main(){\n"               
                "v_position = (model_view * vec4(position, 0.0)).xy;\n"
                "v_uv = uv;\n"
                "gl_Position = model_view_projection * vec4(position, 1.0);\n"
                "}\n";
        
        std::string text_fragment_source = "#ifdef GL_ES\n"
                "precision mediump float;\n"
                "precision mediump int;\n"
                "#else\n"
                "#version 120\n"
                "#endif\n"
                "varying vec3 v_position;\n"
                "varying vec2 v_uv;\n"
                "uniform sampler2D texture;\n"
                "uniform float opacity;\n"

                "void main() {\n"
                "gl_FragColor = texture2D(texture, v_uv);\n"
                "gl_FragColor.w = gl_FragColor.w * opacity;\n"
                "//gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
                "}\n";
        addProgram("text", text_vertex_source, text_fragment_source);
            
        std::string particles_vertex_source = "#ifdef GL_ES\n"
                "precision mediump float;\n"
                "precision mediump int;\n"
                "#else\n"
                "#version 120\n"
                "#endif\n"
                "uniform mat4 model_view_projection;\n"
                "uniform mat4 model_view;\n"
                "attribute vec3 position;\n"
                "attribute vec2 uv;\n"

                "varying vec2 v_position;\n"
                "varying vec2 v_uv;\n"

                "void main(){\n"
                "#ifdef GL_ES\n"
                "#else\n"
                "gl_PointSize = 3.0;\n"
                "#endif\n"                
                "v_position = (model_view * vec4(position, 0.0)).xy;\n"
                "v_uv = uv;\n"
                "gl_Position = model_view_projection * vec4(position, 1.0);\n"
                "}\n";
        
        std::string particles_fragment_source = "#ifdef GL_ES\n"
                "precision mediump float;\n"
                "precision mediump int;\n"
                "#else\n"
                "#version 120\n"
                "#endif\n"
                "varying vec3 v_position;\n"
                "varying vec2 v_uv;\n"
                
                "uniform float opacity;\n"

                "void main() {\n"                
                "gl_FragColor = vec4(1.0, 1.0, 1.0, opacity);\n"               
                "}\n";
        
        addProgram("particles", particles_vertex_source, particles_fragment_source);
    }

    Renderer::~Renderer() {
    }

    void Renderer::addProgram(const std::string path, const std::string vertex_shader_source, const std::string fragment_shader_source) {
        auto vertex_shader = ogli::createShader(vertex_shader_source, GL_VERTEX_SHADER);
        auto fragment_shader = ogli::createShader(fragment_shader_source, GL_FRAGMENT_SHADER);

        auto program = ogli::createProgram();
        ogli::attachShader(program, vertex_shader);
        ogli::attachShader(program, fragment_shader);
        ogli::bindAttribute(program, position_attribute_3P3N2UV_);
        ogli::bindAttribute(program, normal_attribute_3P3N2UV_);
        ogli::bindAttribute(program, uv_attribute_3P3N2UV_);

        ogli::linkProgram(program);

        auto mvp_uniform = ogli::createUniform(program, "model_view_projection");
        auto mv_uniform = ogli::createUniform(program, "model_view");
        auto texture_uniform = ogli::createUniform(program, "texture");
        auto opacity_uniform = ogli::createUniform(program, "opacity");
        auto light_uniform = ogli::createUniform(program, "light_position");
        auto color_uniform = ogli::createUniform(program, "color");

        programs_.insert(ProgramPair(path, ProgramData{program, mvp_uniform, mv_uniform, texture_uniform, opacity_uniform, light_uniform, color_uniform}));
    }

    void Renderer::addProgram(const std::string path) {
        addProgram(path, ogli::loadText(path + ".vs"), ogli::loadText(path + ".fs"));
    }

    void Renderer::clear(const glm::vec3 color) {
        ogli::clearDepth(1.0f);
        ogli::clearColor(glm::vec4(color.r, color.g, color.b, 0.0f));
    }

    void Renderer::render(const Model & model, const glm::mat4 transform, const glm::mat4 view, const glm::mat4 projection, const float opacity, const std::string program_name, const glm::vec3 light_position) {
           
        
        if (array_buffers_.find(model.mesh->id()) == array_buffers_.end()) {
            array_buffers_.insert(ArrayPair(model.mesh->id(),
                    ogli::createArrayBuffer(model.mesh->verticesBegin(), model.mesh->verticesEnd())));
        }
        if (element_array_buffers_.find(model.mesh->id()) == element_array_buffers_.end()) {
            element_array_buffers_.insert(ElementPair(model.mesh->id(),
                    ogli::createElementArrayBuffer(model.mesh->elementsBegin(), model.mesh->elementsEnd())));
        }
        
        if (!model.mesh->valid) {
            ogli::updateArrayBuffer(array_buffers_.at(model.mesh->id()), model.mesh->verticesBegin(), model.mesh->verticesEnd());
            model.mesh->valid = true;
        }     

        if (textures_.find(model.texture->id()) == textures_.end()) {
            ogli::TextureBuffer texture = ogli::createTexture(model.texture->begin(), model.texture->end(), model.texture->width(), model.texture->height(), model.texture->mipmaps);
            textures_.insert(std::pair<unsigned int, ogli::TextureBuffer>(model.texture->id(), texture));
        }

        glm::mat4 mv = view * transform * model.transform;
        glm::mat4 mvp = projection * view * model.transform * transform;

        //ogli::useProgram(standard_program_);

        ogli::useProgram(programs_.at(program_name).program);

        //ogli::useProgram(std::get<0>(programs_.at(program_name)));
        ogli::bindBuffer(array_buffers_.at(model.mesh->id()));
        ogli::bindBuffer(element_array_buffers_.at(model.mesh->id()));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures_.at(model.texture->id()));
        
        ogli::uniform(programs_.at(program_name).mvp, mvp);
        ogli::uniform(programs_.at(program_name).mv, mv);
        ogli::uniform(programs_.at(program_name).texture);
        ogli::uniform(programs_.at(program_name).opacity, opacity);
        ogli::uniform(programs_.at(program_name).light_position, light_position);
        ogli::uniform(programs_.at(program_name).color, model.color);
        

        ogli::attribute(position_attribute_3P3N2UV_);
        ogli::attribute(normal_attribute_3P3N2UV_);
        ogli::attribute(uv_attribute_3P3N2UV_);
        
        int num_elements = std::distance(model.mesh->elementsBegin(), model.mesh->elementsEnd());
        int draw_type = GL_TRIANGLES;
        if (model.draw == Model::Draw::LINES){
            draw_type = GL_LINES;
        }
        else if (model.draw == Model::Draw::POINTS){
            draw_type = GL_POINTS;
        }       
                
        if (num_elements > 0){            
            ogli::drawElements(num_elements, draw_type);            
        }
        else {
            ogli::drawArrays(std::distance(model.mesh->verticesBegin(), model.mesh->verticesEnd()), draw_type);
        }
        
    }

}