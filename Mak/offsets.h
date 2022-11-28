#pragma once
#include <Windows.h>

#define OFFSET_UWORLD 0xE0CF488

namespace OFFSETS
{
    uintptr_t Gameinstance = 0x1A8;
    uintptr_t LocalPlayers = 0x38;
    uintptr_t PlayerController = 0x30;
    uintptr_t LocalPawn = 0x328;
    uintptr_t PlayerState = 0x2A8;
    uintptr_t RootComponet = 0x190;
    uintptr_t PersistentLevel = 0x30;
    uintptr_t ActorCount = 0xA0;
    uintptr_t AActor = 0x98;
    uintptr_t CurrentActor = 0x8;
    uintptr_t Mesh = 0x310;
    uintptr_t Revivefromdbnotime = 0x4270;
    uintptr_t TeamId = 0x1098;
    uintptr_t LocalActorPos = 0x128;
    uintptr_t ComponetToWorld = 0x240;
    uintptr_t BoneArray = 0x5A0;
    uintptr_t Velocity = 0x140;
    uintptr_t PawnPrivate = 0x300;
    uintptr_t PlayerArray = 0x2a0;
    uintptr_t relativelocation = 0x2E8;
    uintptr_t LastFireTime = 0xAB0;
    uintptr_t LastFireTimeVerified = 0xAB4;
    uintptr_t RelativeLocation = 0x128;
}