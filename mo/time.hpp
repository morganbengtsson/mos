/* 
 * File:   time.h
 * Author: morgan
 *
 * Created on July 25, 2014, 11:51 AM
 */

#ifndef MO_TIME_H
#define	MO_TIME_H

#include <chrono>

namespace mo {

    // New implementation, uses c++11 std. Portability?
    /**
     * /brief Current time
     *
     * @return Current time in milliseconds. 
     *
     * C++ 11 implementation of returning the current time in milliseconds. Might not work on all compilers.
     */
    static double now_ms() {
		auto duration = std::chrono::system_clock::now().time_since_epoch();
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		return (double)millis;
	}

}

#endif	/* MO_TIME_H */

