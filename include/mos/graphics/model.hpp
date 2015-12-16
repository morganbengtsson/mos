#ifndef MOS_MODEL_H
#define	MOS_MODEL_H

#include <memory>
#include <optional.hpp>
#include "mesh.hpp"
#include "texture2d.hpp"
#include "material.hpp"
#include "box.hpp"

namespace mos {

    /**
     * A class that gathers what is needed to render an object. A Mesh, a Texture2D
     * , a Texture2D lightmap, a Texture2D normalmap and a Material.
     * Also a transformation (position/rotation).
     */
    class Model {
    public:
        /**
         * @brief How to draw the model.
         */
        enum class Draw {TRIANGLES, LINES, POINTS};

        /**
         * @brief What shader to use
         */
        enum class Shader {TEXT, STANDARD, EFFECT};

        using Models = std::vector<Model>;
        using MeshPtr = std::shared_ptr<Mesh>;
        using TexPtr = std::shared_ptr<Texture2D>;
        using MatPtr = std::shared_ptr<Material>;

        /**
         * @brief Model
         */
        Model();

        /**
         * @brief Model constructor.
         * @param mesh
         * @param texture
         * @param transform
         * @param draw
         * @param material
         * @param lightmap
         * @param normalmap
         * @param selectable
         * @param obstruction
         */
        Model(const std::string & name,
              const MeshPtr & mesh,
              const TexPtr & texture = TexPtr(),
              const TexPtr & texture2 = TexPtr(),
              const glm::mat4 & transform = glm::mat4(1.0f),
              const glm::mat4 & parent_transform = glm::mat4(1.0f),
              const Draw draw = Draw::TRIANGLES,
              const MatPtr & material =
                    std::make_shared<Material>(Material(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f))),
              const TexPtr & lightmap = TexPtr(),
              const TexPtr & normalmap = TexPtr(),
              const bool selectable = false,
              const bool step = false,
              const float obstruction = 0.0f,
              const float receives_light = true,
              const Shader shader = Shader::STANDARD);

        /**
         * @brief ~Model destructor
         */
        virtual ~Model();

        /**
         * @brief name
         * @return
         */
        std::string name() const;

        /**
         * @brief mesh
         */
        std::shared_ptr<Mesh> mesh;
        
        /**
         * @brief texture
         */
        std::shared_ptr<Texture2D> texture;

        /**
         * @brief texture2
         */
        std::shared_ptr<Texture2D> texture2;
        
        /**
         * @brief lightmap
         */
        std::shared_ptr<Texture2D> lightmap;
        
        /**
         * @brief normalmap
         */
        std::shared_ptr<Texture2D> normalmap;
        
        /**
         * @brief material
         */
        std::shared_ptr<Material> material;
        
        /**
		 * Get the transform matrix.
		 *
         * @brief transform
         * @return transform mat4
         */
        glm::mat4 transform() const;
        
        /**
		 * Set the transform matrix.
		 *
         * @brief transform
         * @param transform
         */
        void transform(const glm::mat4 & transform);

        /**
		* Get the position
		* @brief position
		*/
        glm::vec3 position() const;

        /**
		* Set the position.
		* @brief position
		* @param position
		*/
        void position(const glm::vec3 position);

        /**
        * @brief Get the color, (to be deprecated).
		*/
        glm::vec4 color() const;

        /**
        * @brief How to draw the model.
		*/
        Draw draw;

        /**
         * @brief What shader to use.
         */
        Shader shader;

        /**
        * @brief Is the model selectable;
		*/
        bool selectable;

        /**
         * @brief If the model is affected by light model (lightmap and dynamic).
         */
        bool receives_light;

        /**
        * @brief Checks if the model is selected.
        * @return True if selected.
		*/
        bool selected() const;

        /**
        * @brief Set if the model is selected.
		* @param selected
		*/
        void selected(const bool selected);

        /**
         * @brief intersects
         * @param model
         * @return
         */
        std::experimental::optional<BoxIntersection> intersects(const Model & model) const;

        void move_box(const glm::vec3 & offset);

        Models::const_iterator begin() const;
        Models::const_iterator end() const;
        Models::iterator begin();
        Models::iterator end();

        /**
         * @brief Children.
         */
        Models models;
    private:
        std::string name_;
        bool selected_;
        glm::mat4 transform_;
    };
}

#endif	/* MO_MODEL_H */

