/* 
 * File:   Range.h
 * Author: morgan
 *
 * Created on October 16, 2014, 5:08 PM
 */

#ifndef MO_RANGE_H
#define	MO_RANGE_H

namespace mo {

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
#endif	/* MO_RANGE_H */

