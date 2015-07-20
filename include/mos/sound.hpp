/* 
 * File:   Sound.h
 * Author: morgan
 *
 * Created on May 6, 2014, 9:38 PM
 */

#ifndef MO_SOUND_H
#define	MO_SOUND_H

#include <vector>

namespace mos {

    /*!
     * A sound class containing samples of 16bit integer data. In memory.
     */
    class Sound {
    public:
        /**
         * @breif Container for 16bit samples (short).
         */
        using Samples = std::vector<short>;
        template<class It>

        /*!
         * \brief Sound constructor
         * \param begin iterator
         * \param end iterator
         * \param number of channels
         *
         * Constructor that takes iterators from a container of shorts.
         */
        Sound(It begin, It end, const unsigned int channels = 1u) :
            channels_(channels),
            valid(true),
            samples_(begin, end) {
            static unsigned int current_id = 0;
            id_ = current_id++;
        }

        /*!
         * \brief Sound constructor empty
         * \param channels
         */
        Sound(const unsigned int channels = 1u): channels_(channels){
            static unsigned int current_id = 0;
            id_ = current_id++;
        }

        virtual ~Sound();
        
        /*!
         * \brief begin iterator for samples
         * \return
         */
        Samples::const_iterator begin() const {
            return samples_.begin();
        }

        /*!
         * \brief end iterator for samples
         * \return
         */
        Samples::const_iterator end() const {
            return samples_.end();
        }
        
        /*!
         * \brief Raw data
         * \return array of shorts
         */
        const short * data() const;

        /*!
         * \brief unique id
         * \return id
         */
        unsigned int id() const;

        /*!
         * \brief valid
         *
         * A sound becomes invalid, if its data has changed. For subsystems to know, when to update.
         */
        bool valid;

        /*!
         * \brief Number of channels
         * \return channels
         */
        unsigned int channels() const;

    private:
        unsigned int id_;
        Samples samples_;
        unsigned int channels_;


    };
}

#endif	/* MO_SOUND_H */

