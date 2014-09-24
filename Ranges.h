/* 
 * File:   Ranges.h
 * Author: morgan
 *
 * Created on September 24, 2014, 4:30 PM
 */

#ifndef RANGES_H
#define	RANGES_H

namespace mo {

    /*!
     * A minimal interface for a container, provides two const iterators.
     */    
    template<typename Container>
    class ConstRange {
    public:
        ConstRange(const Container& container) : container_(container) {
        }

        typename Container::const_iterator begin() const {
            return container_.begin();
        };

        typename Container::const_iterator end() const {
            return container_.end();
        };
    private:
        const Container& container_;
    };
    
     /*!
     * A minimal interface for a container, provides two iterators.
     */       
    template<typename Container>
    class Range {
    public:
        Range(Container& container) : container_(container) {
        }

        typename Container::iterator begin() {
            return container_.begin();
        };

        typename Container::iterator end() {
            return container_.end();
        };
    private:
        Container& container_;
    };


}
#endif	/* RANGES_H */

