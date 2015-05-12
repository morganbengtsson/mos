#include "entity.hpp"

namespace mo {

    Entity::Entity(const std::initializer_list<mo::Model> models,
                   const std::initializer_list<mo::StreamSource> stream_sources,
                   const std::initializer_list<mo::SoundSource> sound_sources) :
            models(models), stream_sources(stream_sources), sound_sources(sound_sources) {

    }

    Entity::~Entity() {
    }

    Entity::Models::const_iterator Entity::begin() const {
        return models.begin();
    }

    Entity::Models::const_iterator Entity::end() const {
        return models.end();
    }

    Entity::Models::iterator Entity::begin() {
        return models.begin();
    }

    Entity::Models::iterator Entity::end() {
        return models.end();
    }

    Entity::StreamSources::const_iterator Entity::stream_sources_begin() const {
        return stream_sources.begin();
    }

    Entity::StreamSources::const_iterator Entity::stream_sources_end() const {
        return stream_sources.end();
    }

    Entity::StreamSources::iterator Entity::stream_sources_begin() {
        return stream_sources.begin();
    }

    Entity::StreamSources::iterator Entity::stream_sources_end() {
        return stream_sources.end();
    }

    Entity::SoundSources::const_iterator Entity::sound_sources_begin() const {
        return sound_sources.begin();
    }

    Entity::SoundSources::const_iterator Entity::sound_sources_end() const {
        return sound_sources.end();
    }

    Entity::SoundSources::iterator Entity::sound_sources_begin() {
        return sound_sources.begin();
    }

    Entity::SoundSources::iterator Entity::sound_sources_end() {
        return sound_sources.end();
    }

}