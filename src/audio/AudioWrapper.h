/*
* File: Registry.h
* Purpose: a collection of classes for all stuff related to audio engine.
*/
#include "Generic.h"

//  Base class provides all required functionality and engine connections.
//  Derived classes override necessary methods for specific needs.
class AudioWrapper
{
private:
    std::string         SystemName = "Unspecified";
    float_t             Volume;

public:
    AudioWrapper(const std::string& name)
    {}

    void SetVolume(const float_t vol);
    const float_t GetVolume() const;
};

class AudioSDL : public AudioWrapper
{
private:
    
public:
    AudioSDL()
        :AudioWrapper("SDL")
    {}
};

class AudioX : public AudioWrapper
{
private:

public:
    AudioX()
        :AudioWrapper("X")
    {}
};