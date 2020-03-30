#ifndef COOPERATIVEEDITOR_CRDTALGORITHM_H
#define COOPERATIVEEDITOR_CRDTALGORITHM_H

#include <QVector>

#include "components/Symbol.h"

namespace CrdtAlgorithm {

/// Enumeration that defines the different position generation strategy
enum class Strategy { PLUS, MINUS, RANDOM, ALTERNATE };

/// Static variable to space between the position generation
const static int base = 32;
const static int boundary = 10;
const static Strategy strategy = Strategy::RANDOM;

/// True means '+', false means '-'
bool retrieveStrategy(int level);

/// Method to generate and ID between the two given
unsigned int generateIdBetween(unsigned int id1, unsigned int id2,
                               bool boundaryStrategy);

/// Method to find the position of the symbol to erase
int findPositionErase(Symbol &s, FileText &symbols);

/// Method to find the position of the symbol to insert
std::pair<int, int> findPositionInsert(Symbol &s, FileText &symbols);

/// Method to generate a position between two symbols
QVector<Identifier> generatePosBetween(QVector<Identifier> pos1,
                                       QVector<Identifier> pos2,
                                       unsigned int editorId,
                                       QVector<Identifier> newPos = {},
                                       int level = 0);

/// Method to update the list of symbol after a remote insertion
void remoteInsert(Symbol &s, FileText &symbols);

/// Method to update the list of symbol after a remote deletion
void remoteErase(Symbol &s, FileText &symbols);

int replaceSymbol(Symbol &s, FileText &symbols);

int findIndexInLine(Symbol &s, QVector<Symbol> &line, FileText &symbols);

int findInsertIndexInLine(Symbol &s, QVector<Symbol> &line, FileText &symbols);

std::pair<int,int> findEndPosition(Symbol &lastChar, QVector<Symbol> &lastLine, int totalLines);

void insertSymbol(Symbol &symbol, int line, int index, FileText &symbols);

std::pair<int,int> findPosition(Symbol &s, FileText &symbols);

void mergeLines(int fromLine, FileText &symbols);

void removeEmptyLines(FileText &symbols);

} // namespace CrdtAlgorithm

#endif // COOPERATIVEEDITOR_CRDTALGORITHM_H
