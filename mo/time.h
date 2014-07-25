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
    static double now_ms() {
        double milliseconds_since_epoch =
                std::chrono::system_clock::now().time_since_epoch() /
                std::chrono::milliseconds(1);
        return milliseconds_since_epoch;
    }

    /* Old implementation (Available on POSIX systems)
    static double now_ms(void) {
        struct timespec res;
        clock_gettime(CLOCK_REALTIME, &res);
        return 1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6;
    }
    */
}

#endif	/* TIME_H */

