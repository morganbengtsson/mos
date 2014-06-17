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
        ogli::Uniform camera_position;
    };

    class Renderer {
    public:

        typedef std::pair<unsigned int, ogli::ArrayBuffer> ArrayPair;
        typedef std::pair<unsigned int, ogli::ElementArrayBuffer> ElementPair;
        typedef std::pair<unsigned int, ogli::TextureBuffer> TexturePair;
        typedef std::pair<std::string, ProgramData> ProgramPair;

        Renderer();
        void addProgram(const std::string name);
        void addProgram(const std::string path, const std::string vertex_shader_source, const std::string fragment_shader_source);
        void render(const Model & model, const glm::mat4 transform, const glm::mat4 view, const glm::mat4 projection, const std::string program_name = "assets/standard");
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
        ogli::Uniform standard_camera_position_;

        std::map<std::string, ProgramData> programs_;
        std::map<unsigned int, ogli::TextureBuffer> textures_;
        std::map<unsigned int, ogli::ArrayBuffer> array_buffers_;
        std::map<unsigned int, ogli::ElementArrayBuffer> element_array_buffers_;

        static const std::string standard_vertex_source = "#ifdef GL_ES"
                "precision mediump float;"
                "precision mediump int;"
                "#endif"
                "uniform mat4 model_view_projection; "
                "uniform mat4 model_view; "
                "attribute vec3 position;"
                "attribute vec3 normal;"
                "attribute vec2 uv;"
                "varying vec3 v_position;"
                "varying vec3 v_normal;"
                "varying vec3 v_position2;"
                "varying vec2 v_uv;"
                "void main()"
                "{"
                "    v_uv = uv;"
                "    v_position2 = normalize((model_view_projection * vec4(position, 1.0)).xyz);"
                "    v_position = (model_view * vec4(position, 0.0)).xyz;"
                "    v_normal = normalize((model_view * vec4(normal, 0.0)).xyz);"
                "    gl_Position = model_view_projection * vec4(position, 1.0);"
                "}";

        static const std::string standard_fragment_source = "#ifdef GL_ES"
                "precision mediump float;"
                "precision mediump int;"
                "#endif"
                "uniform vec4 color;"
                "uniform sampler2D texture;"
                "varying vec3 v_position;"
                "varying vec3 v_normal; "
                "arying vec3 v_position2;"
                "varying vec2 v_uv;"
                "void main() {"
                "vec3 ambient = vec3(0.3, 0.3, 0.3);"
                "float d = (v_position.z + 100.0)/5000.0;"

                "vec3 light = normalize(vec3(0.0, 500.0, 100.0) - v_position2);"
                "float intensity = max(dot(v_normal,light), 0.0) * 0.2;     "

                "vec3 indirect = texture2D(texture, v_uv).rgb;"

                "gl_FragColor = vec4(indirect + intensity, 1.0);"

                "}";

    };
}
#endif	/* OGLI_RENDERER_H */

