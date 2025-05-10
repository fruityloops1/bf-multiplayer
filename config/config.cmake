set(LINKFLAGS -nodefaultlibs)
set(LLDFLAGS --no-demangle --gc-sections)

set(OPTIMIZE_OPTIONS_DEBUG -O2 -gdwarf-4)
set(OPTIMIZE_OPTIONS_RELEASE -O3 -ffast-math -flto)
set(WARN_OPTIONS -Werror=return-type -Wno-invalid-offsetof)

set(INCLUDES include include/agl)
set(DEFINITIONS DEBUGMENU private=public protected=public)

set(ASM_OPTIONS "")
set(C_OPTIONS -ffunction-sections -fdata-sections)
set(CXX_OPTIONS "")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(IS_32_BIT FALSE)
set(TARGET_IS_STATIC FALSE)
set(MODULE_NAME BowsersFuryMultiplayer)
set(TITLE_ID 0x010028600EBDA000)
set(MODULE_BINARY subsdk4)
set(SDK_PAST_1900 FALSE)
set(USE_SAIL TRUE)

set(TRAMPOLINE_POOL_SIZE 0x40)
set(BAKE_SYMBOLS FALSE)

set(HAKKUN_ADDONS HeapSourceDynamic LogManager)
set(HAKKUN_BSS_HEAP_SIZE 0x100000)
