/*
* File: Registry.h
* Purpose: a helper class for all functionality related to storing application settings system-wide (Win32 registry).
*/
#include "Generic.h"

class Registry
{
private:
    std::string         AppName;            //  An application name.
    std::string         DeveloperName;      //  A developer name (MySampleOrg).

public:
    Registry() = default;
    ~Registry() = default;
};