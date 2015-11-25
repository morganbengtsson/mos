#ifndef MOS_RENDERER_H
#define	MOS_RENDERER_H

#include "GL/glew.h"
#include "texture2d.hpp"
#include "model.hpp"
#include "quad.hpp"
#include "text.hpp"
#include "particles.hpp"
#include "light.hpp"
#include "render_target.hpp"
#include "camera.hpp"

#include <unordered_map>
#include <array>

namespace mos {

/**
 * @brief Render geometry.
 * The class that talks to OpenGL, and renders Model objects.
 */
class Renderer {
public:
    /**
     * @brief Renderer constructor.
     * Inits the renderer, in this implementation also creates a
     * valid OpenGL context with GLEW.
     */
    Renderer();
    /**
     * @brief Renderer destructor.
     * Deletes all allocated GPU memory. Textures, Shaders, Buffers.
     */
    virtual ~Renderer();

    /**
     * @brief load a model into renderers own memory.
     * @param model
     */
    void load(const Model & model);

    template<class It>
    /**
     * @brief Load multiple models.
     * @param begin Iterator to first model.
     * @param end Iterator to last model.
     */
    void load(It begin, It end) {
        for (auto it = begin; it != end; it++){
            load(*it);
        }
    }

    /**
     * @brief unload a model from renderers own memory.
     * @param model
     */
    void unload(const Model & model);

    template<class It>
    /**
     * @brief unload multiple models.
     * @param begin Iterator.
     * @param end Iterator.
     */
    void unload(It begin, It end) {
        for (auto it = begin; it != end; it++) {
            unload(*it);
        }
    }


    /**
     * @brief Load a texture into renderer memory.
     * @param texture The texture.
     */
    void load(const std::shared_ptr<Texture2D> & texture);

    /**
     * @brief unload a texture from renderer memory.
     * @param texture The texture.
     */
    void unload(const std::shared_ptr<Texture2D> & texture);

    void update(const Model & model,
                const glm::mat4 & view,
                const glm::mat4 & projection,
                const Light & light = Light());

    template<class It>
    /**
     * @brief Update render state with multiple models
     * @param begin Iterator to fist model.
     * @param end Iterator to last model
     * @param transform Additional transform matrix.
     * @param view View matrix.
     * @param projection Projection matrix.
     * @param program_name Program to use.
     * @param light Dynamic light.
     */
    void update(It begin, It end,
                const glm::mat4 parent_transform,
                const glm::mat4 view,
                const glm::mat4 projection,
                const std::string program_name = "standard",
                const Light & light = Light()){
        for (auto it = begin; it != end; ++it){
            update(*it, parent_transform, view, projection, program_name, light);
        }
    }

    /**
     * @brief Update render state.
     * @param model Model to update.
     * @param camera Camera to render from (view, projection)
     * @param program_name Program to use.
     * @param light Dynamic light.
     */
    void update(const Model & model,
                const Camera & camera,
                const std::string program_name = "standard",
                const Light & light = Light());

    template<class It>
    /**
     * @brief Update render state for multiple models.
     * @param begin Iterator to fist model.
     * @param end Iterator to last model.
     * @param camera Camera to render from.
     * @param program_name Program to use.
     * @param light Dynamic light.
     */
    void update(It begin,
                It end,
                const Camera & camera,
                const std::string program_name = "standard",
                const Light & light = Light()){
        for (auto it = begin; it != end; ++it) {
            update(*it, camera, program_name, light);
        }
    }

    /**
     * @brief Renders particles.
     * @param particles Particles object.
     * @param view View matrix.
     * @param projection Projection matrix.
     */
    void update(Particles & particles,
                const glm::mat4 view,
                const glm::mat4 projection);

    /**
     * @brief Clears the screen and the depth buffer.
     * @param color
     */
    void clear(const glm::vec3 color);

    /**
     * @brief Clear all internal buffers.
     */
    void clear_buffers(){
        for (auto & texture : textures_) {
            glDeleteTextures(1, &texture.second);
        }
        textures_.clear();

        for (auto & ab : array_buffers_) {
            glDeleteBuffers(1, &ab.second);
        }
        array_buffers_.clear();

        for (auto & eab : element_array_buffers_){
            glDeleteBuffers(1, &eab.second);
        }
        element_array_buffers_.clear();
    }
    //TODO: Remove all these from public api!
    /**
     * @brief readFBO
     */
    GLuint readFBO;
    /**
     * @brief drawFBO
     */
    GLuint drawFBO;
    /**
     * @brief readFBO2
     */
    GLuint readFBO2;
    /**
     * @brief drawFBO2
     */
    GLuint drawFBO2;

    /**
     * @brief render_target_reset
     * @param width
     * @param height
     */
    void render_target_reset(unsigned int width, unsigned int height) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFBO);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    /**
     * @brief render_target
     * @param target
     */
    void render_target(RenderTarget target){
        if (frame_buffers_.find(target.id()) == frame_buffers_.end()) {

            glGenFramebuffers(1, &readFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, readFBO);

            GLuint renderedTexture;
            glGenTextures(1, &renderedTexture);

            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderedTexture);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_SRGB8_ALPHA8, target.texture->width(), target.texture->height(), GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, renderedTexture, 0);

            GLuint depthrenderbuffer;
            glGenRenderbuffers(1, &depthrenderbuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, target.texture->width(), target.texture->height());
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

            GLuint screenTexture;
            glGenTextures(1, &screenTexture);
            glBindTexture(GL_TEXTURE_2D, screenTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, target.texture->width(), target.texture->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            glGenFramebuffers(1, &drawFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, drawFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

            // Always check that our framebuffer is ok
            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
                throw std::runtime_error("Framebuffer incomplete.");
            }
            //textures_.insert(TexturePair(target.texture->id(), ogli::TextureBuffer{renderedTexture}));
            textures_.insert({target.texture->id(), screenTexture});
            frame_buffers_.insert({target.id(), readFBO});

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        auto fb = frame_buffers_[target.id()];
        glBindFramebuffer(GL_FRAMEBUFFER, fb);
    }

    /**
     * @brief Set lightmap use.
     */
    void lightmaps(const bool lightmaps);

    /**
     * @brief Check if lightmaps is enabled.
     * @return True if lightmaps are enabled.
     */
    bool lightmaps() const;

    /**
     * @brief Set bounding box rendering.
     */
    void boxes(const bool boxes);

    /**
     * @brief Check If bounding_boxes are shown.
     * @return True if bounding boxes are rendered.
     */
    bool boxes() const;

private:

    /**
     * @brief The ParticleProgramData struct, uniforms for the particle shader program.
     */
    struct ParticleProgramData {
        GLuint program;
        GLint mvp;
        GLint mv;
    };

    /**
     * @brief The BoxProgramData struct, uniforms for the bounding box shader program.
     */
    struct BoxProgramData {
        GLuint program;
        GLint mvp;
        GLint mv;
    };

    /**
     * @brief The VertexProgramData struct, uniforms for the standard shader.
     */
    struct VertexProgramData {
        GLuint program;
        GLint mvp;
        GLint mv;
        GLint normal_matrix;
        GLint texture;
        GLint texture2;
        GLint lightmap;
        GLint normalmap;
        GLint material_ambient_color;
        GLint material_diffuse_color;
        GLint material_specular_color;
        GLint material_specular_exponent;
        GLint opacity;
        GLint light_position;
        GLint light_diffuse_color;
        GLint light_specular_color;
        GLint has_texture;
        GLint has_texture2;
        GLint has_lightmap;
        GLint has_normalmap;
        GLint has_material;
        GLint selected;
        GLint receives_light;
    };

    using VertexProgramPair = std::pair<Model::Shader, VertexProgramData>;
    using ParticleProgramPair = std::pair<std::string, ParticleProgramData>;
    using BoxProgramPair = std::pair<std::string, BoxProgramData>;

    bool lightmaps_;
    bool boxes_;

    /**
     * @brief Updates render state of model.
     * @param model
     * @param transform Additional transform matrix.
     * @param view View matrix of the camera
     * @param projection Projection matrix of the camera
     * @param program_name Either "text" or "standard"
     * @param light One dynamic light to use.
     */
    void update(const Model & model,
                const glm::mat4 transform,
                const glm::mat4 view,
                const glm::mat4 projection,
                const std::string program_name,
                const Light & light = Light());

    void add_vertex_program(const Model::Shader shader,
                            const std::string vertex_shader_source,
                            const std::string fragment_shader_source);

    void add_particle_program(const std::string name,
                              const std::string vs_source,
                              const std::string fs_source);

    unsigned int create_shader(const std::string source, const unsigned int type);
    bool check_shader(const unsigned int shader);
    bool check_program(const unsigned int program);
    unsigned int create_texture(std::shared_ptr<mos::Texture2D> texture);
    unsigned int create_texture_and_pbo(const std::shared_ptr<mos::Texture2D> & texture);
    void add_box_program(const std::string & name,
                         const std::string & vs_source,
                         const std::string & fs_source);

    std::map<Model::Shader, VertexProgramData> vertex_programs_;
    std::unordered_map<std::string, ParticleProgramData> particle_programs_;
    std::unordered_map<std::string, BoxProgramData> box_programs_;

    std::unordered_map<unsigned int, GLuint> frame_buffers_;
    std::unordered_map<unsigned int, GLuint> pixel_buffers_;
    std::unordered_map<unsigned int, GLuint> textures_;
    std::unordered_map<unsigned int, GLuint> array_buffers_;
    std::unordered_map<unsigned int, GLuint> element_array_buffers_;
    std::unordered_map<unsigned int, GLuint> vertex_arrays_;
    unsigned int box_vbo;
    unsigned int box_ebo;
    unsigned int box_va;
};
}
#endif	/* MOS_RENDERER_H */

