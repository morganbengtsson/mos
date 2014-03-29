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

#include "Vertex.h"
#include "Texture2D.h"
#include "Model.h"

namespace mo {

    class Renderer {
    public:

        typedef std::pair<unsigned int, ogli::ArrayBuffer> ArrayPair;
        typedef std::pair<unsigned int, ogli::ElementArrayBuffer> ElementPair;
        typedef std::pair<unsigned int, ogli::TextureBuffer> TexturePair;

        Renderer();
        void render(const Model model, const glm::mat4 transform);
        void clear(const glm::vec3 color);

        virtual ~Renderer();
    private:
        ogli::Attribute position_attribute_3P3N2UV_;
        ogli::Attribute normal_attribute_3P3N2UV_;
        ogli::Attribute uv_attribute_3P3N2UV_;

        ogli::Program standard_program_;
        ogli::Uniform standard_mvp_uniform_;
        ogli::Uniform standard_mv_uniform_;
        ogli::Uniform standard_texture_uniform_;

        std::map<unsigned int, ogli::TextureBuffer> textures_;
        std::map<unsigned int, ogli::ArrayBuffer> array_buffers_;
        std::map<unsigned int, ogli::ElementArrayBuffer> element_array_buffers_;

        glm::mat4 projection_;
        glm::mat4 view_;
    };
}
#endif	/* OGLI_RENDERER_H */

