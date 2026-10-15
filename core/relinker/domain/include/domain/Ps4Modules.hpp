#ifndef DOMAIN_PS4MODULES_HPP
#define DOMAIN_PS4MODULES_HPP

#include <array>
#include <cstdint>
#include <string_view>
#include <utility>

namespace Domain {

inline constexpr std::uint32_t PtSceDynlibData = 0x61000000;

inline constexpr std::array<std::pair<std::string_view, std::string_view>, 7> Ps4ProviderModules{{
    {"libSceAjm.prx", "libSceAjm.native.prx"},
    {"libSceGnmDriver_padebug.prx", "libSceGnmDriver.prx"},
    {"libSceSaveData.prx", "libSceSaveData.native.prx"},
    {"libSceSaveDataDialog.prx", "libSceSaveDataDialog.native.prx"},
    {"libSceSsl2.prx", "libSceSsl.prx"},
    {"libSceNpWebApi.prx", "libSceNpWebApi2.prx"},
    {"libSceNpTrophy.prx", "libSceNpTrophy2.prx"},
}};

}

#endif
