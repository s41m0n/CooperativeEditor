//
// Created by s41m0n on 17/05/19.
//

#include "Identifier.h"

Identifier::Identifier(unsigned int editorId, unsigned int digit) : editorId(editorId), digit(digit) {}

int Identifier::compareTo(Identifier other) {
    if (this->digit < other.digit) {
        return -1;
    } else if (this->digit > other.digit) {
        return 1;
    } else {
        if (this->editorId < other.editorId) {
            return -1;
        } else if (this->editorId > other.editorId) {
            return 1;
        } else {
            return 0;
        }
    }
}
