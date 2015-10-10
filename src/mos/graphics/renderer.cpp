/* 
 * File:   Renderer.cpp
 * Author: morgan
 *
 * Created on February 15, 2014, 2:37 PM
 */

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/io.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <memory>

#include "graphics/renderer.hpp"
#include "graphics/vertex.hpp"
#include "graphics/mesh.hpp"
#include "graphics/texture2d.hpp"
#include "graphics/model.hpp"
#include "util.hpp"

namespace mos {

Renderer::Renderer() :
    lightmaps_(true),
    boxes_(false) {
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
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_FRAMEBUFFER_SRGB);

    std::string standard_vert_source = text("assets/shaders/standard_330.vert");
    std::string standard_frag_source = text("assets/shaders/standard_330.frag");
    add_vertex_program("standard", standard_vert_source, standard_frag_source);

    std::string text_vert_source = text("assets/shaders/text_330.vert");
    std::string text_frag_source = text("assets/shaders/text_330.frag");
    add_vertex_program("text", text_vert_source, text_frag_source);

    add_vertex_program("effect", text("assets/shaders/effect_330.vert"),
                       text("assets/shaders/effect_330.frag"));

    std::string particles_vert_source = text("assets/shaders/particles_330.vert");
    std::string particles_frag_source = text("assets/shaders/particles_330.frag");
    add_particle_program("particles", particles_vert_source, particles_frag_source);

    std::string box_vert_source = text("assets/shaders/box_330.vert");
    std::string box_frag_source = text("assets/shaders/box_330.frag");
    add_box_program("box", box_vert_source, box_frag_source);


    //Render boxes
    float vertices[] = {
       -0.5, -0.5, -0.5, 1.0,
        0.5, -0.5, -0.5, 1.0,
        0.5,  0.5, -0.5, 1.0,
       -0.5,  0.5, -0.5, 1.0,
       -0.5, -0.5,  0.5, 1.0,
        0.5, -0.5,  0.5, 1.0,
        0.5,  0.5,  0.5, 1.0,
       -0.5,  0.5,  0.5, 1.0,
     };

     glGenBuffers(1, &box_vbo);
     glBindBuffer(GL_ARRAY_BUFFER, box_vbo);
     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
     glBindBuffer(GL_ARRAY_BUFFER, 0);

     unsigned int elements[] = {
       0, 1, 2, 3,
       4, 5, 6, 7,
       0, 4, 1, 5, 2, 6, 3, 7
     };

     glGenBuffers(1, &box_ebo);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box_ebo);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

     glGenVertexArrays(1, &box_va);
     glBindVertexArray(box_va);
        glBindBuffer(GL_ARRAY_BUFFER, box_vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
         0,  // attribute
         4,                  // number of elements per vertex, here (x,y,z,w)
         GL_FLOAT,           // the type of each element
         GL_FALSE,           // take our values as-is
         0,                  // no extra data between each position
         0                   // offset of first element
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box_ebo);
    glBindVertexArray(0);

}

Renderer::~Renderer() {
    //TODO: Delete all buffers, textures and shaders.
}

void Renderer::add_box_program(const std::string &name, const std::string &vs_source, const std::string &fs_source){
    auto vertex_shader = create_shader(vs_source, GL_VERTEX_SHADER);
    check_shader(vertex_shader);
    auto fragment_shader = create_shader(fs_source, GL_FRAGMENT_SHADER);
    check_shader(fragment_shader);

    auto program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glBindAttribLocation(program, 0, "position");
    glLinkProgram(program);
    check_program(program);

    box_programs_.insert({name, BoxProgramData{                                                  program,
                                                  glGetUniformLocation(program, "model_view_projection"),
                                                  glGetUniformLocation(program, "model_view")
                                                  }});
}

void Renderer::add_particle_program(const std::string name, const std::string vs_source, const std::string fs_source) {
    auto vertex_shader = create_shader(vs_source, GL_VERTEX_SHADER);
    check_shader(vertex_shader);
    auto fragment_shader = create_shader(fs_source, GL_FRAGMENT_SHADER);
    check_shader(fragment_shader);

    auto program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glBindAttribLocation(program, 0, "position");
    glBindAttribLocation(program, 1, "color");

    glLinkProgram(program);
    check_program(program);

    particle_programs_.insert(ParticleProgramPair(name, ParticleProgramData{
                                                      program,
                                                      glGetUniformLocation(program, "model_view_projection"),
                                                      glGetUniformLocation(program, "model_view")
                                                  }));


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
                                                  glGetUniformLocation(program, "texture2"),
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
                                                  glGetUniformLocation(program, "has_texture2"),
                                                  glGetUniformLocation(program, "has_lightmap"),
                                                  glGetUniformLocation(program, "has_normalmap"),
                                                  glGetUniformLocation(program, "has_material"),
                                                  glGetUniformLocation(program, "selected"),
                                                  glGetUniformLocation(program, "receives_light")
                                              }));
}

void Renderer::init(const Model & model) {
    if(vertex_arrays_.find(model.mesh->id()) == vertex_arrays_.end()) {
        unsigned int vertex_array;
        glGenVertexArrays(1, &vertex_array);
        glBindVertexArray(vertex_array);
        if (array_buffers_.find(model.mesh->id()) == array_buffers_.end()) {
            unsigned int array_buffer;
            glGenBuffers(1, &array_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
            glBufferData(GL_ARRAY_BUFFER, model.mesh->vertices_size() * sizeof (Vertex),
                         model.mesh->vertices_data(),
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            array_buffers_.insert({model.mesh->id(), array_buffer});
        }
        if (element_array_buffers_.find(model.mesh->id()) == element_array_buffers_.end()) {
            unsigned int element_array_buffer;
            glGenBuffers(1, &element_array_buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         model.mesh->elements_size() * sizeof (unsigned int),
                         model.mesh->elements_data(),
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            element_array_buffers_.insert({model.mesh->id(), element_array_buffer});
        }
        glBindBuffer(GL_ARRAY_BUFFER, array_buffers_.at(model.mesh->id()));
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<const void *>(sizeof(glm::vec3)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<const void *>(sizeof(glm::vec3)*2));
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<const void *>(sizeof(glm::vec3)*2 + sizeof(glm::vec2)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                     element_array_buffers_.at(model.mesh->id()));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glBindVertexArray(0);
        vertex_arrays_.insert({model.mesh->id(), vertex_array});
    }

    if (!model.mesh->valid) {
        glBindBuffer(GL_ARRAY_BUFFER, array_buffers_[model.mesh->id()]);
        glBufferData(GL_ARRAY_BUFFER, model.mesh->vertices_size() * sizeof (Vertex),
                     model.mesh->vertices_data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        /*
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffers_[model.mesh->id()]);
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
            GLuint id = create_texture_and_pbo(model.texture);
            textures_.insert({model.texture->id(), id});
        }
    }

    if (model.texture2){
        if (textures_.find(model.texture2->id()) == textures_.end()) {
            GLuint id = create_texture_and_pbo(model.texture2);
            textures_.insert({model.texture2->id(), id});
        }
    }

    if (model.lightmap) {
        if (textures_.find(model.lightmap->id()) == textures_.end()) {
            auto id = create_texture(model.lightmap);
            textures_.insert({model.lightmap->id(), id});
        }
    }

    if (model.normalmap) {
        if (textures_.find(model.normalmap->id()) == textures_.end()) {
            auto id = create_texture(model.normalmap);
            textures_.insert({model.normalmap->id(), id});
        }
    }
}

void Renderer::init(std::shared_ptr<Texture2D> texture){
    if (textures_.find(texture->id()) == textures_.end()) {
        GLuint id = create_texture(texture);
        textures_.insert({texture->id(), id});
    }
}

void Renderer::update(const Model &model,
                      const glm::mat4 & view,
                      const glm::mat4 & projection,
                      const Light & light) {
    update(model, glm::mat4(1.0f), view, projection, "standard", light);
}

void Renderer::clear(const glm::vec3 color) {
    glClearDepthf(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(color.r, color.g, color.b, 1.0f);
}

void Renderer::lightmaps(const bool lightmaps){
    lightmaps_ = lightmaps;
}

bool Renderer::lightmaps() const {
    return lightmaps_;
}

void Renderer::boxes(const bool bounding_boxes) {
    boxes_ = bounding_boxes;
}

bool Renderer::boxes() const {
    return boxes_;
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

unsigned int Renderer::create_texture(std::shared_ptr<Texture2D> texture){
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    GLfloat sampling = texture->mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampling);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampling);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texture->width(), texture->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data());

    if (texture->mipmaps) {glGenerateMipmap(GL_TEXTURE_2D);};
    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}

unsigned int Renderer::create_texture_and_pbo(const std::shared_ptr<Texture2D> & texture) {
    GLuint pbo_id;
    glGenBuffers(1, &pbo_id);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_id);


    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    GLfloat sampling = texture->mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampling);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampling);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (texture->mipmaps) {glGenerateMipmap(GL_TEXTURE_2D);};
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texture->width(), texture->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data());

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture->width(), texture->height(),
                    GL_SRGB8_ALPHA8, GL_UNSIGNED_BYTE, 0);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, sizeof(unsigned char) * texture->size(), 0, GL_STREAM_DRAW);

    GLubyte * ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    ptr = (GLubyte*)texture->data();

    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

}


void Renderer::update(Particles & particles, const glm::mat4 view, const glm::mat4 projection) {
    if(vertex_arrays_.find(particles.id()) == vertex_arrays_.end()) {
        unsigned int vertex_array;
        glGenVertexArrays(1, &vertex_array);
        glBindVertexArray(vertex_array);
        if (array_buffers_.find(particles.id()) == array_buffers_.end()) {
            unsigned int array_buffer;
            glGenBuffers(1, &array_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
            glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof (Particle),
                         particles.data(),
                         GL_STREAM_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            array_buffers_.insert({particles.id(), array_buffer});
        }
        glBindBuffer(GL_ARRAY_BUFFER, array_buffers_[particles.id()]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle),
                              reinterpret_cast<const void *>(sizeof(glm::vec3)));
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle),
                              reinterpret_cast<const void *>(sizeof(glm::vec3) + sizeof(glm::vec4)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);
        vertex_arrays_.insert({particles.id(), vertex_array});
    }

    if (!particles.valid) {
        glBindBuffer(GL_ARRAY_BUFFER, array_buffers_[particles.id()]);
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof (Particle),
                     particles.data(),
                     GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        particles.valid = true;
    }

    glm::mat4 mv = view;
    glm::mat4 mvp = projection * view;

    auto & uniforms = particle_programs_.at("particles");

    glUseProgram(uniforms.program);

    glBindVertexArray(vertex_arrays_[particles.id()]);

    glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(uniforms.mv, 1, GL_FALSE, &mv[0][0]);

    glDrawArrays(GL_POINTS, 0, particles.size());
}

void Renderer::update(const Model & model,
                      const glm::mat4 parent_transform,
                      const glm::mat4 view,
                      const glm::mat4 projection,
                      const std::string program_name,
                      const Light & light) {
    init(model);

    auto transform = model.transform();
    glm::mat4 mv = view * parent_transform * transform;
    glm::mat4 mvp = projection * view  * parent_transform * transform;

    glUseProgram(vertex_programs_[program_name].program);

    glBindVertexArray(vertex_arrays_.at(model.mesh->id()));

    auto & uniforms = vertex_programs_.at(program_name);

    int texture_unit = 0;
    if (model.texture != nullptr) {
        glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
        glBindTexture(GL_TEXTURE_2D, textures_[model.texture->id()]);
        glUniform1i(uniforms.texture, texture_unit);
        texture_unit ++;
    }

    if (model.texture2 != nullptr) {
        glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
        glBindTexture(GL_TEXTURE_2D, textures_[model.texture2->id()]);
        glUniform1i(uniforms.texture2, texture_unit);
        texture_unit ++;
    }

    if (model.lightmap) {
        glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
        glBindTexture(GL_TEXTURE_2D, textures_[model.lightmap->id()]);
        glUniform1i(uniforms.lightmap, texture_unit);
        texture_unit ++;
    }

    if (model.normalmap) {
        glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
        glBindTexture(GL_TEXTURE_2D, textures_[model.normalmap->id()]);
        glUniform1i(uniforms.normalmap, texture_unit);
        texture_unit ++;
    }    

    glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(uniforms.mv, 1, GL_FALSE, &mv[0][0]);

    glm::mat3 normal_matrix = glm::inverseTranspose(glm::mat3(mv));
    glUniformMatrix3fv(uniforms.normal_matrix,1 , GL_FALSE, &normal_matrix[0][0]);

    if (model.material != nullptr) {
        glUniform3fv(uniforms.material_ambient_color,1 , glm::value_ptr(model.material->ambient));
        glUniform3fv(uniforms.material_diffuse_color,1 , glm::value_ptr(model.material->diffuse));
        glUniform3fv(uniforms.material_specular_color,1, glm::value_ptr(model.material->specular));
        glUniform1fv(uniforms.material_specular_exponent, 1, &model.material->specular_exponent);
    }
    glUniform1fv(uniforms.opacity, 1, &model.opacity);

    //Transform light position to eye space.
    glUniform3fv(uniforms.light_position,1 , glm::value_ptr(glm::vec3(view * glm::vec4(light.position.x, light.position.y, light.position.z, 1.0f))));
    glUniform3fv(uniforms.light_diffuse_color,1 ,glm::value_ptr(light.diffuse_color));
    glUniform3fv(uniforms.light_specular_color,1 , glm::value_ptr(light.specular_color));

    glUniform1i(uniforms.has_texture, !(model.texture == nullptr));
    glUniform1i(uniforms.has_texture2, model.texture2 == nullptr ? false: true);
    glUniform1i(uniforms.has_lightmap, model.lightmap == nullptr ? false : lightmaps_ ? true : false);
    glUniform1i(uniforms.has_normalmap, model.lightmap == nullptr ? false : true);
    glUniform1i(uniforms.has_material, model.material == nullptr ? false : true);

    glUniform1i(uniforms.selected, model.selected());
    glUniform1i(uniforms.receives_light, model.receives_light);

    int num_elements = std::distance(model.mesh->elements_begin(), model.mesh->elements_end());
    int draw_type = GL_TRIANGLES;
    if (model.draw == Model::Draw::LINES) {
        draw_type = GL_LINES;
    } else if (model.draw == Model::Draw::POINTS) {
        draw_type = GL_POINTS;
    }
    if (num_elements > 0) {
        glDrawElements(draw_type, num_elements, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(draw_type, 0, model.mesh->vertices_size());
    }
    if (boxes_ == true){
        auto & uniforms = box_programs_.at("box");

        glUseProgram(uniforms.program);

        glBindVertexArray(box_va);

        glm::vec3 size = model.box.size();

        glm::mat4 mv = view * model.box.transform() *  glm::scale(glm::mat4(1.0f), size);
        glm::mat4 mvp = projection * view * model.box.transform() * glm::scale(glm::mat4(1.0f), size);

        glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(uniforms.mv, 1, GL_FALSE, &mv[0][0]);

        //glDrawArrays(GL_POINTS, 0, 16);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid*)(4*sizeof(GLuint)));
        glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (GLvoid*)(8*sizeof(GLuint)));
    }

    for (auto & child : model.models) {
        update(child, parent_transform * model.transform(), view, projection, program_name, light);
    }
}

void Renderer::update(const Model & model,
                      const Camera & camera,
                      const std::string program_name,
                      const Light & light) {
    update(model, glm::mat4(1.0f), camera.view, camera.projection, program_name, light);
}

void Renderer::update(const Quad & quad,
                      const glm::mat4 view,
                      const glm::mat4 projection) {
    update(quad, glm::mat4(1.0f), view, projection, "effect", Light());
}

void Renderer::update(const Quad & quad,
                      const Camera & camera) {
    update(quad, glm::mat4(1.0f), camera.view, camera.projection, "effect", Light());
}

void Renderer::update(const Text & text,
                      const glm::mat4 view,
                      const glm::mat4 projection) {
    update(text, glm::mat4(1.0f), view, projection, "text", Light());

}
}
