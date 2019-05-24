//
// Created by s41m0n on 17/05/19.
//

#include "Identifier.h"

Identifier::Identifier(unsigned int editorId, unsigned int digit) : editorId(editorId), digit(digit) {}

Identifier::Identifier() : editorId(-1), digit(-1){
}
