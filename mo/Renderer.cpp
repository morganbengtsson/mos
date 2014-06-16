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
        glDepthFunc(GL_LEQUAL);
        glDepthMask(true);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        addProgram("assets/standard");
        
    }

    Renderer::~Renderer() {
    }
    
    void Renderer::addProgram(const std::string path){
        auto vertex_shader = ogli::createShader(ogli::loadText(path + ".vs"), GL_VERTEX_SHADER);
        auto fragment_shader = ogli::createShader(ogli::loadText(path + ".fs"), GL_FRAGMENT_SHADER);
               
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
        auto camera_position_uniform = ogli::createUniform(program, "camera_position");        
        
        programs_.insert(ProgramPair(path, ProgramData{program, mvp_uniform, mv_uniform, texture_uniform, camera_position_uniform}));
    }

    void Renderer::clear(const glm::vec3 color) {
        ogli::clearDepth(1.0f);
        ogli::clearColor(glm::vec4(color.r, color.g, color.b, 0.0f));
    }
    
    void Renderer::render(const Model & model, const glm::mat4 transform, const glm::mat4 view, const glm::mat4 projection, const std::string program_name) {
    if (array_buffers_.find(model.mesh->id()) == array_buffers_.end()) {
        array_buffers_.insert(ArrayPair(model.mesh->id(),
                ogli::createArrayBuffer(model.mesh->verticesBegin(), model.mesh->verticesEnd())));
    }
    if (element_array_buffers_.find(model.mesh->id()) == element_array_buffers_.end()) {
        element_array_buffers_.insert(ElementPair(model.mesh->id(),
                ogli::createElementArrayBuffer(model.mesh->elementsBegin(), model.mesh->elementsEnd())));
    }

    if (textures_.find(model.texture->id()) == textures_.end()) {
        ogli::TextureBuffer texture = ogli::createTexture(model.texture->begin(), model.texture->end(), model.texture->width(), model.texture->height());
        textures_.insert(std::pair<unsigned int, ogli::TextureBuffer>(model.texture->id(), texture));
    }

    glm::mat4 mv = projection * view;
    glm::mat4 mvp = projection * view * transform * model.transform();
    
    //ogli::useProgram(standard_program_);
    
    ogli::useProgram(programs_.at(program_name).program);
    
    //ogli::useProgram(std::get<0>(programs_.at(program_name)));
    ogli::bindBuffer(array_buffers_.at(model.mesh->id()));
    ogli::bindBuffer(element_array_buffers_.at(model.mesh->id()));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures_.at(model.texture->id()));
    
    //TODO: General lighting, this is a hack.
    glm::mat3 rot_mat(view);
    glm::vec3 dir(view[3]);

    glm::vec3 camera_pos = -dir * rot_mat;    
  
    ogli::uniform(programs_.at(program_name).mvp, mvp);
    ogli::uniform(programs_.at(program_name).mv, mv);    
    ogli::uniform(programs_.at(program_name).texture);
    ogli::uniform(programs_.at(program_name).camera_position, camera_pos);   
    
    
    ogli::attribute(position_attribute_3P3N2UV_);
    ogli::attribute(normal_attribute_3P3N2UV_);
    ogli::attribute(uv_attribute_3P3N2UV_);    
    ogli::drawElements(std::distance(model.mesh->elementsBegin(), model.mesh->elementsEnd()));
}

}