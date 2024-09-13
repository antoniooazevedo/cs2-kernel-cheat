#ifndef RADAR_H
#define RADAR_H

#include "../headers/memory.hpp"
#include "../headers/keyboard.hpp"

#include "../headers/offsets.hpp"
#include "../headers/libclient.so.hpp"
#include "../headers/buttons.hpp"

bool radar(uintptr_t base_address, uintptr_t localPlayer, struct proc_mem& pm) {
    const auto entityList = read_memory_from_address<uintptr_t>(
        base_address + cs2_dumper::offsets::libclient_so::dwEntityList, pm);
    
    const auto listEntry = read_memory_from_address<uintptr_t>(
        entityList + 0x10, pm);

    //loop through controllers
    for (int i = 0; i <=64; i++){
        uintptr_t controller = read_memory_from_address<uintptr_t>(
            listEntry + i * 0x78, pm);
        if (controller == 0) continue;

        int pawnHandle = read_memory_from_address<int>(
            controller + cs2_dumper::schemas::libclient_so::CCSPlayerController::m_hPlayerPawn, pm);
        if (pawnHandle == 0) continue;

        uintptr_t secondListEntry = read_memory_from_address<uintptr_t>(
            entityList + (0x8 * ((pawnHandle & 0x7FFF) >> 9) + 0x10), pm);

        uintptr_t pawn = read_memory_from_address<uintptr_t>(
            secondListEntry + (0x78 * (pawnHandle & 0x1FF)), pm);

        // write over spotted flag to true

        write_memory_to_address<bool>(
            pawn + 
            cs2_dumper::schemas::libclient_so::C_CSPlayerPawn::m_entitySpottedState +
            cs2_dumper::schemas::libclient_so::EntitySpottedState_t::m_bSpotted, true, pm);

        // check to see if sucessful
        bool spotted = read_memory_from_address<bool>(
            pawn + 
            cs2_dumper::schemas::libclient_so::C_CSPlayerPawn::m_entitySpottedState +
            cs2_dumper::schemas::libclient_so::EntitySpottedState_t::m_bSpotted, pm);
    }

    return false; //always sleep after this hack
}

#endif 