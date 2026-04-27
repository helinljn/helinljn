#include "light_hook.h"

#if defined(CORE_PLATFORM_WINDOWS)
    #ifdef _KERNEL_MODE
        #ifndef _EFI
            #include <ntifs.h>
            #include <intrin.h>
        #endif
    #else
        #define WIN32_NO_STATUS
        #define WIN32_LEAN_AND_MEAN
        #include <Windows.h>
        #undef LHCopyMemory
    #endif
#elif defined(CORE_PLATFORM_LINUX)
    #include <assert.h>
    #include <errno.h>
    #include <stdio.h>
    #include <sys/mman.h>
    #include <unistd.h>
#endif // defined(CORE_PLATFORM_WINDOWS)

#ifdef _EFI
    #include <efi.h>
    #include <efilib.h>
#endif

#define HOOK_R (*b >> 4)
#define HOOK_C (*b & 0xF)

/**
 * @brief 可读可写可执行保护标识
 * 在不同平台下会被转换为对应的内存保护常量。
 */
#define PROTECTION_READ_WRITE_EXECUTE 0xfffffffffffe

/**
 * @brief 指令前缀字节表
 */
static const unsigned char prefixes[] = { 0xF0, 0xF2, 0xF3, 0x2E, 0x36, 0x3E, 0x26, 0x64, 0x65, 0x66, 0x67 };

/**
 * @brief 需要解析 ModR/M 的单字节操作码表
 */
static const unsigned char op1_modrm[] = { 0x62, 0x63, 0x69, 0x6B, 0xC0, 0xC1, 0xC4, 0xC5, 0xC6, 0xC7, 0xD0, 0xD1, 0xD2, 0xD3, 0xF6, 0xF7, 0xFE, 0xFF };

/**
 * @brief 带 8 位立即数的单字节操作码表
 */
static const unsigned char op1_imm8[] = { 0x6A, 0x6B, 0x80, 0x82, 0x83, 0xA8, 0xC0, 0xC1, 0xC6, 0xCD, 0xD4, 0xD5, 0xEB };

/**
 * @brief 带 16/32/64 位立即数的单字节操作码表
 */
static const unsigned char op1_imm32[] = { 0x68, 0x69, 0x81, 0xA9, 0xC7, 0xE8, 0xE9 };

/**
 * @brief 需要解析 ModR/M 的双字节操作码表
 */
static const unsigned char op2_modrm[] = { 0x0D, 0xA3, 0xA4, 0xA5, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF };

/**
 * @brief 绝对间接跳转模板
 */
static const unsigned char jump_code[] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

/**
 * @brief 检查给定字节是否存在于缓冲区中
 * @param buffer     输入缓冲区
 * @param max_length 缓冲区长度
 * @param value      待查找字节
 * @return 找到返回非 0，未找到返回 0
 */
static int find_byte(const unsigned char* buffer, const unsigned long long max_length, const unsigned char value)
{
    for (unsigned long long i = 0; i < max_length; i++)
    {
        if (buffer[i] == value)
            return 1;
    }

    return 0;
}

/**
 * @brief 解析 ModR/M 字节并调整指令指针
 * @param buffer         当前指令指针
 * @param address_prefix 是否存在地址长度前缀
 */
static void parse_modrm(unsigned char** buffer, const int address_prefix)
{
    const unsigned char modrm = *++*buffer;

    if (!address_prefix || (address_prefix && **buffer >= 0x40))
    {
        int has_sib = 0;
        if (**buffer < 0xC0 && (**buffer & 0b111) == 0b100 && !address_prefix)
            has_sib = 1, (*buffer)++;

        if (modrm >= 0x40 && modrm <= 0x7F)
            (*buffer)++;
        else if ((modrm <= 0x3F && (modrm & 0b111) == 0b101) || (modrm >= 0x80 && modrm <= 0xBF))
            *buffer += address_prefix ? 2 : 4;
        else if (has_sib && (**buffer & 0b111) == 0b101)
            *buffer += (modrm & 0b01000000) ? 1 : 4;
    }
    else if (address_prefix && modrm == 0x26)
    {
        *buffer += 2;
    }
}

/**
 * @brief 获取 x86_64 指令长度
 * @param address 指令地址
 * @return 指令长度（字节数）
 */
static int get_instruction_size(const void* address)
{
    /*
     * Based on length-disassembler by @Nomade040
     * https://github.com/Nomade040/length-disassembler
     */
    unsigned long long offset = 0;
    int operand_prefix = 0;
    int address_prefix = 0;
    int rex_w = 0;
    unsigned char* b = (unsigned char*)address;

    for (int i = 0; (i < 14 && find_byte(prefixes, sizeof(prefixes), *b)) || HOOK_R == 4; i++, b++)
    {
        if (*b == 0x66)
            operand_prefix = 1;
        else if (*b == 0x67)
            address_prefix = 1;
        else if (HOOK_R == 4 && HOOK_C >= 8)
            rex_w = 1;
    }

    if (*b == 0x0F)
    {
        b++;
        if (*b == 0x38 || *b == 0x3A)
        {
            if (*b++ == 0x3A)
                offset++;

            parse_modrm(&b, address_prefix);
        }
        else
        {
            if (HOOK_R == 8)
                offset += 4;
            else if ((HOOK_R == 7 && HOOK_C < 4) || *b == 0xA4 || *b == 0xC2 || (*b > 0xC3 && *b <= 0xC6) || *b == 0xBA || *b == 0xAC)
                offset++;

            if (find_byte(op2_modrm, sizeof(op2_modrm), *b) || (HOOK_R != 3 && HOOK_R > 0 && HOOK_R < 7) || *b >= 0xD0 || (HOOK_R == 7 && HOOK_C != 7) || HOOK_R == 9 || HOOK_R == 0xB || (HOOK_R == 0xC && HOOK_C < 8) || (HOOK_R == 0 && HOOK_C < 4))
                parse_modrm(&b, address_prefix);
        }
    }
    else
    {
        if ((HOOK_R == 0xE && HOOK_C < 8) || (HOOK_R == 0xB && HOOK_C < 8) || HOOK_R == 7 || (HOOK_R < 4 && (HOOK_C == 4 || HOOK_C == 0xC)) || (*b == 0xF6 && !(*(b + 1) & 48)) || find_byte(op1_imm8, sizeof(op1_imm8), *b))
            offset++;
        else if (*b == 0xC2 || *b == 0xCA)
            offset += 2;
        else if (*b == 0xC8)
            offset += 3;
        else if ((HOOK_R < 4 && (HOOK_C == 5 || HOOK_C == 0xD)) || (HOOK_R == 0xB && HOOK_C >= 8) || (*b == 0xF7 && !(*(b + 1) & 48)) || find_byte(op1_imm32, sizeof(op1_imm32), *b))
        {
            if (*b == 0xB8 || (*b >= 0xB8 && *b <= 0xBF))
                offset += rex_w ? 8 : 4;
            else if (*b == 0xC7)
                offset += 4;
            else if (*b == 0x69)
                offset += 4;
            else
                offset += operand_prefix ? 2 : 4;
        }
        else if (HOOK_R == 0xA && HOOK_C < 4)
        {
            offset += rex_w ? 8 : (address_prefix ? 2 : 4);
        }
        else if (*b == 0xEA || *b == 0x9A)
        {
            offset += operand_prefix ? 4 : 6;
        }

        if (find_byte(op1_modrm, sizeof(op1_modrm), *b) || (HOOK_R < 4 && (HOOK_C < 4 || (HOOK_C >= 8 && HOOK_C < 0xC))) || HOOK_R == 8 || (HOOK_R == 0xD && HOOK_C >= 8))
            parse_modrm(&b, address_prefix);
    }

    return (int)(++b + offset - (unsigned char*)address);
}

/**
 * @brief 简单内存拷贝实现
 * @param destination 目标地址
 * @param source 源地址
 * @param size   拷贝字节数
 */
static void memory_copy(void* destination, void* source, unsigned long long size)
{
    unsigned char* dst = (unsigned char*)destination;
    unsigned char* src = (unsigned char*)source;
    for (unsigned long long i = 0; i < size; i++)
        dst[i] = src[i];
}

/**
 * @brief 分配可执行内存
 * @param size 字节数
 * @return 分配得到的内存地址，失败返回 0
 */
static void* platform_allocate(const unsigned long long size)
{
#ifdef _EFI
    const unsigned long long number_of_pages = 1 + size / 1024;
    EFI_PHYSICAL_ADDRESS physical_address;
    gBS->AllocatePages(AllocateAnyPages, EfiRuntimeServicesCode, number_of_pages, &physical_address);
    return (void*)physical_address;
#endif

#if defined(CORE_PLATFORM_WINDOWS)
    #ifdef _KERNEL_MODE
        #ifndef _EFI
            return ExAllocatePool(NonPagedPoolExecute, size);
        #endif
    #else
        return VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    #endif
#elif defined(CORE_PLATFORM_LINUX)
    return mmap(0, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
#else
    (void)size;
    return 0;
#endif // defined(CORE_PLATFORM_WINDOWS)
}

/**
 * @brief 释放由 platform_allocate 分配的内存
 * @param address 内存地址
 * @param size    字节数
 */
static void platform_free(void* address, const unsigned long long size)
{
#ifdef _EFI
    const unsigned long long number_of_pages = 1 + size / 1024;
    gBS->FreePages((EFI_PHYSICAL_ADDRESS)address, number_of_pages);
    return;
#endif

#if defined(CORE_PLATFORM_WINDOWS)
    #ifdef _KERNEL_MODE
        #ifndef _EFI
            (void)size;
            ExFreePool(address);
        #endif
    #else
        (void)size;
        VirtualFree(address, 0, MEM_RELEASE);
    #endif
#elif defined(CORE_PLATFORM_LINUX)
    munmap(address, size);
#else
    (void)address;
    (void)size;
#endif // defined(CORE_PLATFORM_WINDOWS)
}

/**
 * @brief 修改内存保护属性
 * @param address    内存地址
 * @param size       内存大小
 * @param protection 目标保护属性
 * @return 原始保护值或恢复所需值
 */
static unsigned long long platform_protect(void* address, unsigned long long size, unsigned long long protection)
{
#if defined(CORE_PLATFORM_WINDOWS)
    #ifdef _KERNEL_MODE
        (void)size;
        (void)address;
        if (protection == PROTECTION_READ_WRITE_EXECUTE)
        {
            _disable();

            unsigned long long cr0 = __readcr0();
            unsigned long long original_cr0 = cr0;
            cr0 &= ~(1ULL << 16);
            __writecr0(cr0);

            return original_cr0;
        }
        else
        {
            __writecr0(protection);
            _enable();
            return 0;
        }
    #else
        if (protection == PROTECTION_READ_WRITE_EXECUTE)
            protection = PAGE_EXECUTE_READWRITE;

        unsigned long original = 0;
        VirtualProtect(address, size, (unsigned long)protection, &original);
        return original;
    #endif
#elif defined(CORE_PLATFORM_LINUX)
    (void)size;
    if (protection == PROTECTION_READ_WRITE_EXECUTE)
        protection = PROT_READ | PROT_WRITE | PROT_EXEC;
    else
        protection = PROT_READ | PROT_EXEC;

    int page_size = getpagesize();
    unsigned long long page_offset = (unsigned long long)address % page_size;
    address = (void*)((unsigned long long)address - page_offset);

    int status = mprotect(address, page_size, (int)protection);
    assert(status == 0);
    (void)status;

    return protection;
#else
    (void)address;
    (void)size;
    (void)protection;
    return 0;
#endif // defined(CORE_PLATFORM_WINDOWS)
}

#define create_jump(name, target_address)                            \
    unsigned char name[sizeof(jump_code)];                           \
    memory_copy(name, (void*)jump_code, sizeof(jump_code));          \
    *(unsigned long long*)((unsigned long long)name + 6) = (unsigned long long)(target_address)

hook_information_t create_hook(void* original_function, void* target_function)
{
    hook_information_t information;
    information.enabled = 0;
    information.trampoline = 0;
    information.original_function = original_function;
    information.target_function = target_function;

    int size = 0;
    while (size < (int)sizeof(jump_code))
        size += get_instruction_size((unsigned char*)original_function + size);

    information.bytes_to_copy = size;
    memory_copy(information.original_buffer, original_function, size);

    return information;
}

int enable_hook(hook_information_t* information)
{
    if (information->enabled)
        return 1;

    const int buffer_size = (int)sizeof(jump_code) + information->bytes_to_copy;
    unsigned char* buffer = (unsigned char*)platform_allocate(buffer_size);
    if (!buffer)
        return 0;

    information->trampoline = buffer;
    memory_copy(buffer, information->original_buffer, information->bytes_to_copy);

    create_jump(original_jump, (unsigned char*)information->original_function + information->bytes_to_copy);
    memory_copy(buffer + information->bytes_to_copy, original_jump, sizeof(jump_code));

    create_jump(target_jump, information->target_function);
    unsigned long long original_protection = platform_protect(information->original_function, information->bytes_to_copy, PROTECTION_READ_WRITE_EXECUTE);
    memory_copy(information->original_function, target_jump, sizeof(jump_code));
    platform_protect(information->original_function, information->bytes_to_copy, original_protection);

    information->enabled = 1;
    return 1;
}

int disable_hook(hook_information_t* information)
{
    if (!information->enabled)
        return 1;

    unsigned long long original_protection = platform_protect(information->original_function, information->bytes_to_copy, PROTECTION_READ_WRITE_EXECUTE);
    memory_copy(information->original_function, information->original_buffer, information->bytes_to_copy);
    platform_protect(information->original_function, information->bytes_to_copy, original_protection);

    platform_free(information->trampoline, sizeof(jump_code) + information->bytes_to_copy);

    information->enabled = 0;
    return 1;
}