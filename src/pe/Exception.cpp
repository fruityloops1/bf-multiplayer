#include "pe/Exception.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include "hk/svc/api.h"
#include "hk/svc/types.h"
#include "nn/os.h"
#include "pe/Util/Log.h"
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
    hk::svc::MemoryInfo info;
    u32 pageinfo;
    hk::svc::QueryMemory(&info, &pageinfo, addr);

    int i = 1;
    if (info.permission & hk::svc::MemoryPermission_Read)
        out[i++] = 'R';
    if (info.permission & hk::svc::MemoryPermission_Write)
        out[i++] = 'W';
    if (info.permission & hk::svc::MemoryPermission_Execute)
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
    struct ModuleAddrOffset {
        const char* moduleName;
        uintptr_t offset;
    };

    char memoryPermBuffer[12] { 0 };

    auto getAddrOffset = [](uintptr_t addr) {
        ModuleAddrOffset offset { "?", 0 };
        for (int i = 0; i < hk::ro::getNumModules(); i++) {
            const auto* module = hk::ro::getModuleByIndex(i);
            if (addr >= module->range().start() && addr <= module->range().end()) {
                offset.moduleName = module->getModuleName();
                offset.offset = addr - module->range().start();
                break;
            }
        }

        return offset;
    };

    pe::err("---------------------");
    pe::err("Exception Occurred!!!");
    pe::err("---------------------");
    pe::err("Type: %s", getReasonString(ExceptionType(info->ErrorDescription)));
    pe::err("Modules: ");

    for (int i = 0; i < hk::ro::getNumModules(); i++) {
        const auto* module = hk::ro::getModuleByIndex(i);
        pe::err("\t%s: %.16lx - %.16lx ", module->getModuleName(), module->range().start(), module->range().end());
    }

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

HkReplace<void, const nn::Result*> abortImpl = hk::hook::replace([](const nn::Result* result) -> void {
    HK_ABORT("nnsdk abort module: %d desc: %d", result->GetModule(), result->GetDescription());
});

void pe::initUserExceptionHandler()
{
    constexpr size_t userExceptionHandlerStackSize = 0x1000;
    void* userExceptionHandlerStack = malloc(userExceptionHandlerStackSize);
    nn::os::SetUserExceptionHandler(userExceptionHandler, userExceptionHandlerStack, userExceptionHandlerStackSize, nullptr);
    abortImpl.installAtSym<"_ZN2nn4diag6detail5AbortEPKNS_6ResultE">();
}
