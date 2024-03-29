#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include "timespan.h"

namespace core {

////////////////////////////////////////////////////////////////
// A timestamp stores a monotonic* time value
// with (theoretical) microseconds resolution.
// Timestamps can be compared with each other
// and simple arithmetic is supported.
//
// The internal reference time is the Unix epoch,
// microseconds midnight, January 1, 1970.
////////////////////////////////////////////////////////////////
class CORE_API timestamp final
{
public:
    // Resolution in units per second. Since the timestamp
    // has microsecond resolution, the value is always 1000000.
    static inline constexpr time_t resolution = 1000000;

public:
    // Creates a timestamp with the current time.
    explicit timestamp(void) : _tsval(0) {update();}

    // Creates a timestamp from Unix epoch(expressed in microseconds).
    explicit timestamp(time_t epoch_time) : _tsval(epoch_time) {}

    // Copy & Move constructors.
    timestamp(const timestamp&) = default;
    timestamp(timestamp&&) = default;

    // Copy & Move assignments.
    timestamp& operator =(const timestamp&) = default;
    timestamp& operator =(timestamp&&) = default;

    // Assigns a Unix epoch(expressed in microseconds).
    timestamp& operator =(const time_t epoch_time) {return assign(epoch_time);}

    // Assigns a Unix epoch(expressed in microseconds).
    timestamp& assign(const time_t epoch_time) {_tsval = epoch_time; return *this;}

    // Updates the timestamp with the current time.
    void update(void);

    // Returns the time elapsed since the time denoted by the timestamp. Equivalent to timestamp() - *this.
    time_t elapsed(void) const {return timestamp() - *this;}

    // Returns true iff the given interval(expressed in microseconds)
    // has passed since the time denoted by the timestamp.
    bool is_elapsed(const time_t interval) const {return elapsed() >= interval;}

    // Returns the timestamp expressed in seconds.
    time_t epoch_time(void) const {return _tsval / resolution;}

    // Returns the timestamp expressed in microseconds.
    time_t epoch_microseconds(void) const {return _tsval;}

    bool operator ==(const timestamp& ts) const {return _tsval == ts._tsval;}
    bool operator !=(const timestamp& ts) const {return _tsval != ts._tsval;}
    bool operator < (const timestamp& ts) const {return _tsval <  ts._tsval;}
    bool operator <=(const timestamp& ts) const {return _tsval <= ts._tsval;}
    bool operator > (const timestamp& ts) const {return _tsval >  ts._tsval;}
    bool operator >=(const timestamp& ts) const {return _tsval >= ts._tsval;}

    timestamp  operator + (const timespan& span) const {return timestamp(_tsval + span.total_microseconds());}
    timestamp  operator - (const timespan& span) const {return timestamp(_tsval - span.total_microseconds());}
    timestamp& operator +=(const timespan& span) {return assign(_tsval + span.total_microseconds());}
    timestamp& operator -=(const timespan& span) {return assign(_tsval - span.total_microseconds());}

    timestamp  operator + (time_t microseconds) const {return timestamp(_tsval + microseconds);}
    timestamp  operator - (time_t microseconds) const {return timestamp(_tsval - microseconds);}
    timestamp& operator +=(time_t microseconds) {return assign(_tsval + microseconds);}
    timestamp& operator -=(time_t microseconds) {return assign(_tsval - microseconds);}

    time_t operator -(const timestamp& ts) const {return _tsval - ts._tsval;}

private:
    time_t _tsval;  // Expressed in microseconds
};

} // namespace core

#endif // __TIMESTAMP_H__