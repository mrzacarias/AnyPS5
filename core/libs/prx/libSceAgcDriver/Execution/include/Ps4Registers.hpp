#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_PS4REGISTERS_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_PS4REGISTERS_HPP

#include "prx/libSceAgcDriver/Execution/include/QueueState.hpp"
#include <cstdint>

namespace AgcDriver {

// Stores a PS4 (GFX7) context register write in the GFX10 layout the graphics state decodes, with the GFX10
// registers that PS4 spreads over other registers.
void WritePs4ContextRegister(Registers& context, std::uint32_t offset, std::uint32_t value);

}

#endif
