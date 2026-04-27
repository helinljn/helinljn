#pragma once

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "timespan.h"

namespace core {

////////////////////////////////////////////////////////////////
// 时间戳保存一个单调递增*的时间值，理论精度为微秒
// 时间戳之间可以互相比较，并且支持简单的算术运算
//
// 内部参考时间为 Unix 纪元，即 1970 年 1 月 1 日午夜的微秒数
////////////////////////////////////////////////////////////////
class timestamp final
{
public:
    // 每秒的分辨率单位数，由于时间戳的精度为微秒，因此该值始终为 1000000
    static inline constexpr time_t resolution = 1000000;

public:
    // 创建一个表示当前时间的时间戳
    explicit timestamp() : tsval_(0) {update();}

    // 根据 Unix 纪元时间创建时间戳(以微秒表示)
    explicit timestamp(time_t epoch_microseconds) : tsval_(epoch_microseconds) {}

    // 拷贝构造与移动构造函数
    timestamp(const timestamp&) = default;
    timestamp(timestamp&&) = default;

    // 拷贝赋值与移动赋值运算符
    timestamp& operator =(const timestamp&) = default;
    timestamp& operator =(timestamp&&) = default;

    // 赋值为一个 Unix 纪元时间(以微秒表示)
    timestamp& operator =(time_t epoch_microseconds) {return assign(epoch_microseconds);}

    // 设置一个 Unix 纪元时间(以微秒表示)
    timestamp& assign(time_t epoch_microseconds) {tsval_ = epoch_microseconds; return *this;}

    // 使用当前时间更新时间戳
    void update()
    {
        const auto duration = std::chrono::system_clock::now().time_since_epoch();
        tsval_ = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    }

    // 返回自该时间戳所表示时刻起已经过去的时间，等价于 timestamp() - *this
    time_t elapsed() const {return timestamp() - *this;}

    // 当且仅当自该时间戳所表示时刻起，已经过了给定的时间间隔(以微秒表示)时返回 true
    bool is_elapsed(time_t interval) const {return elapsed() >= interval;}

    // 返回以秒表示的时间戳
    time_t epoch_time() const {return tsval_ / resolution;}

    // 返回以微秒表示的时间戳
    time_t epoch_microseconds() const {return tsval_;}

    bool operator ==(const timestamp& ts) const {return tsval_ == ts.tsval_;}
    bool operator !=(const timestamp& ts) const {return tsval_ != ts.tsval_;}
    bool operator < (const timestamp& ts) const {return tsval_ <  ts.tsval_;}
    bool operator <=(const timestamp& ts) const {return tsval_ <= ts.tsval_;}
    bool operator > (const timestamp& ts) const {return tsval_ >  ts.tsval_;}
    bool operator >=(const timestamp& ts) const {return tsval_ >= ts.tsval_;}

    timestamp  operator + (const timespan& span) const {return timestamp(tsval_ + span.total_microseconds());}
    timestamp  operator - (const timespan& span) const {return timestamp(tsval_ - span.total_microseconds());}
    timestamp& operator +=(const timespan& span) {return assign(tsval_ + span.total_microseconds());}
    timestamp& operator -=(const timespan& span) {return assign(tsval_ - span.total_microseconds());}

    timestamp  operator + (time_t microseconds) const {return timestamp(tsval_ + microseconds);}
    timestamp  operator - (time_t microseconds) const {return timestamp(tsval_ - microseconds);}
    timestamp& operator +=(time_t microseconds) {return assign(tsval_ + microseconds);}
    timestamp& operator -=(time_t microseconds) {return assign(tsval_ - microseconds);}

    time_t operator -(const timestamp& ts) const {return tsval_ - ts.tsval_;}

private:
    time_t tsval_;  // 以微秒表示
};

} // namespace core

#endif // TIMESTAMP_H