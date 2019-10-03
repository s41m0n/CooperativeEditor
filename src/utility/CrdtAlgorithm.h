//
// Created by s41m0n on 24/05/19.
//

#ifndef COOPERATIVEEDITOR_CRDTALGORITHM_H
#define COOPERATIVEEDITOR_CRDTALGORITHM_H

#include "components/Symbol.h"

namespace CrdtAlgorithm {
    ///Static variable to space between the position generation
    const static int base = 32;

    ///Method to generate and ID between the two given
    int generateIdBetween(int id1, int id2);

    ///Method to find the position of the symbol to erase
    int findPositionErase(Symbol &s, std::vector<Symbol> &symbols);

    ///Method to find the position of the symbol to insert
    int findPositionInsert(Symbol &s, std::vector<Symbol> &symbols);

    ///Method to generate a position between two symbols
    void generatePosBetween(Symbol *s1, Symbol *s2,
                            std::vector<int> &newPos, int index1 = 0,
                            int level = 0, int index2 = 0);

    ///Method to update the list of symbol after a remote insertion
    void remoteInsert(Symbol &s, std::vector<Symbol> &symbols);

    ///Method to update the list of symbol after a remote deletion
    void remoteErase(Symbol &s, std::vector<Symbol> &symbols);

}

#endif //COOPERATIVEEDITOR_CRDTALGORITHM_H
