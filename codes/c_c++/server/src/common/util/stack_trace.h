#ifndef __STACK_TRACE_H__
#define __STACK_TRACE_H__

#include <string>
#include <vector>
#include <atomic>
#include <mutex>

namespace common {

////////////////////////////////////////////////////////////////
// Stack trace snapshot provider
// Capture the current stack trace snapshot
////////////////////////////////////////////////////////////////
class stack_trace final
{
public:
    struct frame
    {
        void*       address;
        std::string module;
        std::string function;
        std::string filename;
        int         line;
    };

public:
    ////////////////////////////////////////////////////////////////
    // @brief Initialize stack trace(This method should be called
    //        before you start capture any stack trace snapshots!)
    //
    // @param
    // @return
    ////////////////////////////////////////////////////////////////
    static void initialize(void);

    ////////////////////////////////////////////////////////////////
    // @brief Cleanup stack trace(This method should be called just
    //        before the current process exits!
    //
    // @param
    // @return
    ////////////////////////////////////////////////////////////////
    static void cleanup(void);

public:
    stack_trace(void);

    stack_trace(const stack_trace&) = default;
    stack_trace& operator=(const stack_trace&) = default;

    stack_trace(stack_trace&&) = default;
    stack_trace& operator=(stack_trace&&) = default;

    ////////////////////////////////////////////////////////////////
    // @brief Get frames
    //
    // @param
    // @return
    ////////////////////////////////////////////////////////////////
    const std::vector<frame>& get_frames(void) const {return _frames;}

    ////////////////////////////////////////////////////////////////
    // @brief Get string from the current stack trace snapshot
    //
    // @param
    // @return
    ////////////////////////////////////////////////////////////////
    std::string to_string(void) const;

private:
    std::vector<frame>      _frames;
    static std::atomic_bool _initialized;
    static std::mutex       _capture_mutex;
};

} // namespace common

#endif // __STACK_TRACE_H__