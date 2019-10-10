#ifndef COOPERATIVEEDITOR_CRDTALGORITHM_H
#define COOPERATIVEEDITOR_CRDTALGORITHM_H

#include "components/Symbol.h"

namespace CrdtAlgorithm {

    ///Enumeration that defines the different position generation strategy
    enum class Strategy {
        PLUS, MINUS, RANDOM, ALTERNATE
    };

    ///Static variable to space between the position generation
    const static int base = 32;
    const static int boundary = 10;
    const static Strategy strategy = Strategy::RANDOM;

    ///True means '+', false means '-'
    bool retrieveStrategy(int level);

    ///Method to generate and ID between the two given
    unsigned int generateIdBetween(unsigned int id1, unsigned int id2,
            bool boundaryStrategy);

    ///Method to find the position of the symbol to erase
    int findPositionErase(Symbol &s, std::vector<Symbol> &symbols);

    ///Method to find the position of the symbol to insert
    int findPositionInsert(Symbol &s, std::vector<Symbol> &symbols);

    ///Method to generate a position between two symbols
    std::vector<Identifier> generatePosBetween(std::vector<Identifier> pos1,
            std::vector<Identifier> pos2, unsigned int editorId,
            std::vector<Identifier> newPos = {}, int level = 0);

    ///Method to update the list of symbol after a remote insertion
    void remoteInsert(Symbol &s, std::vector<Symbol> &symbols);

    ///Method to update the list of symbol after a remote deletion
    void remoteErase(Symbol &s, std::vector<Symbol> &symbols);

}

#endif //COOPERATIVEEDITOR_CRDTALGORITHM_H
