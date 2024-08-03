#include "common.hpp"
#include "replace.hpp"

#define PATCH_DEFINE_ASM(name, assembly)                                                          \
    asm(".global __asm" #name "\n"                                                                \
        "__asm" #name ":\n" assembly);                                                            \
    asm(".global __asm" #name "end\n"                                                             \
        "__asm" #name "end:\nnop\n");                                                             \
    extern "C" const u32 __asm##name[];                                                           \
    extern "C" const u32 __asm##name##end[];                                                      \
                                                                                                  \
    namespace name {                                                                              \
                                                                                                  \
    static void* OrigData = nullptr;                                                              \
    static uintptr_t& AddrRef()                                                                   \
    {                                                                                             \
        static uintptr_t addr = 0;                                                                \
        return addr;                                                                              \
    }                                                                                             \
                                                                                                  \
    static ALWAYS_INLINE size_t GetAsmSize()                                                      \
    {                                                                                             \
        return ((uintptr_t) & __asm##name##end) - ((uintptr_t) & __asm##name);                    \
    }                                                                                             \
                                                                                                  \
    static ALWAYS_INLINE void Uninstall()                                                         \
    {                                                                                             \
        memcpy((void*)AddrRef(), OrigData, GetAsmSize());                                         \
        free(OrigData);                                                                           \
        OrigData = nullptr;                                                                       \
        AddrRef() = 0;                                                                            \
    }                                                                                             \
                                                                                                  \
    static ALWAYS_INLINE void TryUninstall()                                                      \
    {                                                                                             \
        if (AddrRef() != 0)                                                                       \
            Uninstall();                                                                          \
    }                                                                                             \
                                                                                                  \
    static ALWAYS_INLINE void InstallAtOffset(ptrdiff_t address)                                  \
    {                                                                                             \
        TryUninstall();                                                                           \
        AddrRef() = exl::util::modules::GetTargetOffset(address);                                 \
        OrigData = malloc(GetAsmSize());                                                          \
        memcpy(OrigData, (void*)AddrRef(), GetAsmSize());                                         \
        exl::replace::ReplaceData((uintptr_t)AddrRef(), (uintptr_t) & __asm##name, GetAsmSize()); \
    }                                                                                             \
                                                                                                  \
    template <typename R, typename... A>                                                          \
    static ALWAYS_INLINE void InstallAtFuncPtr(exl::util::GenericFuncPtr<R, A...> ptr)            \
    {                                                                                             \
        TryUninstall();                                                                           \
        AddrRef() = (uintptr_t)ptr;                                                               \
        OrigData = malloc(GetAsmSize());                                                          \
        memcpy(OrigData, (void*)AddrRef(), GetAsmSize());                                         \
        exl::replace::ReplaceData((uintptr_t)AddrRef(), (uintptr_t) & __asm##name, GetAsmSize()); \
    }                                                                                             \
    }
