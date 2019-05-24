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

int Symbol::compareTo(const Symbol &other) {
    int digit1, digit2;
    std::vector<int> pos1 = this->position;
    std::vector<int> pos2 = other.position;

    for (int i = 0; i < std::min(pos1.size(), pos2.size()); i++) {
        digit1 = pos1[i];
        digit2 = pos2[i];
        if (digit1 != digit2)
            return digit1 > digit2? 1 : -1;
    }

    if (pos1.size() < pos2.size()) {
        return -1;
    } else if (pos1.size() > pos2.size()) {
        return 1;
    } else {
        return this->id.compareTo(other.id);
    }
}
