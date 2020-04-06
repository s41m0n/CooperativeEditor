#ifndef COOPERATIVEEDITOR_SYMBOL_H
#define COOPERATIVEEDITOR_SYMBOL_H

#include <QVector>
#include <QTextCharFormat>

#include "Identifier.h"
#include "src/include/lib/serialization/Serializable.h"

enum Attribute { BOLD, ITALIC, UNDERLINED, ATTRIBUTE_SIZE };

/**
 * Symbol class, to identify each inserted character
 *
 */
class Symbol : public Serializable {

private:
  QChar character;

  unsigned siteId;

  QVector<Identifier> position;

  QVector<bool> attributes;

public:
  Symbol(QChar character, unsigned siteId, QVector<Identifier> &position);

  Symbol();

  int compareTo(const Symbol &s);

  QTextCharFormat format;

  QChar &getChar();

  QVector<Identifier> &getPos();

  std::string toStdString(int level = 0) override;

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;

  void setFormat(QTextCharFormat format);

  QTextCharFormat &getFormat();
};

typedef QVector<Symbol> FileText;

Q_DECLARE_METATYPE(FileText);
Q_DECLARE_METATYPE(Attribute);

#endif // COOPERATIVEEDITOR_SYMBOL_H
