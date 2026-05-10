#include "light_hook.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <mutex>
#include <tuple>
#include <thread>
#include <vector>

#if defined(CORE_PLATFORM_WINDOWS)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <TlHelp32.h>
#elif defined(CORE_PLATFORM_LINUX)
    #include <dirent.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <signal.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <sys/mman.h>
    #include <sys/syscall.h>
    #include <sys/ucontext.h>
#endif // defined(CORE_PLATFORM_WINDOWS)

#if !defined(_M_X64) && !defined(__x86_64__) && !defined(__amd64__)
    #error "light_hook only supports x86_64."
#endif // !defined(_M_X64) && !defined(__x86_64__) && !defined(__amd64__)

namespace {

constexpr size_t k_jump_size         = 14;
constexpr size_t k_trampoline_size   = 512;
constexpr int    k_patch_retry_count = 1000;
constexpr int    k_trap_grace_ms     = 2;

const unsigned char k_jump_code[k_jump_size] = {
    0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

std::mutex                       g_hook_mutex;
std::atomic<hook_information_t*> g_active_transaction{nullptr};

#if defined(CORE_PLATFORM_WINDOWS)
void* g_trap_handler_handle = nullptr;
#elif defined(CORE_PLATFORM_LINUX)
bool g_trap_handler_installed = false;
#endif // defined(CORE_PLATFORM_WINDOWS)

struct instruction_info
{
    int           length              = 0;
    bool          supported           = true;
    bool          rip_relative        = false;
    int           displacement_offset = 0;
    int           displacement_size   = 0;
    int           relative_offset     = 0;
    int           relative_size       = 0;
    int           relative_kind       = 0;  // 1 call, 2 jmp, 3 jcc
    int           modrm_reg           = -1;
    bool          terminal            = false;
    unsigned char condition           = 0;
};

struct relocated_instruction
{
    int              original_offset = 0;
    size_t           output_offset   = 0;
    size_t           output_size     = 0;
    instruction_info info;
};

#if defined(CORE_PLATFORM_LINUX)
struct protection_range
{
    uintptr_t start      = 0;
    uintptr_t end        = 0;
    int       protection = 0;
};
#endif // defined(CORE_PLATFORM_LINUX)

struct platform_protection_state
{
#if defined(CORE_PLATFORM_WINDOWS)
    unsigned long old_protection = 0;
#elif defined(CORE_PLATFORM_LINUX)
    std::vector<protection_range> ranges;
#endif // defined(CORE_PLATFORM_WINDOWS)
};

bool fits_int32(intptr_t value)
{
    return value >= std::numeric_limits<int32_t>::min() && value <= std::numeric_limits<int32_t>::max();
}

bool has_valid_copy_size(const hook_information_t* information)
{
    return information != nullptr
        && information->original_function != nullptr
        && information->target_function != nullptr
        && information->bytes_to_copy >= static_cast<int>(k_jump_size)
        && information->bytes_to_copy <= static_cast<int>(sizeof(information->original_buffer));
}

bool is_legacy_prefix(unsigned char value)
{
    switch (value)
    {
    case 0xF0:
    case 0xF2:
    case 0xF3:
    case 0x2E:
    case 0x36:
    case 0x3E:
    case 0x26:
    case 0x64:
    case 0x65:
    case 0x66:
    case 0x67:
        return true;
    default:
        return false;
    }
}

bool has_opcode(unsigned char opcode, const unsigned char* values, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        if (values[i] == opcode)
            return true;
    }

    return false;
}

bool is_vex_or_evex_prefix(unsigned char opcode)
{
    return opcode == 0xC4 || opcode == 0xC5 || opcode == 0x62;
}

void parse_modrm(const unsigned char* start, const unsigned char*& cursor, bool address_prefix, instruction_info& info)
{
    const unsigned char* modrm_ptr = cursor;
    const unsigned char  modrm     = *cursor++;
    const unsigned char  mod       = static_cast<unsigned char>(modrm >> 6);
    const unsigned char  reg       = static_cast<unsigned char>((modrm >> 3) & 0x07);
    const unsigned char  rm        = static_cast<unsigned char>(modrm & 0x07);
    info.modrm_reg                 = reg;

    if (address_prefix)
    {
        if (mod == 0 && rm == 6)
            cursor += 2;
        else if (mod == 1)
            cursor += 1;
        else if (mod == 2)
            cursor += 2;
        return;
    }

    if (mod != 3 && rm == 4)
    {
        const unsigned char sib  = *cursor++;
        const unsigned char base = static_cast<unsigned char>(sib & 0x07);
        if (mod == 0 && base == 5)
            cursor += 4;
    }
    else if (mod == 0 && rm == 5)
    {
        info.rip_relative        = true;
        info.displacement_offset = static_cast<int>(cursor - start);
        info.displacement_size   = 4;
        cursor += 4;
    }

    if (mod == 1)
        cursor += 1;
    else if (mod == 2)
        cursor += 4;

    (void)modrm_ptr;
}

instruction_info decode_instruction(const void* address)
{
    static const unsigned char op1_modrm[] = {0x62, 0x63, 0x69, 0x6B, 0xC0, 0xC1, 0xC4, 0xC5, 0xC6, 0xC7, 0xD0, 0xD1, 0xD2, 0xD3, 0xF6, 0xF7, 0xFE, 0xFF};
    static const unsigned char op1_imm8[]  = {0x6A, 0x6B, 0x80, 0x82, 0x83, 0xA8, 0xC0, 0xC1, 0xC6, 0xCD, 0xD4, 0xD5, 0xEB};
    static const unsigned char op1_imm32[] = {0x68, 0x69, 0x81, 0xA9, 0xC7, 0xE8, 0xE9};
    static const unsigned char op2_modrm[] = {0x0D, 0xA3, 0xA4, 0xA5, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF};

    instruction_info     info;
    const unsigned char* start          = static_cast<const unsigned char*>(address);
    const unsigned char* cursor         = start;
    bool                 operand_prefix = false;
    bool                 address_prefix = false;
    bool                 rex_w          = false;
    for (int i = 0; i < 15; ++i)
    {
        if (is_legacy_prefix(*cursor))
        {
            operand_prefix = (*cursor == 0x66) || operand_prefix;
            address_prefix = (*cursor == 0x67) || address_prefix;
            ++cursor;

            continue;
        }

        if (*cursor >= 0x40 && *cursor <= 0x4F)
        {
            rex_w = ((*cursor & 0x08) != 0) || rex_w;
            ++cursor;

            continue;
        }

        break;
    }

    // 0x67 switches x86_64 instructions to 32-bit addressing. The lightweight
    // relocator only handles the normal 64-bit ModR/M RIP-relative form.
    if (address_prefix)
    {
        info.supported = false;
        info.length    = 0;

        return info;
    }

    const unsigned char opcode = *cursor++;
    if (is_vex_or_evex_prefix(opcode))
    {
        info.supported = false;
        info.length    = 0;

        return info;
    }

    if (opcode == 0xC3 || opcode == 0xCB)
    {
        info.terminal = true;
        info.length   = static_cast<int>(cursor - start);

        return info;
    }

    if (opcode == 0xC2 || opcode == 0xCA)
    {
        info.terminal = true;
        cursor       += 2;
        info.length   = static_cast<int>(cursor - start);

        return info;
    }

    if (opcode == 0xEA || opcode == 0x9A || opcode == 0xCC || opcode == 0xCF)
    {
        info.supported = false;
        info.length    = 0;

        return info;
    }

    bool has_modrm_byte = false;
    int  immediate_size = 0;
    if (opcode == 0xE8)
    {
        info.relative_kind   = 1;
        info.relative_offset = static_cast<int>(cursor - start);
        info.relative_size   = 4;
        cursor              += 4;
        info.length          = static_cast<int>(cursor - start);

        return info;
    }

    if (opcode == 0xE9)
    {
        info.relative_kind   = 2;
        info.terminal        = true;
        info.relative_offset = static_cast<int>(cursor - start);
        info.relative_size   = 4;
        cursor              += 4;
        info.length          = static_cast<int>(cursor - start);

        return info;
    }

    if (opcode == 0xEB)
    {
        info.relative_kind   = 2;
        info.terminal        = true;
        info.relative_offset = static_cast<int>(cursor - start);
        info.relative_size   = 1;
        cursor              += 1;
        info.length          = static_cast<int>(cursor - start);

        return info;
    }

    if (opcode >= 0xE0 && opcode <= 0xE3)
    {
        // LOOP/LOOPE/LOOPNE/JRCXZ are relative control-flow instructions with
        // special semantics. Refuse them instead of copying a stale offset.
        info.supported = false;
        info.length    = 0;

        return info;
    }

    if (opcode >= 0x70 && opcode <= 0x7F)
    {
        info.relative_kind   = 3;
        info.condition       = static_cast<unsigned char>(opcode & 0x0F);
        info.relative_offset = static_cast<int>(cursor - start);
        info.relative_size   = 1;
        cursor              += 1;
        info.length          = static_cast<int>(cursor - start);

        return info;
    }

    if (opcode == 0x0F)
    {
        const unsigned char opcode2 = *cursor++;
        if (opcode2 == 0x0B)
        {
            info.supported = false;
            info.length    = 0;

            return info;
        }

        if (opcode2 >= 0x80 && opcode2 <= 0x8F)
        {
            info.relative_kind   = 3;
            info.condition       = static_cast<unsigned char>(opcode2 & 0x0F);
            info.relative_offset = static_cast<int>(cursor - start);
            info.relative_size   = 4;
            cursor              += 4;
            info.length          = static_cast<int>(cursor - start);

            return info;
        }

        if (opcode2 == 0x38 || opcode2 == 0x3A)
        {
            std::ignore = *cursor++;
            has_modrm_byte = true;

            if (opcode2 == 0x3A)
                immediate_size = 1;
        }
        else
        {
            const unsigned char high = static_cast<unsigned char>(opcode2 >> 4);
            const unsigned char low  = static_cast<unsigned char>(opcode2 & 0x0F);
            if (high == 8)
                immediate_size = 4;
            else if ((high == 7 && low < 4) || opcode2 == 0xA4 || opcode2 == 0xC2 || (opcode2 > 0xC3 && opcode2 <= 0xC6) || opcode2 == 0xBA || opcode2 == 0xAC)
                immediate_size = 1;

            has_modrm_byte = has_opcode(opcode2, op2_modrm, sizeof(op2_modrm))
                                || (high != 3 && high > 0 && high < 7)
                                || opcode2 >= 0xD0
                                || (high == 7 && low != 7)
                                || high == 9
                                || high == 0xB
                                || (high == 0xC && low < 8)
                                || (high == 0 && low < 4);
        }
    }
    else
    {
        const unsigned char high = static_cast<unsigned char>(opcode >> 4);
        const unsigned char low  = static_cast<unsigned char>(opcode & 0x0F);
        if (   (high == 0xE && low < 8)
            || (high == 0xB && low < 8)
            || high == 7
            || (high < 4 && (low == 4 || low == 0xC))
            || (opcode == 0xF6 && !(*(cursor) & 48))
            || has_opcode(opcode, op1_imm8, sizeof(op1_imm8)))
        {
            immediate_size = 1;
        }
        else if (opcode == 0xC2 || opcode == 0xCA)
        {
            immediate_size = 2;
        }
        else if (opcode == 0xC8)
        {
            immediate_size = 3;
        }
        else if (  (high < 4 && (low == 5 || low == 0xD))
                || (high == 0xB && low >= 8)
                || (opcode == 0xF7 && !(*(cursor) & 48))
                || has_opcode(opcode, op1_imm32, sizeof(op1_imm32)))
        {
            if (opcode >= 0xB8 && opcode <= 0xBF)
                immediate_size = rex_w ? 8 : 4;
            else if (opcode == 0xC7 || opcode == 0x69)
                immediate_size = operand_prefix ? 2 : 4;
            else
                immediate_size = operand_prefix ? 2 : 4;
        }
        else if (high == 0xA && low < 4)
        {
            immediate_size = 8;
        }
        else if (opcode == 0xEA || opcode == 0x9A)
        {
            immediate_size = operand_prefix ? 4 : 6;
        }

        has_modrm_byte = has_opcode(opcode, op1_modrm, sizeof(op1_modrm))
                            || (high < 4 && (low < 4 || (low >= 8 && low < 0xC)))
                            || high == 8
                            || (high == 0xD && low >= 8);
    }

    if (has_modrm_byte)
        parse_modrm(start, cursor, address_prefix, info);

    if (opcode == 0xFF && (info.modrm_reg == 4 || info.modrm_reg == 5))
        info.terminal = true;

    cursor     += immediate_size;
    info.length = static_cast<int>(cursor - start);
    if (info.length <= 0 || info.length > 15)
        info.supported = false;

    return info;
}

void write_absolute_jump(unsigned char* output, const void* target)
{
    std::memcpy(output, k_jump_code, sizeof(k_jump_code));
    const uintptr_t target_value = reinterpret_cast<uintptr_t>(target);
    std::memcpy(output + 6, &target_value, sizeof(target_value));
}

bool append_bytes(std::vector<unsigned char>& output, const void* data, size_t size)
{
    if (output.size() + size > k_trampoline_size)
        return false;

    const unsigned char* bytes = static_cast<const unsigned char*>(data);
    output.insert(output.end(), bytes, bytes + size);

    return true;
}

bool append_absolute_jump(std::vector<unsigned char>& output, const void* target)
{
    unsigned char buffer[k_jump_size];
    write_absolute_jump(buffer, target);
    return append_bytes(output, buffer, sizeof(buffer));
}

bool append_absolute_call(std::vector<unsigned char>& output, const void* target)
{
    // call [rip+2]; jmp +8; <abs64 target>
    const unsigned char prefix[] = {0xFF, 0x15, 0x02, 0x00, 0x00, 0x00, 0xEB, 0x08};
    if (!append_bytes(output, prefix, sizeof(prefix)))
        return false;

    const uintptr_t target_value = reinterpret_cast<uintptr_t>(target);
    return append_bytes(output, &target_value, sizeof(target_value));
}

bool append_absolute_jcc(std::vector<unsigned char>& output, unsigned char condition, const void* target)
{
    const unsigned char inverted_condition = static_cast<unsigned char>(condition ^ 0x01);
    const unsigned char skip_jump[]        = {
        0x0F,
        static_cast<unsigned char>(0x80 | inverted_condition),
        0x0E,
        0x00,
        0x00,
        0x00
    };

    return append_bytes(output, skip_jump, sizeof(skip_jump)) && append_absolute_jump(output, target);
}

intptr_t read_relative_value(const unsigned char* instruction, const instruction_info& info)
{
    if (info.relative_size == 1)
    {
        int8_t value = 0;
        std::memcpy(&value, instruction + info.relative_offset, sizeof(value));
        return value;
    }

    int32_t value = 0;
    std::memcpy(&value, instruction + info.relative_offset, sizeof(value));
    return value;
}

bool compute_relative_disp32(uintptr_t target, uintptr_t next_instruction, int32_t* displacement)
{
    if (!displacement)
        return false;

    if (target >= next_instruction)
    {
        const uintptr_t delta = target - next_instruction;
        if (delta > static_cast<uintptr_t>(std::numeric_limits<int32_t>::max()))
            return false;

        *displacement = static_cast<int32_t>(delta);
        return true;
    }

    const uintptr_t delta = next_instruction - target;
    if (delta > static_cast<uintptr_t>(std::numeric_limits<int32_t>::max()) + 1U)
        return false;

    *displacement = static_cast<int32_t>(-static_cast<int64_t>(delta));
    return true;
}

uintptr_t add_signed_offset(uintptr_t base, intptr_t offset)
{
    if (offset >= 0)
        return base + static_cast<uintptr_t>(offset);

    return base - static_cast<uintptr_t>(-offset);
}

size_t relocated_instruction_size(const instruction_info& info)
{
    if (info.relative_kind == 1)
        return 16;

    if (info.relative_kind == 2)
        return k_jump_size;

    if (info.relative_kind == 3)
        return 6 + k_jump_size;

    return static_cast<size_t>(info.length);
}

bool resolve_internal_relocation_target(uintptr_t                                  target,
                                        uintptr_t                                  source_base,
                                        int                                        bytes_to_copy,
                                        const std::vector<relocated_instruction>&  instructions,
                                        unsigned char*                             trampoline_base,
                                        const void**                               relocated_target)
{
    if (!relocated_target)
        return false;

    const uintptr_t source_end = source_base + static_cast<uintptr_t>(bytes_to_copy);
    if (target < source_base || target >= source_end)
    {
        *relocated_target = reinterpret_cast<const void*>(target);
        return true;
    }

    const auto target_offset = static_cast<int>(target - source_base);
    for (const auto& instruction : instructions)
    {
        if (instruction.original_offset != target_offset)
            continue;

        *relocated_target = trampoline_base + instruction.output_offset;
        return true;
    }

    return false;
}

bool append_relocated_instruction(std::vector<unsigned char>& output,
                                  const unsigned char*        source_instruction,
                                  unsigned char*              trampoline_base,
                                  const instruction_info&     info,
                                  uintptr_t                   source_base,
                                  int                         bytes_to_copy,
                                  const std::vector<relocated_instruction>& instructions)
{
    const auto source_address = reinterpret_cast<uintptr_t>(source_instruction);
    const auto source_next    = source_address + static_cast<uintptr_t>(info.length);
    if (info.relative_kind != 0)
    {
        const uintptr_t target = add_signed_offset(source_next, read_relative_value(source_instruction, info));
        const void* relocated_target = nullptr;
        if (!resolve_internal_relocation_target(target, source_base, bytes_to_copy, instructions, trampoline_base, &relocated_target))
            return false;

        if (info.relative_kind == 1)
            return append_absolute_call(output, relocated_target);

        if (info.relative_kind == 2)
            return append_absolute_jump(output, relocated_target);

        return append_absolute_jcc(output, info.condition, relocated_target);
    }

    const size_t output_offset = output.size();
    if (!append_bytes(output, source_instruction, static_cast<size_t>(info.length)))
        return false;

    if (info.rip_relative)
    {
        int32_t source_disp = 0;
        std::memcpy(&source_disp, source_instruction + info.displacement_offset, sizeof(source_disp));
        const uintptr_t source_target = add_signed_offset(source_next, source_disp);
        const uintptr_t dest_next     = reinterpret_cast<uintptr_t>(trampoline_base) + output_offset + static_cast<uintptr_t>(info.length);
        int32_t         relocated_disp = 0;
        if (!compute_relative_disp32(source_target, dest_next, &relocated_disp))
            return false;

        std::memcpy(output.data() + output_offset + static_cast<size_t>(info.displacement_offset), &relocated_disp, sizeof(relocated_disp));
    }

    return true;
}

bool build_trampoline(hook_information_t* information)
{
    std::vector<unsigned char> relocated;
    relocated.reserve(k_trampoline_size);

    const auto*    source     = static_cast<const unsigned char*>(information->original_function);
    unsigned char* trampoline = static_cast<unsigned char*>(information->trampoline);
    const auto     source_base = reinterpret_cast<uintptr_t>(source);

    std::vector<relocated_instruction> instructions;
    instructions.reserve(16);

    int copied = 0;
    while (copied < information->bytes_to_copy)
    {
        const instruction_info info = decode_instruction(source + copied);
        if (!info.supported || info.length <= 0 || copied + info.length > information->bytes_to_copy)
            return false;

        relocated_instruction instruction;
        instruction.original_offset = copied;
        instruction.output_offset   = relocated.size();
        instruction.output_size     = relocated_instruction_size(info);
        instruction.info            = info;
        if (instruction.output_offset + instruction.output_size > k_trampoline_size)
            return false;

        instructions.push_back(instruction);
        relocated.resize(instruction.output_offset + instruction.output_size);

        copied += info.length;
    }

    relocated.clear();
    for (const auto& instruction : instructions)
    {
        if (relocated.size() != instruction.output_offset)
            return false;

        if (!append_relocated_instruction(relocated,
                                          source + instruction.original_offset,
                                          trampoline,
                                          instruction.info,
                                          source_base,
                                          information->bytes_to_copy,
                                          instructions))
            return false;

        if (relocated.size() != instruction.output_offset + instruction.output_size)
            return false;
    }

    const auto* return_address = static_cast<const unsigned char*>(information->original_function) + information->bytes_to_copy;
    if (!append_absolute_jump(relocated, return_address))
        return false;

    std::memcpy(trampoline, relocated.data(), relocated.size());
    information->trampoline_size = static_cast<int>(k_trampoline_size);

    return true;
}

size_t page_size()
{
#if defined(CORE_PLATFORM_WINDOWS)
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwPageSize;
#elif defined(CORE_PLATFORM_LINUX)
    return static_cast<size_t>(getpagesize());
#endif // defined(CORE_PLATFORM_WINDOWS)
}

uintptr_t align_down(uintptr_t value, size_t alignment)
{
    return value & ~(static_cast<uintptr_t>(alignment) - 1U);
}

#if defined(CORE_PLATFORM_LINUX)
int permissions_to_protection(const char* permissions)
{
    int protection = 0;
    if (permissions[0] == 'r')
        protection |= PROT_READ;

    if (permissions[1] == 'w')
        protection |= PROT_WRITE;

    if (permissions[2] == 'x')
        protection |= PROT_EXEC;

    return protection;
}

bool record_original_protection(uintptr_t start, uintptr_t end, platform_protection_state* state)
{
    state->ranges.clear();

    FILE* maps = std::fopen("/proc/self/maps", "r");
    if (!maps)
        return false;

    char line[512];
    while (std::fgets(line, sizeof(line), maps))
    {
        unsigned long long map_start = 0;
        unsigned long long map_end   = 0;
        char               perms[5]  = {};
        if (std::sscanf(line, "%llx-%llx %4s", &map_start, &map_end, perms) != 3)
            continue;

        const uintptr_t range_start = static_cast<uintptr_t>(map_start);
        const uintptr_t range_end   = static_cast<uintptr_t>(map_end);
        if (range_end <= start || range_start >= end)
            continue;

        protection_range range;
        range.start      = std::max(range_start, start);
        range.end        = std::min(range_end, end);
        range.protection = permissions_to_protection(perms);
        state->ranges.push_back(range);
    }

    std::fclose(maps);

    if (state->ranges.empty())
        return false;

    std::sort(state->ranges.begin(), state->ranges.end(), [](const protection_range& left, const protection_range& right) {
        return left.start < right.start;
    });

    uintptr_t covered = start;
    for (const auto& range : state->ranges)
    {
        if (range.start > covered)
            return false;

        if (range.end > covered)
            covered = range.end;
    }

    return covered >= end;
}
#endif // defined(CORE_PLATFORM_LINUX)

void* platform_allocate_near(const void* target, size_t size)
{
#if defined(CORE_PLATFORM_WINDOWS)
    SYSTEM_INFO info;
    GetSystemInfo(&info);

    const uintptr_t granularity = info.dwAllocationGranularity;
    const uintptr_t base        = align_down(reinterpret_cast<uintptr_t>(target), granularity);
    const uintptr_t range       = 0x7FFF0000ULL;
    for (uintptr_t offset = 0; offset < range; offset += granularity)
    {
        const auto high   = base + offset;
        void*      memory = VirtualAlloc(reinterpret_cast<void*>(high), size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (memory)
            return memory;

        if (base > offset)
        {
            const auto low = base - offset;
            memory         = VirtualAlloc(reinterpret_cast<void*>(low), size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (memory)
                return memory;
        }
    }

    return VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#elif defined(CORE_PLATFORM_LINUX)
    const size_t    page      = page_size();
    const uintptr_t base      = align_down(reinterpret_cast<uintptr_t>(target), page);
    const uintptr_t max_range = 0x7FFF0000ULL;
    for (uintptr_t offset = 0; offset < max_range; offset += page * 16)
    {
        const uintptr_t candidates[] = { base + offset, base > offset ? base - offset : 0 };
        for (uintptr_t candidate : candidates)
        {
            if (candidate == 0)
                continue;

            void* memory = mmap(reinterpret_cast<void*>(candidate), size,
                                PROT_READ | PROT_WRITE,
                                MAP_PRIVATE | MAP_ANONYMOUS
#ifdef MAP_FIXED_NOREPLACE
                                    | MAP_FIXED_NOREPLACE
#endif // MAP_FIXED_NOREPLACE
                                ,
                                -1,
                                0);
            if (memory == MAP_FAILED)
                continue;

            if (fits_int32(reinterpret_cast<intptr_t>(memory) - reinterpret_cast<intptr_t>(target)))
                return memory;

            munmap(memory, size);
        }
    }

    void* memory = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return memory == MAP_FAILED ? nullptr : memory;
#endif // defined(CORE_PLATFORM_WINDOWS)
}

void platform_free(void* address, size_t size)
{
    if (!address)
        return;

#if defined(CORE_PLATFORM_WINDOWS)
    (void)size;
    VirtualFree(address, 0, MEM_RELEASE);
#elif defined(CORE_PLATFORM_LINUX)
    munmap(address, size);
#endif // defined(CORE_PLATFORM_WINDOWS)
}

bool platform_make_executable(void* address, size_t size)
{
    if (!address || size == 0)
        return false;

#if defined(CORE_PLATFORM_WINDOWS)
    DWORD ignored = 0;
    return VirtualProtect(address, size, PAGE_EXECUTE_READ, &ignored) != 0;
#elif defined(CORE_PLATFORM_LINUX)
    const size_t page     = page_size();
    const auto   start    = align_down(reinterpret_cast<uintptr_t>(address), page);
    const auto   end      = reinterpret_cast<uintptr_t>(address) + size;
    const auto   end_page = align_down(end + page - 1, page);
    return mprotect(reinterpret_cast<void*>(start), end_page - start, PROT_READ | PROT_EXEC) == 0;
#endif // defined(CORE_PLATFORM_WINDOWS)
}

bool platform_make_writable(void* address, size_t size, platform_protection_state* protection_state)
{
#if defined(CORE_PLATFORM_WINDOWS)
    DWORD old = 0;
    if (!VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &old))
        return false;
    protection_state->old_protection = old;
    return true;
#elif defined(CORE_PLATFORM_LINUX)
    const size_t page     = page_size();
    const auto   start    = align_down(reinterpret_cast<uintptr_t>(address), page);
    const auto   end      = reinterpret_cast<uintptr_t>(address) + size;
    const auto   end_page = align_down(end + page - 1, page);
    if (!record_original_protection(start, end_page, protection_state))
        return false;

    return mprotect(reinterpret_cast<void*>(start), end_page - start, PROT_READ | PROT_WRITE | PROT_EXEC) == 0;
#endif // defined(CORE_PLATFORM_WINDOWS)
}

void platform_restore_protection(void* address, size_t size, const platform_protection_state& protection_state)
{
#if defined(CORE_PLATFORM_WINDOWS)
    DWORD ignored = 0;
    VirtualProtect(address, size, protection_state.old_protection, &ignored);
#elif defined(CORE_PLATFORM_LINUX)
    (void)address;
    (void)size;
    for (const auto& range : protection_state.ranges)
        mprotect(reinterpret_cast<void*>(range.start), range.end - range.start, range.protection);
#endif // defined(CORE_PLATFORM_WINDOWS)
}

void platform_flush_instruction_cache(void* address, size_t size)
{
#if defined(CORE_PLATFORM_WINDOWS)
    FlushInstructionCache(GetCurrentProcess(), address, size);
#elif defined(CORE_PLATFORM_LINUX)
    __builtin___clear_cache(static_cast<char*>(address), static_cast<char*>(address) + size);
#endif // defined(CORE_PLATFORM_WINDOWS)
}

void make_patch_bytes(unsigned char* buffer, const void* target, int patch_size)
{
    std::memset(buffer, 0x90, static_cast<size_t>(patch_size));
    write_absolute_jump(buffer, target);
}

bool entry_matches_original(const hook_information_t* information)
{
    if (!has_valid_copy_size(information))
        return false;

    return std::memcmp(information->original_function, information->original_buffer, static_cast<size_t>(information->bytes_to_copy)) == 0;
}

bool entry_matches_patch(const hook_information_t* information)
{
    if (!has_valid_copy_size(information))
        return false;

    unsigned char patch_bytes[sizeof(information->original_buffer)];
    make_patch_bytes(patch_bytes, information->target_function, information->bytes_to_copy);

    return std::memcmp(information->original_function, patch_bytes, static_cast<size_t>(information->bytes_to_copy)) == 0;
}

bool is_in_patch_range(uintptr_t ip, const hook_information_t* information)
{
    const auto start = reinterpret_cast<uintptr_t>(information->original_function);
    const auto end   = start + static_cast<uintptr_t>(information->bytes_to_copy);
    return ip >= start && ip < end;
}

#if defined(CORE_PLATFORM_WINDOWS)
LONG CALLBACK trap_handler(EXCEPTION_POINTERS* exception_info)
{
    if (exception_info == nullptr || exception_info->ExceptionRecord == nullptr || exception_info->ContextRecord == nullptr)
        return EXCEPTION_CONTINUE_SEARCH;

    if (exception_info->ExceptionRecord->ExceptionCode != EXCEPTION_BREAKPOINT)
        return EXCEPTION_CONTINUE_SEARCH;

    hook_information_t* information = g_active_transaction.load(std::memory_order_acquire);
    if (!information)
        return EXCEPTION_CONTINUE_SEARCH;

    const auto entry_rip    = reinterpret_cast<uintptr_t>(information->original_function);
    const auto expected_rip = entry_rip + 1U;
    const auto current_rip  = static_cast<uintptr_t>(exception_info->ContextRecord->Rip);
    if (current_rip != entry_rip && current_rip != expected_rip)
        return EXCEPTION_CONTINUE_SEARCH;

    exception_info->ContextRecord->Rip = reinterpret_cast<DWORD64>(information->target_function);

    return EXCEPTION_CONTINUE_EXECUTION;
}

bool ensure_trap_handler_installed()
{
    static std::once_flag once;
    std::call_once(once, []() {
        g_trap_handler_handle = AddVectoredExceptionHandler(1, trap_handler);
    });
    return g_trap_handler_handle != nullptr;
}

class thread_parker
{
public:
    ~thread_parker()
    {
        resume();
    }

    bool park()
    {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snapshot == INVALID_HANDLE_VALUE)
            return false;

        THREADENTRY32 entry;
        entry.dwSize = sizeof(entry);

        if (!Thread32First(snapshot, &entry))
        {
            CloseHandle(snapshot);
            return false;
        }

        const DWORD current_process_id = GetCurrentProcessId();
        const DWORD current_thread_id  = GetCurrentThreadId();
        do
        {
            if (entry.th32OwnerProcessID != current_process_id || entry.th32ThreadID == current_thread_id)
                continue;

            HANDLE thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, entry.th32ThreadID);
            if (!thread)
            {
                CloseHandle(snapshot);
                resume();
                return false;
            }

            if (SuspendThread(thread) == static_cast<DWORD>(-1))
            {
                CloseHandle(thread);
                CloseHandle(snapshot);
                resume();
                return false;
            }

            threads_.push_back(thread);
        } while (Thread32Next(snapshot, &entry));

        CloseHandle(snapshot);

        return true;
    }

    bool has_ip_in_range(const hook_information_t* information) const
    {
        for (HANDLE thread : threads_)
        {
            CONTEXT context;
            std::memset(&context, 0, sizeof(context));
            context.ContextFlags = CONTEXT_CONTROL;

            if (!GetThreadContext(thread, &context))
                return true;

            if (is_in_patch_range(static_cast<uintptr_t>(context.Rip), information))
                return true;
        }

        return false;
    }

    void resume()
    {
        for (HANDLE thread : threads_)
        {
            ResumeThread(thread);
            CloseHandle(thread);
        }

        threads_.clear();
    }

private:
    std::vector<HANDLE> threads_;
};
#elif defined(CORE_PLATFORM_LINUX)
struct linux_thread_record
{
    pid_t                  tid{0};
    std::atomic<int>       parked{0};
    std::atomic<int>       ignored{0};
    std::atomic<uintptr_t> rip{0};
};

constexpr int                     k_suspend_signal{SIGUSR2};
std::atomic<int>                  g_suspend_requested{0};
std::atomic<linux_thread_record*> g_suspend_records{nullptr};
std::atomic<int>                  g_suspend_record_count{0};
std::atomic<int>                  g_suspend_handler_active{0};
struct sigaction                  g_previous_trap_action{};
struct sigaction                  g_previous_suspend_action{};

struct suspend_handler_scope
{
    suspend_handler_scope()
    {
        g_suspend_handler_active.fetch_add(1, std::memory_order_acq_rel);
    }

    ~suspend_handler_scope()
    {
        g_suspend_handler_active.fetch_sub(1, std::memory_order_acq_rel);
    }
};

pid_t current_tid()
{
    return static_cast<pid_t>(syscall(SYS_gettid));
}

uintptr_t context_rip(void* context)
{
    auto* uc = static_cast<ucontext_t*>(context);
    return static_cast<uintptr_t>(uc->uc_mcontext.gregs[REG_RIP]);
}

void set_context_rip(void* context, uintptr_t rip)
{
    auto* uc = static_cast<ucontext_t*>(context);
    uc->uc_mcontext.gregs[REG_RIP] = static_cast<greg_t>(rip);
}

void call_previous_signal_action(const struct sigaction& action, int signal_number, siginfo_t* signal_info, void* context)
{
    if (action.sa_flags & SA_SIGINFO)
    {
        if (action.sa_sigaction)
            action.sa_sigaction(signal_number, signal_info, context);
        return;
    }

    if (action.sa_handler == SIG_DFL)
    {
        signal(signal_number, SIG_DFL);
        raise(signal_number);
        return;
    }

    if (action.sa_handler && action.sa_handler != SIG_IGN)
        action.sa_handler(signal_number);
}

bool is_internal_suspend_signal(int signal_number, siginfo_t* signal_info)
{
    return signal_number == k_suspend_signal
        && signal_info != nullptr
        && signal_info->si_code == SI_TKILL
        && signal_info->si_pid == getpid();
}

void suspend_signal_handler(int signal_number, siginfo_t* signal_info, void* context)
{
    suspend_handler_scope scope;
    linux_thread_record* records = g_suspend_records.load(std::memory_order_acquire);
    const int            count   = g_suspend_record_count.load(std::memory_order_acquire);
    if (!records || count <= 0)
    {
        if (is_internal_suspend_signal(signal_number, signal_info))
            return;

        call_previous_signal_action(g_previous_suspend_action, signal_number, signal_info, context);
        return;
    }

    const pid_t          tid     = current_tid();
    for (int i = 0; i < count; ++i)
    {
        if (records[i].tid != tid)
            continue;

        records[i].rip.store(context_rip(context), std::memory_order_release);
        records[i].parked.store(1, std::memory_order_release);

        while (g_suspend_requested.load(std::memory_order_acquire))
        {
#if defined(__x86_64__)
            __asm__ __volatile__("pause");
#endif // defined(__x86_64__)
        }

        records[i].parked.store(0, std::memory_order_release);

        return;
    }

    if (is_internal_suspend_signal(signal_number, signal_info))
        return;

    call_previous_signal_action(g_previous_suspend_action, signal_number, signal_info, context);
}

void trap_signal_handler(int signal_number, siginfo_t* signal_info, void* context)
{
    hook_information_t* information = g_active_transaction.load(std::memory_order_acquire);
    if (information)
    {
        const auto entry_rip    = reinterpret_cast<uintptr_t>(information->original_function);
        const auto expected_rip = entry_rip + 1U;
        const auto current_rip  = context_rip(context);
        if (current_rip == entry_rip || current_rip == expected_rip)
        {
            set_context_rip(context, reinterpret_cast<uintptr_t>(information->target_function));
            return;
        }
    }

    call_previous_signal_action(g_previous_trap_action, signal_number, signal_info, context);
}

bool ensure_trap_handler_installed()
{
    static std::once_flag once;
    std::call_once(once, []() {
        struct sigaction trap_action;
        std::memset(&trap_action, 0, sizeof(trap_action));
        trap_action.sa_sigaction = trap_signal_handler;
        trap_action.sa_flags     = SA_SIGINFO | SA_NODEFER;
        sigemptyset(&trap_action.sa_mask);
        if (sigaction(SIGTRAP, &trap_action, &g_previous_trap_action) != 0)
            return;

        struct sigaction suspend_action;
        std::memset(&suspend_action, 0, sizeof(suspend_action));
        suspend_action.sa_sigaction = suspend_signal_handler;
        suspend_action.sa_flags     = SA_SIGINFO | SA_RESTART;
        sigemptyset(&suspend_action.sa_mask);
        if (sigaction(k_suspend_signal, &suspend_action, &g_previous_suspend_action) != 0)
        {
            sigaction(SIGTRAP, &g_previous_trap_action, nullptr);
            return;
        }

        g_trap_handler_installed = true;
    });
    return g_trap_handler_installed;
}

class thread_parker
{
public:
    ~thread_parker()
    {
        resume();
    }

    bool park()
    {
        DIR* dir = opendir("/proc/self/task");
        if (!dir)
            return false;

        const pid_t        self = current_tid();
        std::vector<pid_t> tids;
        while (dirent* entry = readdir(dir))
        {
            if (entry->d_name[0] == '.')
                continue;

            const pid_t tid = static_cast<pid_t>(strtol(entry->d_name, nullptr, 10));
            if (tid <= 0 || tid == self)
                continue;

            tids.push_back(tid);
        }

        closedir(dir);

        if (tids.empty())
            return true;

        record_count_ = tids.size();
        records_.reset(new linux_thread_record[record_count_]);
        for (size_t i = 0; i < record_count_; ++i)
            records_[i].tid = tids[i];

        g_suspend_records.store(records_.get(), std::memory_order_release);
        g_suspend_record_count.store(static_cast<int>(record_count_), std::memory_order_release);
        g_suspend_requested.store(1, std::memory_order_release);

        const pid_t process_id = getpid();
        for (size_t i = 0; i < record_count_; ++i)
        {
            if (syscall(SYS_tgkill, process_id, records_[i].tid, k_suspend_signal) == 0)
                continue;

            if (errno == ESRCH)
            {
                records_[i].ignored.store(1, std::memory_order_release);
                continue;
            }

            resume();
            return false;
        }

        for (int retry = 0; retry < k_patch_retry_count; ++retry)
        {
            bool all_parked = true;
            for (size_t i = 0; i < record_count_; ++i)
            {
                if (!records_[i].ignored.load(std::memory_order_acquire)
                    && !records_[i].parked.load(std::memory_order_acquire))
                {
                    all_parked = false;
                    break;
                }
            }

            if (all_parked)
                return true;

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        resume();

        return false;
    }

    bool has_ip_in_range(const hook_information_t* information) const
    {
        for (size_t i = 0; i < record_count_; ++i)
        {
            if (records_[i].ignored.load(std::memory_order_acquire))
                continue;

            if (is_in_patch_range(records_[i].rip.load(std::memory_order_acquire), information))
                return true;
        }

        return false;
    }

    void resume()
    {
        if (records_)
        {
            g_suspend_requested.store(0, std::memory_order_release);
            for (int retry = 0; retry < k_patch_retry_count; ++retry)
            {
                bool all_resumed = true;
                for (size_t i = 0; i < record_count_; ++i)
                {
                    if (!records_[i].ignored.load(std::memory_order_acquire)
                        && records_[i].parked.load(std::memory_order_acquire))
                    {
                        all_resumed = false;
                        break;
                    }
                }

                if (all_resumed)
                    break;

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        g_suspend_record_count.store(0, std::memory_order_release);
        g_suspend_records.store(nullptr, std::memory_order_release);
        for (int retry = 0; retry < k_patch_retry_count; ++retry)
        {
            if (g_suspend_handler_active.load(std::memory_order_acquire) == 0)
                break;

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        records_.reset();
        record_count_ = 0;
    }

private:
    std::unique_ptr<linux_thread_record[]> records_{};
    size_t                                 record_count_{0};
};
#endif // defined(CORE_PLATFORM_WINDOWS)

bool wait_for_safe_patch_window(hook_information_t* information)
{
    for (int retry = 0; retry < k_patch_retry_count; ++retry)
    {
        thread_parker parker;
        if (!parker.park())
            return false;

        // The parked threads are intentionally resumed when returning true.
        // entry[0] remains INT3 and g_active_transaction stays active while the remaining bytes are patched.
        if (!parker.has_ip_in_range(information))
            return true;

        parker.resume();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return false;
}

bool write_entry_transaction(hook_information_t* information, const unsigned char* final_bytes)
{
    if (!has_valid_copy_size(information) || final_bytes == nullptr)
        return false;

    if (!ensure_trap_handler_installed())
        return false;

    platform_protection_state protection_state;
    if (!platform_make_writable(information->original_function, static_cast<size_t>(information->bytes_to_copy), &protection_state))
        return false;

    auto* entry = static_cast<unsigned char*>(information->original_function);
    unsigned char rollback_bytes[sizeof(information->original_buffer)];
    std::memcpy(rollback_bytes, entry, static_cast<size_t>(information->bytes_to_copy));

    g_active_transaction.store(information, std::memory_order_release);
    std::atomic_signal_fence(std::memory_order_seq_cst);

    entry[0] = 0xCC;
    platform_flush_instruction_cache(entry, 1);

    if (!wait_for_safe_patch_window(information))
    {
        std::memcpy(entry, rollback_bytes, static_cast<size_t>(information->bytes_to_copy));
        platform_flush_instruction_cache(entry, static_cast<size_t>(information->bytes_to_copy));
        std::atomic_signal_fence(std::memory_order_seq_cst);
        g_active_transaction.store(nullptr, std::memory_order_release);
        platform_restore_protection(information->original_function, static_cast<size_t>(information->bytes_to_copy), protection_state);

        return false;
    }

    if (information->bytes_to_copy > 1)
    {
        std::memcpy(entry + 1, final_bytes + 1, static_cast<size_t>(information->bytes_to_copy - 1));
        platform_flush_instruction_cache(entry + 1, static_cast<size_t>(information->bytes_to_copy - 1));
    }

    entry[0] = final_bytes[0];
    platform_flush_instruction_cache(entry, static_cast<size_t>(information->bytes_to_copy));

    bool patch_window_settled = wait_for_safe_patch_window(information);
    if (!patch_window_settled)
    {
        for (int retry = 0; retry < 3; ++retry)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            patch_window_settled = wait_for_safe_patch_window(information);
            if (patch_window_settled)
                break;
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(patch_window_settled ? k_trap_grace_ms : k_trap_grace_ms * 4));
    std::atomic_signal_fence(std::memory_order_seq_cst);
    g_active_transaction.store(nullptr, std::memory_order_release);
    platform_restore_protection(information->original_function, static_cast<size_t>(information->bytes_to_copy), protection_state);

    return true;
}

bool prepare_patch_size(hook_information_t* information)
{
    if (!information || !information->original_function || !information->target_function)
        return false;

    int copied = 0;
    while (copied < static_cast<int>(k_jump_size))
    {
        if (copied >= static_cast<int>(sizeof(information->original_buffer)))
            return false;

        const instruction_info info = decode_instruction(static_cast<unsigned char*>(information->original_function) + copied);
        if (!info.supported || info.length <= 0 || copied + info.length > static_cast<int>(sizeof(information->original_buffer)))
            return false;

        copied += info.length;
        if (copied < static_cast<int>(k_jump_size) && info.terminal)
            return false;
    }

    information->bytes_to_copy = copied;
    std::memcpy(information->original_buffer, information->original_function, static_cast<size_t>(copied));

    return true;
}

} // namespace

hook_information_t create_hook(void* original_function, void* target_function)
{
    hook_information_t information;
    std::memset(&information, 0, sizeof(information));
    information.original_function = original_function;
    information.target_function   = target_function;

    prepare_patch_size(&information);

    return information;
}

int enable_hook(hook_information_t* information)
{
    std::lock_guard<std::mutex> lock(g_hook_mutex);

    if (!information)
        return 0;

    if (information->enabled)
        return entry_matches_patch(information) ? 1 : 0;

    if (information->bytes_to_copy <= 0 && !prepare_patch_size(information))
        return 0;

    if (!has_valid_copy_size(information) || !entry_matches_original(information))
        return 0;

    const bool needs_trampoline_build = information->trampoline == nullptr;
    if (needs_trampoline_build)
    {
        information->trampoline = platform_allocate_near(information->original_function, k_trampoline_size);
        if (!information->trampoline)
            return 0;

        if (!build_trampoline(information) || !platform_make_executable(information->trampoline, k_trampoline_size))
        {
            platform_free(information->trampoline, k_trampoline_size);
            information->trampoline      = nullptr;
            information->trampoline_size = 0;

            return 0;
        }
    }
    else if (information->trampoline_size <= 0)
    {
        return 0;
    }

    unsigned char patch_bytes[sizeof(information->original_buffer)];
    make_patch_bytes(patch_bytes, information->target_function, information->bytes_to_copy);

    if (!write_entry_transaction(information, patch_bytes))
    {
        if (needs_trampoline_build)
        {
            platform_free(information->trampoline, k_trampoline_size);
            information->trampoline      = nullptr;
            information->trampoline_size = 0;
        }

        return 0;
    }

    information->enabled = 1;

    return 1;
}

int disable_hook(hook_information_t* information)
{
    std::lock_guard<std::mutex> lock(g_hook_mutex);

    if (!information)
        return 0;

    if (!information->enabled)
        return 1;

    if (!has_valid_copy_size(information))
        return 0;

    if (!entry_matches_patch(information))
        return 0;

    if (!write_entry_transaction(information, information->original_buffer))
        return 0;

    // Keep the trampoline mapped after disable. Other threads, or target code
    // already holding the trampoline pointer, may still execute through it.
    information->enabled = 0;

    return 1;
}