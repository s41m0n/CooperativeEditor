#ifndef COOPERATIVEEDITOR_SYMBOL_H
#define COOPERATIVEEDITOR_SYMBOL_H

#include <QTextCharFormat>
#include <QVector>

#include "Identifier.h"
#include "src/include/lib/serialization/Serializable.h"

/**
 * Symbol class, to identify each inserted character
 *
 */
class Symbol : public Serializable {

private:
  QChar character;

  quint32 siteId;

  QVector<Identifier> position;

  QTextCharFormat format;

public:
  Symbol(QChar character, quint32 siteId, QVector<Identifier> &position);

  Symbol();

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;

  std::string toStdString(int level = 0) override;

  /**
   * Method to compare two symbols
   * @param s the other one
   * @return 1 if this is greater, -1 if lower, 0 if equal
   */
  int compareTo(const Symbol &s);

  /**
   * Function to get the character representing the symbol
   * @return the character
   */
  QChar &getChar();

  /**
   * Function to get the vector of identifier
   * @return the vector of ID
   */
  QVector<Identifier> &getPos();

  /**
   * Function to set the current symbol format
   * @param format the format to be set
   */
  void setFormat(QTextCharFormat format);

  /**
   * Function to get the symbol format
   * @return  the current format
   */
  QTextCharFormat &getFormat();
};

/*FileText is a portion of text or the entire one*/
typedef QVector<Symbol> FileText;

Q_DECLARE_METATYPE(FileText);

#endif // COOPERATIVEEDITOR_SYMBOL_H
