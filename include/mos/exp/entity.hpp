#ifndef MO_ENTITY_HPP
#define MO_ENTITY_HPP

#include <vector>
#include <memory>
#include <initializer_list>
#include "../model.hpp"
#include "../soundsource.hpp"
#include "../streamsource.hpp"

namespace mos {
class Entity
{
public:   
    using Models = std::vector<mos::Model>;
    using Sounds = std::vector<mos::SoundSource>;
    using Streams = std::vector<mos::StreamSource>;

    Entity(std::initializer_list<mos::Model> models,
           std::initializer_list<mos::SoundSource> sounds,
           std::initializer_list<mos::StreamSource> streams):
       models(models),
       sounds(sounds),
       streams(streams){
    }
    virtual ~Entity(){}

    Models::const_iterator begin() const{ return models.begin();}
    Models::const_iterator end() const{ return models.end();}

    Sounds::const_iterator sounds_begin() const { return sounds.begin();}
    Sounds::const_iterator sounds_end() const { return sounds.end();}

    Streams::const_iterator streams_begin() const { return streams.begin();}
    Streams::const_iterator streams_end() const { return streams.end();}

    virtual void update(const float dt) = 0;

    Models models;
    Sounds sounds;
    Streams streams;
};
}

#endif // MO_ENTITY_HPP
