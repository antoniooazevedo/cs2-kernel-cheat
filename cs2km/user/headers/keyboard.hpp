#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>  
#include <iostream>

static Display* display = XOpenDisplay(nullptr);


bool get_key_state(KeyCode key){
    char keys[32];
    XQueryKeymap(display, keys);

    // Ensure the keycode is within the valid range
    if (key < 8 || key >= 8 * 32) {
        std::cerr << "Keycode out of range" << std::endl;
        return false;
    }

    return keys[key / 8] & (1 << (key % 8));
}

#endif