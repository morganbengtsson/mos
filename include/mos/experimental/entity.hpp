#ifndef MO_ENTITY_HPP
#define MO_ENTITY_HPP

#include <vector>
#include <memory>
#include <initializer_list>
#include "../graphics/model.hpp"
#include "../audio/soundsource.hpp"
#include "../audio/streamsource.hpp"

namespace mos {
/**
 * @brief The Entity base class
 *
 * A base entity to inherit from, for convenience. May be deprecated in the
 *future though.
 */
class Entity {
public:
  /**
   * @brief Models container.
   */
  using Models = std::vector<mos::Model>;
  /**
   * @brief Sounds container.
   */
  using Sounds = std::vector<mos::SoundSource>;
  /**
   * @brief Streams container.
   */
  using Streams = std::vector<mos::StreamSource>;
  /**
   * @brief Entity constructor.
   * @param models Models for the entity.
   * @param sounds Sounds for the entity.
   * @param streams SoundStreams for the entity.
   */
  Entity(std::initializer_list<mos::Model> models,
         std::initializer_list<mos::SoundSource> sounds,
         std::initializer_list<mos::StreamSource> streams)
      : models(models), sounds(sounds), streams(streams) {}
  /**
   * @brief ~Entity destructor.
   */
  virtual ~Entity() {}

  /**
   * @brief begin
   * @return Iterator to first model.
   */
  Models::const_iterator begin() const { return models.begin(); }
  /**
   * @brief end
   * @return Iterator to last model
   */
  Models::const_iterator end() const { return models.end(); }

  /**
   * @brief sounds_begin
   * @return Iterator to fist sound.
   */
  Sounds::const_iterator sounds_begin() const { return sounds.begin(); }
  /**
   * @brief sounds_end
   * @return Iterator to last sound.
   */
  Sounds::const_iterator sounds_end() const { return sounds.end(); }

  /**
   * @brief streams_begin
   * @return Iterator to first StreamSource.
   */
  Streams::const_iterator streams_begin() const { return streams.begin(); }
  /**
   * @brief streams_end
   * @return Iterator to last StreamSource.
   */
  Streams::const_iterator streams_end() const { return streams.end(); }

  /**
   * @brief Update the entity.
   * @param dt Time step.
   */
  virtual void update(const float dt) = 0;

  /**
   * @brief models
   */
  Models models;

  /**
   * @brief sounds
   */
  Sounds sounds;

  /**
   * @brief streams
   */
  Streams streams;
};
}

#endif // MO_ENTITY_HPP
