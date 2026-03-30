#ifndef __TIMESPAN_H__
#define __TIMESPAN_H__

#include <chrono>

namespace core    {
namespace details {

inline constexpr time_t milliseconds = 1000;                 // The number of microseconds in a millisecond.
inline constexpr time_t seconds      = 1000 * milliseconds;  // The number of microseconds in a second.
inline constexpr time_t minutes      =   60 * seconds;       // The number of microseconds in a minute.
inline constexpr time_t hours        =   60 * minutes;       // The number of microseconds in a hour.
inline constexpr time_t days         =   24 * hours;         // The number of microseconds in a day.

} // namespace details

////////////////////////////////////////////////////////////////
// A class that represents time spans up to
// microsecond resolution.
////////////////////////////////////////////////////////////////
class timespan final
{
public:
    static_assert(sizeof(time_t) == 8 && std::is_signed_v<time_t>, "Invalid time_t!");

public:
    // Creates a zero timespan.
    constexpr timespan(void) : _span(0) {}

    // Creates a timespan.
    constexpr timespan(time_t microseconds) : _span(microseconds) {}

    // Creates a timespan.
    constexpr timespan(int days, int hours, int minutes, int seconds, int microseconds)
        : _span(0)
    {
        assign(days, hours, minutes, seconds, microseconds);
    }

    // Creates a timespan from std::chrono::duration
    template <class T, class Period>
    constexpr timespan(const std::chrono::duration<T, Period>& dtime)
        : _span(static_cast<time_t>(std::chrono::duration_cast<std::chrono::microseconds>(dtime).count()))
    {
    }

    // Copy & Move constructors.
    timespan(const timespan&) = default;
    timespan(timespan&&) = default;

    // Copy & Move assignments.
    timespan& operator =(const timespan&) = default;
    timespan& operator =(timespan&&) = default;

    // Assigns a new timespan.
    timespan& operator =(time_t microseconds) {_span = microseconds; return *this;}

    // Assigns a new timespan.
    constexpr timespan& assign(int days, int hours, int minutes, int seconds, int microseconds)
    {
        _span  = 0;
        _span += microseconds;
        _span += details::seconds * seconds;
        _span += details::minutes * minutes;
        _span += details::hours   * hours;
        _span += details::days    * days;

        return *this;
    }

    constexpr bool operator ==(const timespan& ts) const {return _span == ts._span;}
    constexpr bool operator !=(const timespan& ts) const {return _span != ts._span;}
    constexpr bool operator < (const timespan& ts) const {return _span <  ts._span;}
    constexpr bool operator <=(const timespan& ts) const {return _span <= ts._span;}
    constexpr bool operator > (const timespan& ts) const {return _span >  ts._span;}
    constexpr bool operator >=(const timespan& ts) const {return _span >= ts._span;}

    constexpr bool operator ==(time_t microseconds) const {return _span == microseconds;}
    constexpr bool operator !=(time_t microseconds) const {return _span != microseconds;}
    constexpr bool operator < (time_t microseconds) const {return _span <  microseconds;}
    constexpr bool operator <=(time_t microseconds) const {return _span <= microseconds;}
    constexpr bool operator > (time_t microseconds) const {return _span >  microseconds;}
    constexpr bool operator >=(time_t microseconds) const {return _span >= microseconds;}

    constexpr timespan operator +(const timespan& ts) const {return timespan(_span + ts._span);}
    constexpr timespan operator -(const timespan& ts) const {return timespan(_span - ts._span);}
    timespan& operator +=(const timespan& ts) {_span += ts._span; return *this;}
    timespan& operator -=(const timespan& ts) {_span -= ts._span; return *this;}

    constexpr timespan operator +(time_t microseconds) const {return timespan(_span + microseconds);}
    constexpr timespan operator -(time_t microseconds) const {return timespan(_span - microseconds);}
    timespan& operator +=(time_t microseconds) {_span += microseconds; return *this;}
    timespan& operator -=(time_t microseconds) {_span -= microseconds; return *this;}

    // Returns the number of days.
    constexpr int days(void) const {return static_cast<int>(_span / details::days);}

    // Returns the number of hours(0 to 23).
    constexpr int hours(void) const {return static_cast<int>(_span / details::hours % 24);}

    // Returns the number of minutes(0 to 59).
    constexpr int minutes(void) const {return static_cast<int>(_span / details::minutes % 60);}

    // Returns the number of seconds(0 to 59).
    constexpr int seconds(void) const {return static_cast<int>(_span / details::seconds % 60);}

    // Returns the number of milliseconds(0 to 999).
    constexpr int milliseconds(void) const {return static_cast<int>(_span / details::milliseconds % 1000);}

    // Returns the fractions of a millisecond in microseconds(0 to 999).
    constexpr int microseconds(void) const {return static_cast<int>(_span % 1000);}

    // Returns the total number of days.
    constexpr int total_days(void) const {return days();}

    // Returns the total number of hours.
    constexpr int total_hours(void) const {return static_cast<int>(_span / details::hours);}

    // Returns the total number of minutes.
    constexpr int total_minutes(void) const {return static_cast<int>(_span / details::minutes);}

    // Returns the total number of seconds.
    constexpr int total_seconds(void) const {return static_cast<int>(_span / details::seconds);}

    // Returns the total number of milliseconds.
    constexpr time_t total_milliseconds(void) const {return _span / details::milliseconds;}

    // Returns the total number of microseconds.
    constexpr time_t total_microseconds(void) const {return _span;}

private:
    time_t _span;  // Expressed in microseconds
};

} // namespace core

#endif // __TIMESPAN_H__