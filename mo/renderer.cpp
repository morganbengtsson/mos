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

#include "renderer.hpp"
#include "vertex.hpp"
#include "mesh.hpp"
#include "texture2d.hpp"
#include "model.hpp"
#include "util.hpp"

namespace mo {

    Renderer::Renderer() {
        GLenum err = glewInit();
        if (GLEW_OK != err) {
            fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        }
        fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

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

        std::string standard_vert_source = text("assets/shaders/standard_330.vert");
        std::string standard_frag_source = text("assets/shaders/standard_330.frag");
        add_vertex_program("standard", standard_vert_source, standard_frag_source);

        std::string text_vert_source = text("assets/shaders/text_330.vert");
        std::string text_frag_source = text("assets/shaders/text_330.frag");
        add_vertex_program("text", text_vert_source, text_frag_source);

        add_vertex_program("effect", text("assets/shaders/effect.vert"),text("assets/shaders/effect.frag"));

        std::string particles_vert_source = text("assets/shaders/particles.vert");
        std::string particles_frag_source = text("assets/shaders/particles.frag");
        add_particle_program("particles", particles_vert_source, particles_frag_source);
    }

    Renderer::~Renderer() {
    }

    void Renderer::add_particle_program(const std::string name, const std::string vs_source, const std::string fs_source) {
        auto vertex_shader = ogli::createShader(vs_source, GL_VERTEX_SHADER);
        auto fragment_shader = ogli::createShader(fs_source, GL_FRAGMENT_SHADER);
        /*
        auto vertex_shader = create_shader(vs_source, GL_VERTEX_SHADER);
        check_shader(vertex_shader);
        auto fragment_shader = create_shader(fs_source, GL_FRAGMENT_SHADER);
        check_shader(fragment_shader);
        */

        auto program = ogli::createProgram();
        //auto program = glCreateProgram();

        /*
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glBindAttribLocation(program, 0, "position");
        glBindAttribLocation(program, 1, "color");
        */
        ogli::attachShader(program, vertex_shader);
        ogli::attachShader(program, fragment_shader);
        ogli::bindAttribute(program, particle_attributes_.position);
        ogli::bindAttribute(program, particle_attributes_.color);
        ogli::linkProgram(program);


        particle_programs_.insert(ParticleProgramPair(name, ParticleProgramData{
                                                          program,
                                                          ogli::createUniform(program, "model_view_projection"),
                                                          ogli::createUniform(program, "model_view")
                                                      }));

        /*
        glLinkProgram(program);
        check_program(program);

        particle_programs_.insert(ParticleProgramPair(name, ParticleProgramData{
                                                          program,
                                                          glGetUniformLocation(program, "model_view_projection"),
                                                          glGetUniformLocation(program, "model_view")
                                                      }));
        */

    }

    void Renderer::add_vertex_program(const std::string path, const std::string vertex_shader_source, const std::string fragment_shader_source) {  
        auto vertex_shader = create_shader(vertex_shader_source, GL_VERTEX_SHADER);
        check_shader(vertex_shader);

        auto fragment_shader = create_shader(fragment_shader_source, GL_FRAGMENT_SHADER);
        check_shader(fragment_shader);

        auto program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);

        glBindAttribLocation(program, 0, "position");
        glBindAttribLocation(program, 1, "normal");
        glBindAttribLocation(program, 2, "uv");
        glBindAttribLocation(program, 3, "uv_lightmap");

        std::cout << "Linking program" << std::endl;
        glLinkProgram(program);
        check_program(program);

        vertex_programs_.insert(VertexProgramPair(path, VertexProgramData{
            program,
            glGetUniformLocation(program, "model_view_projection"),
            glGetUniformLocation(program, "model_view"),
            glGetUniformLocation(program, "normal_matrix"),
            glGetUniformLocation(program, "texture"),
            glGetUniformLocation(program, "lightmap"),
            glGetUniformLocation(program, "normalmap"),
            glGetUniformLocation(program, "material_ambient_color"),
            glGetUniformLocation(program, "material_diffuse_color"),
            glGetUniformLocation(program, "material_specular_color"),
            glGetUniformLocation(program, "material_specular_exponent"),
            glGetUniformLocation(program, "opacity"),
            glGetUniformLocation(program, "light_position"),
            glGetUniformLocation(program, "light_diffuse_color"),
            glGetUniformLocation(program, "light_specular_color"),
            glGetUniformLocation(program, "has_texture"),
            glGetUniformLocation(program, "has_lightmap"),
            glGetUniformLocation(program, "has_normalmap"),
            glGetUniformLocation(program, "selected"),
            glGetUniformLocation(program, "time")
        }));
    }

    void Renderer::add_program(const std::string path) {
        add_vertex_program(path, ogli::loadText(path + ".vs"), ogli::loadText(path + ".fs"));
    }

    void Renderer::clear(const glm::vec3 color) {
        ogli::clearDepth(1.0f);
        ogli::clearColor(glm::vec4(color.r, color.g, color.b, 0.0f));
    }

    unsigned int Renderer::create_shader(const std::string source, const unsigned int type) {
        auto const * chars = source.c_str();
        auto id = glCreateShader(type);

        std::map<unsigned int, std::string> types{{GL_VERTEX_SHADER, "vertex shader"},
                                                  {GL_FRAGMENT_SHADER, "fragment shader"},
                                                  {GL_GEOMETRY_SHADER, "geometry shader"}};

        std::cout << "Compiling " << types[type] << std::endl;
        glShaderSource(id, 1, &chars, NULL);
        glCompileShader(id);
        return id;
    }

    bool Renderer::check_shader(const unsigned int shader) {
        if (!shader){
            return false;
        }
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        std::map<unsigned int, std::string> types{{GL_VERTEX_SHADER, "vertex shader"},
                                                  {GL_FRAGMENT_SHADER, "fragment shader"},
                                                  {GL_GEOMETRY_SHADER, "geometry shader"}};
        GLint type;
        glGetShaderiv(shader, GL_SHADER_TYPE, &type);

        if (status == GL_FALSE) {
            int length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            if(length > 0) {
                std::vector<char> buffer(length);
                glGetShaderInfoLog(shader, length, NULL, &buffer[0]);
                std::cerr << "Compile failure in " << types[type] << " shader" << std::endl;
                std::cerr << std::string(buffer.begin(), buffer.end()) << std::endl;
            }
            return false;
        }
        return true;
    }

    bool Renderer::check_program(const unsigned int program) {
        if (!program){
            return false;
        }

        GLint status;
        glGetShaderiv(program, GL_LINK_STATUS, &status);

        if (status == GL_FALSE) {
            int length;
            glGetShaderiv(program, GL_INFO_LOG_LENGTH, &length);
            if(length > 0) {
                std::vector<char> buffer(length);
                glGetShaderInfoLog(program, length, NULL, &buffer[0]);
                std::cerr << "Link failure in program" << std::endl;
                std::cerr << std::string(buffer.begin(), buffer.end()) << std::endl;
            }
            return false;
        }
        return true;
    }

    void Renderer::update(Particles & particles, const glm::mat4 view, const glm::mat4 projection) {


        if (array_buffers_.find(particles.id()) == array_buffers_.end()) {
            array_buffers_.insert(ArrayPair(particles.id(),
                    ogli::createArrayBuffer(particles.end(), particles.end())));
        }
        /*
        if(vertex_arrays_.find(particles.id()) == vertex_arrays_.end()) {
            unsigned int vertex_array;
            glGenVertexArrays(1, &vertex_array);
            glBindVertexArray(vertex_array);
                if (array_buffers2_.find(particles.id()) == array_buffers2_.end()) {
                    unsigned int array_buffer;
                    glGenBuffers(1, &array_buffer);
                    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
                    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof (Particle),
                                 particles.data(),
                                 GL_STATIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    array_buffers2_.insert({particles.id(), array_buffer});
                }
                glBindBuffer(GL_ARRAY_BUFFER, array_buffers2_.at(particles.id()));
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
                    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle),
                                          reinterpret_cast<const void *>(sizeof(glm::vec3)));

                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glEnableVertexAttribArray(0);
                    glEnableVertexAttribArray(1);
            glBindVertexArray(0);
            vertex_arrays_.insert({particles.id(), vertex_array});
        }
        */
        if (!particles.valid) {
            ogli::updateArrayBuffer(array_buffers_.at(particles.id()),
                    particles.begin(),
                    particles.end());


            /*
            glBindBuffer(GL_ARRAY_BUFFER, array_buffers2_[particles.id()]);
            glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof (Particle),
                         particles.data(),
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            */
            particles.valid = true;
        }


        glm::mat4 mv = view;
        glm::mat4 mvp = projection * view;

        //auto & uniforms = particle_programs_.at("particles");

        ogli::useProgram(particle_programs_.at("particles").program);
        //glUseProgram(uniforms.program);

        //glBindVertexArray(vertex_arrays_[particles.id()]);
        ogli::bindBuffer(array_buffers_.at(particles.id()));


        //glUniformMatrix4fv(uniforms.program, 1, GL_FALSE, &mvp[0][0]);
        //glUniformMatrix4fv(uniforms.program, 1, GL_FALSE, &mv[0][0]);
        ogli::uniform(particle_programs_.at("particles").mvp, mvp);
        ogli::uniform(particle_programs_.at("particles").mv, mv);

        ogli::attribute(particle_attributes_.position);
        ogli::attribute(particle_attributes_.color);
        ogli::drawArrays(particles.size(), GL_POINTS);

    }
 
    void Renderer::update(const Model & model,
                          const glm::mat4 transform,
                          const glm::mat4 view,
                          const glm::mat4 projection,
                          const float opacity,
                          const std::string program_name,
                          const Light & light,
                          const float time) {


        if(vertex_arrays_.find(model.mesh->id()) == vertex_arrays_.end()) {
            unsigned int vertex_array;
            glGenVertexArrays(1, &vertex_array);
            glBindVertexArray(vertex_array);
                if (array_buffers2_.find(model.mesh->id()) == array_buffers2_.end()) {
                    unsigned int array_buffer;
                    glGenBuffers(1, &array_buffer);
                    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
                    glBufferData(GL_ARRAY_BUFFER, model.mesh->vertices_size() * sizeof (Vertex),
                                 model.mesh->vertices_data(),
                                 GL_STATIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    array_buffers2_.insert({model.mesh->id(), array_buffer});
                }
                if (element_array_buffers2_.find(model.mesh->id()) == element_array_buffers2_.end()) {
                    unsigned int element_array_buffer;
                    glGenBuffers(1, &element_array_buffer);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                 model.mesh->elements_size() * sizeof (unsigned int),
                                 model.mesh->elements_data(),
                                 GL_STATIC_DRAW);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                    element_array_buffers2_.insert({model.mesh->id(), element_array_buffer});
                }
                glBindBuffer(GL_ARRAY_BUFFER, array_buffers2_.at(model.mesh->id()));
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                          reinterpret_cast<const void *>(sizeof(glm::vec3)));
                    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                          reinterpret_cast<const void *>(sizeof(glm::vec3)*2));
                    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                          reinterpret_cast<const void *>(sizeof(glm::vec3)*2 + sizeof(glm::vec2)));
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                                 element_array_buffers2_.at(model.mesh->id()));
                    glEnableVertexAttribArray(0);
                    glEnableVertexAttribArray(1);
                    glEnableVertexAttribArray(2);
                    glEnableVertexAttribArray(3);
            glBindVertexArray(0);
            vertex_arrays_.insert({model.mesh->id(), vertex_array});
        }

        if (!model.mesh->valid) {       
            glBindBuffer(GL_ARRAY_BUFFER, array_buffers2_[model.mesh->id()]);
            glBufferData(GL_ARRAY_BUFFER, model.mesh->vertices_size() * sizeof (Vertex),
                         model.mesh->vertices_data(),
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            /*
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffers2_[model.mesh->id()]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         model.mesh->elements_size() * sizeof (unsigned int),
                         model.mesh->elements_data(),
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            */
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

        if (model.normalmap){
            if (textures_.find(model.normalmap->id()) == textures_.end()) {
                ogli::TextureBuffer texture = ogli::createTexture(model.normalmap->begin(), model.normalmap->end(),
                                                                  model.normalmap->width(), model.normalmap->height(),
                                                                  model.normalmap->mipmaps);
                textures_.insert(std::pair<unsigned int, ogli::TextureBuffer>(model.normalmap->id(), texture));
            }
        }

        auto t = model.transform();
        glm::mat4 mv = view * transform * t;
        glm::mat4 mvp = projection * view * t * transform;

        //ogli::useProgram(vertex_programs_.at(program_name).program);
        glUseProgram(vertex_programs_[program_name].program);

        glBindVertexArray(vertex_arrays_.at(model.mesh->id()));

        auto & uniforms = vertex_programs_.at(program_name);

        if (model.texture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures_.at(model.texture->id()));
            glUniform1i(uniforms.texture, 0u);
        }

        if (model.lightmap) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, textures_.at(model.lightmap->id()));
            glUniform1i(uniforms.lightmap, 1u);
        }

        if (model.normalmap) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, textures_.at(model.normalmap->id()));
            glUniform1i(uniforms.normalmap, 1u);
        }

        glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(uniforms.mv, 1, GL_FALSE, &mv[0][0]);

        glm::mat3 normal_matrix = glm::inverseTranspose(glm::mat3(mv));
        glUniformMatrix3fv(uniforms.normal_matrix,1 , GL_FALSE, &normal_matrix[0][0]);

        glUniform3fv(uniforms.material_ambient_color,1 , glm::value_ptr(model.material->ambient));
        glUniform3fv(uniforms.material_diffuse_color,1 , glm::value_ptr(model.material->diffuse));
        glUniform3fv(uniforms.material_specular_color,1, glm::value_ptr(model.material->specular));
        glUniform1fv(uniforms.material_specular_exponent, 1, &model.material->specular_exponent);
        glUniform1fv(uniforms.opacity, 1, &opacity);

        //Transform light position to eye space.
        glUniform3fv(uniforms.light_position,1 , glm::value_ptr(glm::vec3(view * glm::vec4(light.position.x, light.position.y, light.position.z, 1.0f))));
        glUniform3fv(uniforms.light_diffuse_color,1 ,glm::value_ptr(light.diffuse_color));
        glUniform3fv(uniforms.light_specular_color,1 , glm::value_ptr(light.specular_color));

        glUniform1i(uniforms.has_texture, model.texture.get() == nullptr ? false : true);
        glUniform1i(uniforms.has_lightmap, model.lightmap.get() == nullptr ? false : true);
        glUniform1i(uniforms.has_normalmap, model.lightmap.get() == nullptr ? false : true);

        glUniform1i(uniforms.selected, model.selected());
        glUniform1f(uniforms.time, time);

        int num_elements = std::distance(model.mesh->elements_begin(), model.mesh->elements_end());
        int draw_type = GL_TRIANGLES;
        if (model.draw == Model::Draw::LINES) {
            draw_type = GL_LINES;
        } else if (model.draw == Model::Draw::POINTS) {
            draw_type = GL_POINTS;
        }
        if (num_elements > 0) {
            ogli::drawElements(num_elements, draw_type);
        } else {
            ogli::drawArrays(std::distance(model.mesh->vertices_begin(), model.mesh->vertices_end()), draw_type);
        }

    }
}
