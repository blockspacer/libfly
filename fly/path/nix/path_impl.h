#pragma once

#include <string>

#include "fly/fly.h"
#include "fly/exit_codes.h"

namespace fly {

/**
 * Linux declaration of the PathImpl interface.
 *
 * @author Timothy Flynn (trflynn89@gmail.com)
 * @version January 21, 2017
 */
class PathImpl
{
public:
    static bool MakePath(const std::string &);
    static bool RemovePath(const std::string &);
    static char GetSeparator();
    static std::string GetTempDirectory();
};

}