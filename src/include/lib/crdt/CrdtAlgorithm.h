#ifndef COOPERATIVEEDITOR_CRDTALGORITHM_H
#define COOPERATIVEEDITOR_CRDTALGORITHM_H

#include <cmath>
#include <random>
#include <QVector>

#include "components/Symbol.h"

namespace CrdtAlgorithm {

    ///Enumeration that defines the different position generation strategy
    enum class Strategy {
        PLUS, MINUS, RANDOM, EVERY2ND, EVERY3RD, LOW_LEVEL_1ST
    };

    ///Static variable to space between the position generation
    const static int base = 32;
    const static int boundary = 10;
    const static Strategy strategy = Strategy::LOW_LEVEL_1ST;
    static std::vector<bool> cache;

    ///True means '+', false means '-'
    bool retrieveStrategy(int level);

    ///Method to generate and ID between the two given
    unsigned int generateIdBetween(unsigned int id1, unsigned int id2,
                                   bool boundaryStrategy);

    ///Method to find the position of the symbol to erase
    int findPositionErase(Symbol &s, FileText &symbols);

    ///Method to find the position of the symbol to insert
    int findPositionInsert(Symbol &s, FileText &symbols);

    ///Method to generate a position between two symbols
    QVector<Identifier> generatePosBetween(QVector<Identifier> pos1,
                                           QVector<Identifier> pos2,
                                           unsigned int editorId,
                                           QVector<Identifier> newPos = {},
                                           int level = 0);

    ///Method to update the list of symbol after a remote insertion
    int remoteInsert(Symbol &toInsert, FileText &symbols);

    ///Method to update the list of symbol after a remote deletion
    int remoteErase(Symbol &toErase, FileText &symbols);
}

#endif //COOPERATIVEEDITOR_CRDTALGORITHM_H
