#include "ScriptFunctions.h"

/// <summary>
/// Used to find an entity by it's name.
/// </summary>
/// <param name="entityName">A name of the entity to be found</param>
/// <returns>Entity handle if found, otherwise "entity_handle_undefined".</returns>
EntityHandle GetEntityByName(StringArgument entityName)
{
    if (entityName.empty())
        return ENTITY_HANDLE_UNDEFINED;

    return ENTITY_HANDLE_UNDEFINED;
}

/// <summary>
/// Used to attach an event handler for specified entity. See "eventsid" enumeration for all possible eventId value.
/// On each game "tick" if such event occurs and this entity is active (enabled), a handler function will be called.
/// </summary>
/// <param name="entityHandle">A handle of the entity, can be obtained with "GetEntityByName". Otherwise, it's just an entity id in scene file.</param>
/// <param name="eventId">Event id that handler function will be attached to.</param>
/// <param name="eventHandler">Function pointer that will be called when event is happened.</param>
/// <returns>If all passed arguments are valid 'true' will be returned. Otherwise - false</returns>
bool AddEvent(EntityHandle entityHandle, eEventId eventId, void (*eventHandler)())
{
    if (!entityHandle || !eventHandler)
        return false;

    if (!CanAttachEvent(entityHandle, eventId))
        return false;

    return false;
}

/// <summary>
/// Explicitly passes control to the event handler for this entity. Handler will be marked as 'pending'
/// and next time all event handlers are checked and called, it will be called.
/// Return value indicates if the handler call was successfull.
/// </summary>
/// <param name="entityHandle"></param>
/// <param name="eventId"></param>
/// <returns></returns>
bool FireEvent(EntityHandle entityHandle, eEventId eventId)
{
    if (!entityHandle)
        return false;
}

/// <summary>
/// Performs 
/// </summary>
/// <param name="entityHandle"></param>
void Unload(EntityHandle entityHandle)
{

}

void FadeOut(IntArgument duration)
{

}

void FadeIn(IntArgument duration)
{

}

ScriptHandle StartScript(StringArgument scriptPath)
{

}