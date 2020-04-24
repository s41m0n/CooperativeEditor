#ifndef COOPERATIVEEDITOR_CRDTALGORITHM_H
#define COOPERATIVEEDITOR_CRDTALGORITHM_H

#include <QVector>
#include <cmath>
#include <random>

#include "components/Symbol.h"

namespace CrdtAlgorithm {

/// Enumeration that defines the different position generation strategy
enum class Strategy { PLUS, MINUS, RANDOM, EVERY2ND, EVERY3RD, LOW_LEVEL_1ST };

/// CRDT base identifier
const static int base = 32;
/// CRDT boundary
const static int boundary = 10;
/// CRDT strategy to select new identifiers
const static Strategy strategy = Strategy::LOW_LEVEL_1ST;
/// CRDT strategies cache per level
static std::vector<bool> cache;

/**
 * Function to retrieve strategy for a level
 * @param level the algorithm depth
 * @return true if Strategy::PLUS, false if Strategy::MINUS
 */
bool retrieveStrategy(qint32 level);

/**
 * Function to generate an ID between two given
 * @param id1 the first given ID
 * @param id2  the second given ID
 * @param boundaryStrategy the strategy to generate the new ID
 * @return the new ID
 */
quint32 generateIdBetween(quint32 id1, quint32 id2, bool boundaryStrategy);

/**
 * Function to find the position of the symbol to be erased
 * @param s the symbol to be erased
 * @param symbols  the content of the file
 * @return the index of the symbol
 */
int findPositionErase(Symbol &s, FileText &symbols);

/**
 * Function to find the position of the symbol to be inserted
 * @param s  the symbol to be inserted
 * @param symbols  the content of the file
 * @return  the index of the new symbol
 */
int findPositionInsert(Symbol &s, FileText &symbols);

/**
 * Function to generate a position between two symbols
 * @param pos1 the position of the first symbol
 * @param pos2  the position of the second symbol
 * @param editorId the editor identifier
 * @param newPos the new position to be calculated
 * @param level  the algorithm depth
 * @return the new position for the symbol
 */
QVector<Identifier> generatePosBetween(QVector<Identifier> pos1,
                                       QVector<Identifier> pos2,
                                       quint32 editorId,
                                       QVector<Identifier> newPos = {},
                                       int level = 0);

/**
 * Function to handle remote insertion
 * @param toInsert  symbol to be inserted
 * @param symbols  file content to be filled
 * @return the index of the symbol inserted
 */
int remoteInsert(Symbol &toInsert, FileText &symbols);

/**
 * Function to handle remote deletion
 * @param toErase  symbol to be erased
 * @param symbols  file content to be modified
 * @return the index of the erased symbol
 */
int remoteErase(Symbol &toErase, FileText &symbols);
} // namespace CrdtAlgorithm

#endif // COOPERATIVEEDITOR_CRDTALGORITHM_H
