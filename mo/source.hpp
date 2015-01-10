/* 
 * File:   Source.h
 * Author: morgan
 *
 * Created on July 28, 2014, 3:47 PM
 */

#ifndef SOURCE_H
#define	SOURCE_H

namespace mo {

/*!
 * Audio source base
 */
class Source {
public:
    Source();
    virtual ~Source();
    unsigned int id() const;
private:
    unsigned int id_;

};

}

#endif	/* SOURCE_H */

