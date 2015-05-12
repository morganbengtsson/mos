#ifndef MO_ENTITY_HPP
#define MO_ENTITY_HPP

#include <vector>
#include <mo/model.hpp>
#include <mo/streamsource.hpp>
#include <mo/soundsource.hpp>
#include <mo/assets.hpp>
#include <glm/glm.hpp>

namespace mo {

    class Entity {
    public:
        using Models = std::vector<mo::Model>;
        using StreamSources = std::vector<mo::StreamSource>;
        using SoundSources = std::vector<mo::SoundSource>;

        Entity(const std::initializer_list<mo::Model> models = {},
               const std::initializer_list<mo::StreamSource> stream_sources = {},
               const std::initializer_list<mo::SoundSource> sound_sources = {});

        virtual ~Entity();

        virtual void update(const float dt) = 0;

        Models models;
        StreamSources stream_sources;
        SoundSources sound_sources;

        Models::const_iterator begin() const, end() const;

        Models::iterator begin(), end();

        StreamSources::const_iterator stream_sources_begin() const, stream_sources_end() const;

        StreamSources::iterator stream_sources_begin(), stream_sources_end();

        SoundSources::const_iterator sound_sources_begin() const, sound_sources_end() const;

        SoundSources::iterator sound_sources_begin(), sound_sources_end();
    };
}

#endif // MO_ENTITY_HPP
