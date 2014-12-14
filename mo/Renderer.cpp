/* 
 * File:   Renderer.cpp
 * Author: morgan
 * 
 * Created on February 15, 2014, 2:37 PM
 */

#include "GL/glew.h"


#include <ogli/util.h>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/io.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "Renderer.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Texture2D.h"
#include "Model.h"

namespace mo {

    Renderer::Renderer() {
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
        glEnable(GL_FRAMEBUFFER_SRGB);

        std::string standard_vertex_source = "#ifdef GL_ES\n"
                "precision mediump float;\n"
                "precision mediump int;\n"
                "#else\n"
                "#version 120\n"
                "#endif\n"
                "uniform mat4 model;\n"
                "uniform mat4 model_view_projection;\n"
                "uniform mat4 model_view;\n"
                "uniform mat3 normal_matrix;\n"
                "attribute vec3 position;\n"
                "attribute vec3 normal;\n"
                "attribute vec2 uv;\n"
                "attribute vec2 lightmap_uv;\n"
                "varying vec3 fragment_position;\n"
                "varying vec3 fragment_normal\n;"
                "varying vec2 fragment_uv;\n"
                "varying vec2 fragment_lightmap_uv;\n"
                "void main()\n"
                "{\n"
                "    fragment_uv = uv;\n"
                "    fragment_lightmap_uv = lightmap_uv;\n"
                "    fragment_position = (model_view * vec4(position, 1.0)).xyz;\n"
                "    fragment_normal = normal_matrix * normal;\n"
                "    gl_Position = model_view_projection * vec4(position, 1.0);\n"
                "}\n";

        std::string standard_fragment_source = "#ifdef GL_ES\n"
                "precision mediump float;\n"
                "precision mediump int;\n"
                "#else\n"
                "#version 120\n"
                "#endif\n"
                "uniform vec3 material_ambient_color;\n"
                "uniform vec3 material_diffuse_color;\n"
                "uniform vec3 material_specular_color;\n"
                "uniform float material_specular_exponent;\n"
                "uniform float opacity;\n"
                "uniform sampler2D texture;\n"
                "uniform sampler2D lightmap;\n"
                "uniform vec3 light_position;\n"
                "uniform vec3 light_diffuse_color;\n"
                "uniform vec3 light_specular_color;\n"
                "uniform bool has_texture;\n"
                "uniform bool has_lightmap;\n"
                "varying vec3 fragment_position;\n"
                "varying vec3 fragment_normal;\n"
                "varying vec2 fragment_uv;\n"
                "varying vec2 fragment_lightmap_uv;\n"

                "void main() {\n"

                "vec4 indirect = texture2D(lightmap, fragment_lightmap_uv);\n"

                "vec3 normal = normalize(fragment_normal);\n"
                "vec3 surface_to_light = normalize(light_position - fragment_position);\n"
                "float diffuse_contribution = max(dot(normal, surface_to_light), 0.0);\n"
                "diffuse_contribution = clamp(diffuse_contribution, 0.0, 1.0);\n"
                "vec4 diffuse_color;"
                "if (has_texture == true){\n"
                "diffuse_color = texture2D(texture, fragment_uv).rgba;\n"
                "} else {"
                "diffuse_color = vec4(material_diffuse_color, 1.0);\n"
                "}\n"
                "float dist = distance(light_position, fragment_position);\n"
                "float a = 1.0;\n"
                "float b = 1.0;\n"
                "float att = 1.0 / (1.0 + a*dist + b*dist*dist);\n"
                "vec4 diffuse = vec4(att * diffuse_contribution* light_diffuse_color, 1.0) * diffuse_color;\n"                
                
                "vec3 surface_to_view = normalize(fragment_position);\n"
                "vec3 reflection = reflect(normal, -surface_to_light);\n"
                "float secular_contribution = pow(max(0.0, dot(surface_to_view, reflection)), material_specular_exponent);\n"
                "vec4 specular = vec4(secular_contribution * light_specular_color * material_specular_color, 1.0);\n"

                "vec4 diffuse_indirect = indirect * diffuse_color;"
                
                "gl_FragColor = vec4(diffuse.xyz + diffuse_indirect.xyz + specular.xyz, 1.0);\n"               
                
                "}\n";
        add_vertex_program("standard", standard_vertex_source, standard_fragment_source);

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
        add_vertex_program("text", text_vertex_source, text_fragment_source);

        std::string particles_vertex_source =
                "#ifdef GL_ES\n"
                "precision mediump float;\n"
                "precision mediump int;\n"
                "#else\n"
                "#version 120\n"
                "#endif\n"
                "uniform mat4 model_view_projection;\n"
                "uniform mat4 model_view;\n"
                "attribute vec3 position;\n"
                "attribute vec4 color;\n"

                "varying vec3 v_position;\n"
                "varying vec4 v_color;\n"

                "void main(){\n"
                "#ifdef GL_ES\n"
                "#else\n"
                "gl_PointSize = 1.0;\n"
                "#endif\n"
                "v_color = color;\n"
                "v_position = (model_view * vec4(position, 0.0)).xyz;\n"
                "gl_Position = model_view_projection * vec4(position, 1.0);\n"
                "}\n";

        std::string particles_fragment_source = "#ifdef GL_ES\n"
                "precision mediump float;\n"
                "precision mediump int;\n"
                "#else\n"
                "#version 120\n"
                "#endif\n"
                "varying vec3 v_position;\n"
                "varying vec4 v_color;\n"

                "void main() {\n"
                "gl_FragColor = vec4(v_color.xyz, 1.0);\n"

                //"gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
                "}\n";

        add_particle_program("particles", particles_vertex_source, particles_fragment_source);

    }

    Renderer::~Renderer() {
    }

    void Renderer::add_particle_program(const std::string name, const std::string vs_source, const std::string fs_source) {
        auto vertex_shader = ogli::createShader(vs_source, GL_VERTEX_SHADER);
        auto fragment_shader = ogli::createShader(fs_source, GL_FRAGMENT_SHADER);

        auto program = ogli::createProgram();
        ogli::attachShader(program, vertex_shader);
        ogli::attachShader(program, fragment_shader);
        ogli::bindAttribute(program, particle_attributes_.position);
        ogli::bindAttribute(program, particle_attributes_.color);
        ogli::linkProgram(program);

        auto mvp_uniform = ogli::createUniform(program, "model_view_projection");
        auto mv_uniform = ogli::createUniform(program, "model_view");

        particle_programs_.insert(ParticleProgramPair(name, ParticleProgramData{program, mvp_uniform, mv_uniform}));

    }

    void Renderer::add_vertex_program(const std::string path, const std::string vertex_shader_source, const std::string fragment_shader_source) {
        auto vertex_shader = ogli::createShader(vertex_shader_source, GL_VERTEX_SHADER);
        auto fragment_shader = ogli::createShader(fragment_shader_source, GL_FRAGMENT_SHADER);

        auto program = ogli::createProgram();
        ogli::attachShader(program, vertex_shader);
        ogli::attachShader(program, fragment_shader);
        ogli::bindAttribute(program, vertex_attributes_.position);
        ogli::bindAttribute(program, vertex_attributes_.normal);
        ogli::bindAttribute(program, vertex_attributes_.uv_texture);
        ogli::bindAttribute(program, vertex_attributes_.uv_lightmap);

        ogli::linkProgram(program);

        auto mvp_uniform = ogli::createUniform(program, "model_view_projection");
        auto mv_uniform = ogli::createUniform(program, "model_view");
        auto normal_matrix_uniform = ogli::createUniform(program, "normal_matrix");
        auto texture_uniform = ogli::createUniform(program, "texture");
        auto lightmap_uniform = ogli::createUniform(program, "lightmap");
        auto material_ambient_color_uniform = ogli::createUniform(program, "material_ambient_color");
        auto material_diffuse_color_uniform = ogli::createUniform(program, "material_diffuse_color");
        auto material_specular_color_uniform = ogli::createUniform(program, "material_specular_color");
        auto material_specular_exponent_uniform = ogli::createUniform(program, "material_specular_exponent");
        auto opacity_uniform = ogli::createUniform(program, "opacity");
        auto light_uniform = ogli::createUniform(program, "light_position");
        auto light_diffuse_color_uniform = ogli::createUniform(program, "light_diffuse_color");
        auto light_specular_color_uniform = ogli::createUniform(program, "light_specular_color");
        auto has_texture = ogli::createUniform(program, "has_texture");
        auto has_lightmap = ogli::createUniform(program, "has_lightmap");

        vertex_programs_.insert(VertexProgramPair(path, VertexProgramData{
            program,
            mvp_uniform,
            mv_uniform,
            normal_matrix_uniform,
            texture_uniform,
            lightmap_uniform,
            material_ambient_color_uniform,
            material_diffuse_color_uniform,
            material_specular_color_uniform,
            material_specular_exponent_uniform,
            opacity_uniform,
            light_uniform,
            light_diffuse_color_uniform,
            light_specular_color_uniform,
            has_texture,
            has_lightmap
        }));
    }

    void Renderer::add_program(const std::string path) {
        add_vertex_program(path, ogli::loadText(path + ".vs"), ogli::loadText(path + ".fs"));
    }

    void Renderer::clear(const glm::vec3 color) {
        ogli::clearDepth(1.0f);
        ogli::clearColor(glm::vec4(color.r, color.g, color.b, 0.0f));
    }

    void Renderer::render(Particles & particles, const glm::mat4 view, const glm::mat4 projection) {
        if (array_buffers_.find(particles.id()) == array_buffers_.end()) {
            array_buffers_.insert(ArrayPair(particles.id(),
                    ogli::createArrayBuffer(particles.end(), particles.end())));
        }
        if (!particles.valid) {
            ogli::updateArrayBuffer(array_buffers_.at(particles.id()),
                    particles.begin(),
                    particles.end());
            particles.valid = true;
        }


        glm::mat4 mv = view;
        glm::mat4 mvp = projection * view;

        ogli::useProgram(particle_programs_.at("particles").program);

        ogli::bindBuffer(array_buffers_.at(particles.id()));

        ogli::uniform(particle_programs_.at("particles").mvp, mvp);
        ogli::uniform(particle_programs_.at("particles").mv, mv);

        ogli::attribute(particle_attributes_.position);
        ogli::attribute(particle_attributes_.color);
        ogli::drawArrays(std::distance(particles.begin(), particles.end()), GL_POINTS);

    }
 
    void Renderer::render(const Model & model,
            const glm::mat4 transform,
            const glm::mat4 view,
            const glm::mat4 projection,
            const float opacity,
            const std::string program_name,
            const Light & light) {


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

        if (model.texture) {
            if (textures_.find(model.texture->id()) == textures_.end()) {
                ogli::TextureBuffer texture = ogli::createTexture(model.texture->begin(), model.texture->end(), model.texture->width(), model.texture->height(), model.texture->mipmaps);
                textures_.insert(std::pair<unsigned int, ogli::TextureBuffer>(model.texture->id(), texture));
            }
        }

        if (model.lightmap) {
            if (textures_.find(model.lightmap->id()) == textures_.end()) {
                ogli::TextureBuffer texture = ogli::createTexture(model.lightmap->begin(), model.lightmap->end(), model.lightmap->width(), model.lightmap->height(), model.lightmap->mipmaps);
                textures_.insert(std::pair<unsigned int, ogli::TextureBuffer>(model.lightmap->id(), texture));
            }
        }

        glm::mat4 mv = view * transform * model.transform;
        glm::mat4 mvp = projection * view * model.transform * transform;

        ogli::useProgram(vertex_programs_.at(program_name).program);

        ogli::bindBuffer(array_buffers_.at(model.mesh->id()));
        ogli::bindBuffer(element_array_buffers_.at(model.mesh->id()));

        if (model.texture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures_.at(model.texture->id()));
            ogli::uniform(vertex_programs_.at(program_name).texture, 0u);
        }

        if (model.lightmap) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, textures_.at(model.lightmap->id()));
            ogli::uniform(vertex_programs_.at(program_name).lightmap, 1u);
        }

        ogli::uniform(vertex_programs_.at(program_name).mvp, mvp);
        ogli::uniform(vertex_programs_.at(program_name).mv, mv);
        glm::mat3 normal_matrix = glm::inverseTranspose(glm::mat3(mv));
        ogli::uniform(vertex_programs_.at(program_name).normal_matrix, normal_matrix);
            
        ogli::uniform(vertex_programs_.at(program_name).material_ambient_color, model.material->ambient);
        ogli::uniform(vertex_programs_.at(program_name).material_diffuse_color, model.material->diffuse);
        ogli::uniform(vertex_programs_.at(program_name).material_specular_color, model.material->specular);
        ogli::uniform(vertex_programs_.at(program_name).material_specular_exponent,
                model.material->specular_exponent);
         ogli::uniform(vertex_programs_.at(program_name).opacity, opacity); 
         
        //Transform light position to eye space.
        ogli::uniform(vertex_programs_.at(program_name).light_position, 
                glm::vec3(view * glm::vec4(light.position.x, light.position.y, light.position.z, 1.0f)));
        ogli::uniform(vertex_programs_.at(program_name).light_diffuse_color,
                light.diffuse_color);
        ogli::uniform(vertex_programs_.at(program_name).light_specular_color,
                light.specular_color);
        
        ogli::uniform(vertex_programs_.at(program_name).has_texture,
                model.texture.get() == nullptr ? false : true);
        ogli::uniform(vertex_programs_.at(program_name).has_lightmap,
                model.lightmap.get() == nullptr ? false : true);

        ogli::attribute(vertex_attributes_.position);
        ogli::attribute(vertex_attributes_.normal);
        ogli::attribute(vertex_attributes_.uv_texture);
        ogli::attribute(vertex_attributes_.uv_lightmap);

        int num_elements = std::distance(model.mesh->elementsBegin(), model.mesh->elementsEnd());
        int draw_type = GL_TRIANGLES;
        if (model.draw == Model::Draw::LINES) {
            draw_type = GL_LINES;
        } else if (model.draw == Model::Draw::POINTS) {
            draw_type = GL_POINTS;
        }

        if (num_elements > 0) {
            ogli::drawElements(num_elements, draw_type);
        } else {
            ogli::drawArrays(std::distance(model.mesh->verticesBegin(), model.mesh->verticesEnd()), draw_type);
        }

    }

}