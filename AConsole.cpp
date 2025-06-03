#include "AConsole.h"
#include <iostream>

AConsole::AConsole(String name) : name(std::move(name)) {}

AConsole::String AConsole::getName() const {
    return name;
}