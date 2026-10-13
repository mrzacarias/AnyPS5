#ifndef CORE_LIBS_PRX_LIBKERNEL_APPMETADATA_INCLUDE_PARAMSFOPARSER_HPP
#define CORE_LIBS_PRX_LIBKERNEL_APPMETADATA_INCLUDE_PARAMSFOPARSER_HPP

#include <filesystem>

#include "prx/libkernel/AppMetadata/include/ParamJsonParser.hpp"

ParsedParamJson parseParamSfo(const std::filesystem::path& paramSfoPath);

#endif
