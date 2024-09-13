#ifndef BHOP_H
#define BHOP_H

#include "../headers/memory.hpp"
#include "../headers/keyboard.hpp"

#include "../headers/offsets.hpp"
#include "../headers/libclient.so.hpp"
#include "../headers/buttons.hpp"

#include <cstdint>

bool bhop(uintptr_t base_address, uintptr_t localPlayer, struct proc_mem& pm) {
    const auto flags = read_memory_from_address<uint32_t>(
        localPlayer + cs2_dumper::schemas::libclient_so::C_BaseEntity::m_fFlags, pm);


    // Get individual flags
    const bool in_air = flags & (1 << 0);
    const bool space_pressed = get_key_state(XKeysymToKeycode(display, XK_space));

    const auto force_jumpAddress = base_address + cs2_dumper::buttons::jump;
    const auto force_jump = read_memory_from_address<uint32_t>(force_jumpAddress, pm);

    if (space_pressed && in_air) {
        uint32_t value = 65537; //needed to assume uint32_t and not int
        write_memory_to_address(force_jumpAddress, value, pm);

        return false;
    }
    else if ((space_pressed && !in_air) || (!space_pressed && force_jump == 65537)) {
        uint32_t value = 256;
        write_memory_to_address(force_jumpAddress, value, pm);

        return true;
    }

    return false;
}

#endif