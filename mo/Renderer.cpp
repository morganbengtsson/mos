/* 
 * File:   Renderer.cpp
 * Author: morgan
 * 
 * Created on February 15, 2014, 2:37 PM
 */

#include <GL/glew.h>
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
        GLenum err = glewInit();
        std::stringstream ss;
        if (GLEW_OK != err) {
            ss << "Error " << glewGetErrorString(err) << std::endl;
        }
        ss << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
        std::cout << ss.str();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(true);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        auto standard_vertex_shader = ogli::createShader(ogli::loadText("standard.vs"), GL_VERTEX_SHADER);
        auto standard_fragment_shader = ogli::createShader(ogli::loadText("standard.fs"), GL_FRAGMENT_SHADER);

        standard_program_ = ogli::createProgram();
        ogli::attachShader(standard_program_, standard_vertex_shader);
        ogli::attachShader(standard_program_, standard_fragment_shader);
        ogli::bindAttribute(standard_program_, position_attribute_3P3N2UV_);
        ogli::bindAttribute(standard_program_, normal_attribute_3P3N2UV_);
        ogli::bindAttribute(standard_program_, uv_attribute_3P3N2UV_);

        ogli::linkProgram(standard_program_);

        standard_mvp_uniform_ = ogli::createUniform(standard_program_, "model_view_projection");
        standard_mv_uniform_ = ogli::createUniform(standard_program_, "model_view");
        standard_texture_uniform_ = ogli::createUniform(standard_program_, "texture");       
    }

    Renderer::~Renderer() {
    }

    void Renderer::clear(const glm::vec3 color) {
        ogli::clearDepth(1.0f);
        ogli::clearColor(glm::vec4(color.r, color.g, color.b, 0.0f));
    }
    
    void Renderer::render(const Model & model, const glm::mat4 transform, const glm::mat4 view, const glm::mat4 projection) {
    if (array_buffers_.find(model.mesh()->id()) == array_buffers_.end()) {
        array_buffers_.insert(ArrayPair(model.mesh()->id(),
                ogli::createArrayBuffer(model.mesh()->verticesBegin(), model.mesh()->verticesEnd())));
    }
    if (element_array_buffers_.find(model.mesh()->id()) == element_array_buffers_.end()) {
        element_array_buffers_.insert(ElementPair(model.mesh()->id(),
                ogli::createElementArrayBuffer(model.mesh()->elementsBegin(), model.mesh()->elementsEnd())));
    }

    if (textures_.find(model.texture()->id()) == textures_.end()) {
        ogli::TextureBuffer texture = ogli::createTexture(model.texture()->begin(), model.texture()->end(), model.texture()->width(), model.texture()->height());
        textures_.insert(std::pair<unsigned int, ogli::TextureBuffer>(model.texture()->id(), texture));
    }

    glm::mat4 mv = projection * view;
    glm::mat4 mvp = projection * view * transform * model.transform();
    ogli::useProgram(standard_program_);
    ogli::bindBuffer(array_buffers_.at(model.mesh()->id()));
    ogli::bindBuffer(element_array_buffers_.at(model.mesh()->id()));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures_.at(model.texture()->id()));

    ogli::uniform(standard_texture_uniform_);
    ogli::uniform(standard_mv_uniform_, mv);
    ogli::uniform(standard_mvp_uniform_, mvp);

    ogli::attribute(position_attribute_3P3N2UV_);
    ogli::attribute(normal_attribute_3P3N2UV_);
    ogli::attribute(uv_attribute_3P3N2UV_);    
    ogli::drawElements(std::distance(model.mesh()->elementsBegin(), model.mesh()->elementsEnd()));
}

}