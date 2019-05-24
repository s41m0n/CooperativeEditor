//
// Created by s41m0n on 16/05/19.
//

#include "Symbol.h"

Symbol::Symbol(char character, int siteId, int digit, std::vector<int> &position)
        : position(std::move(position)), id(siteId, digit), character(character){}

Symbol::Symbol() : character('\0'), id(), position(std::vector<int>()){}

const char Symbol::getChar() {
    return this->character;
}

const std::vector<int> Symbol::getPos() {
    return this->position;
}

