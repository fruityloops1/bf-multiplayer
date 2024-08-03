#include "pe/Exception.h"
#include "diag/assert.hpp"
#include "hook/trampoline.hpp"
#include "lib/util/modules.hpp"
#include "nn/os.h"
#include "nx/kernel/svc.h"
#include "pe/Util/Log.h"
#include "util/sys/mem_layout.hpp"
#include "util/sys/rw_pages.hpp"
#include <cstdlib>

enum class ExceptionType : u32 {
    Init = 0x000,
    InstructionAbort = 0x100,
    DataAbort = 0x101,
    UnalignedInstruction = 0x102,
    UnalignedData = 0x103,
    UndefinedInstruction = 0x104,
    ExceptionInstruction = 0x105,
    MemorySystemError = 0x106,
    FpuException = 0x200,
    InvalidSystemCall = 0x301,
    SystemCallBreak = 0x302,

    AtmosphereStdAbort = 0xFFE,
};

static const char* getReasonString(ExceptionType type)
{
    switch (type) {
    case ExceptionType::Init:
        return "Init";
    case ExceptionType::InstructionAbort:
        return "InstructionAbort";
    case ExceptionType::DataAbort:
        return "DataAbort";
    case ExceptionType::UnalignedInstruction:
        return "UnalignedInstruction";
    case ExceptionType::UnalignedData:
        return "UnalignedData";
    case ExceptionType::UndefinedInstruction:
        return "UndefinedInstruction";
    case ExceptionType::ExceptionInstruction:
        return "ExceptionInstruction";
    case ExceptionType::MemorySystemError:
        return "MemorySystemError";
    case ExceptionType::FpuException:
        return "FpuException";
    case ExceptionType::InvalidSystemCall:
        return "InvalidSystemCall";
    case ExceptionType::SystemCallBreak:
        return "SystemCallBreak";
    case ExceptionType::AtmosphereStdAbort:
        return "AtmosphereStdAbort";
    default:
        return "Unknown";
    }
}

static void getMemoryPerm(char* out, uintptr_t addr)
{
    MemoryInfo info;
    u32 pageinfo;
    svcQueryMemory(&info, &pageinfo, addr);

    int i = 1;
    if (info.perm & Perm_R)
        out[i++] = 'R';
    if (info.perm & Perm_W)
        out[i++] = 'W';
    if (info.perm & Perm_X)
        out[i++] = 'X';

    if (i == 1)
        out[0] = '\0';
    else {
        out[0] = '[';
        out[i++] = ']';
        out[i++] = '\0';
    }
}

static void userExceptionHandler(nn::os::UserExceptionInfo* info)
{
    auto& rtld = exl::util::GetRtldModuleInfo();
    auto& main = exl::util::GetMainModuleInfo();
    auto& self = exl::util::GetSelfModuleInfo();
    auto& nnsdk = exl::util::GetSdkModuleInfo();

    const struct {
        const exl::util::ModuleInfo& info;
        const char name[8];
    } modules[] { { rtld, "rtld" }, { main, "main" }, { self, "self" }, { nnsdk, "nnsdk" } };

    struct ModuleAddrOffset {
        const char* moduleName;
        uintptr_t offset;
    };

    char memoryPermBuffer[12] { 0 };

    auto getAddrOffset = [&modules](uintptr_t addr) {
        ModuleAddrOffset offset { "?", 0 };
        for (const auto& module : modules)
            if (addr >= module.info.m_Total.m_Start && addr <= module.info.m_Total.GetEnd()) {
                offset.moduleName = module.name;
                offset.offset = addr - module.info.m_Total.m_Start;
                break;
            }

        return offset;
    };

    pe::err("---------------------");
    pe::err("Exception Occurred!!!");
    pe::err("---------------------");
    pe::err("Type: %s", getReasonString(ExceptionType(info->ErrorDescription)));
    pe::err("Modules: ");
    for (auto& module : modules)
        pe::err("\t%s: %.16lx - %.16lx ", module.name, module.info.m_Total.m_Start, module.info.m_Total.GetEnd());

    pe::err("Fault Address: %.16lx ", info->FAR.x);
    pe::err("Registers: ");
    for (int i = 0; i < 29; i++) {
        auto offset = getAddrOffset(info->CpuRegisters[i].x);
        getMemoryPerm(memoryPermBuffer, info->CpuRegisters[i].x);
        if (offset.offset != 0)
            pe::err("\tX[%02d]: %.16lx (%s + 0x%.8lx) %s ", i, info->CpuRegisters[i].x, offset.moduleName, offset.offset, memoryPermBuffer);
        else
            pe::err("\tX[%02d]: %.16lx %s ", i, info->CpuRegisters[i].x, memoryPermBuffer);
    }

    const struct {
        const char name[3];
        uintptr_t value;
    } registers[] {
        { "FP", info->FP.x },
        { "LR", info->LR.x },
        { "SP", info->SP.x },
        { "PC", info->PC.x },
    };

    for (auto& r : registers) {
        auto offset = getAddrOffset(r.value);
        getMemoryPerm(memoryPermBuffer, r.value);
        if (offset.offset != 0)
            pe::err("\t%s:    %.16lx (%s + 0x%.8lx) %s ", r.name, r.value, offset.moduleName, offset.offset, memoryPermBuffer);
        else
            pe::err("\t%s:    %.16lx %s ", r.name, r.value, memoryPermBuffer);
    }
    pe::err("Stack Trace: ");

    uintptr_t* frame = (uintptr_t*)info->FP.x;
    uintptr_t* prevFrame = nullptr;
    int i = 0;
    while (frame != nullptr and prevFrame != frame) {
        prevFrame = frame;
        auto offset = getAddrOffset(frame[1]);
        if (offset.offset != 0)
            pe::err("\tReturnAddress[%02d]: %.16lx (%s + 0x%.8lx) ", i, frame[1], offset.moduleName, offset.offset);
        else
            pe::err("\tReturnAddress[%02d]: %.16lx ", i, frame[1]);
        frame = (uintptr_t*)frame[0];

        i++;
    }
}

HOOK_DEFINE_TRAMPOLINE(AbortImpl) { static void Callback(const nn::Result* result); };

void AbortImpl::Callback(const nn::Result* result)
{
    EXL_ABORT(42, "nnsdk abort module: %d desc: %d", result->GetModule(), result->GetDescription());
}

void pe::initUserExceptionHandler()
{
    constexpr size_t userExceptionHandlerStackSize = 0x1000;
    void* userExceptionHandlerStack = malloc(userExceptionHandlerStackSize);
    nn::os::SetUserExceptionHandler(userExceptionHandler, userExceptionHandlerStack, userExceptionHandlerStackSize, nullptr);
    AbortImpl::InstallAtSymbol("_ZN2nn4diag6detail5AbortEPKNS_6ResultE");
}
