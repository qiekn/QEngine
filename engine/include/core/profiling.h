#pragma once

#ifdef ZPROFILE
#include "tracy/Tracy.hpp"

#define ZPROFILE_ZONE() ZoneScoped
#define ZPROFILE_ZONE_NAMED(name) ZoneScopedN(name)
#define ZPROFILE_FUNCTION() ZoneScoped
#define ZPROFILE_TEXT(text, size) ZoneText(text, size)
#define ZPROFILE_VALUE(value) ZoneValue(value)
#else
#define ZPROFILE_ZONE()
#define ZPROFILE_ZONE_NAMED(name)
#define ZPROFILE_FUNCTION()
#define ZPROFILE_TEXT(text, size)
#define ZPROFILE_VALUE(value)
#endif
