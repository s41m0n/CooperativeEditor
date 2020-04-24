#ifndef COOPERATIVEEDITOR_IDENTIFIER_H
#define COOPERATIVEEDITOR_IDENTIFIER_H

#include <src/include/lib/serialization/Serializable.h>
#include <string>

/**
 * Identifier class, used to identify a symbol
 *
 */
class Identifier : public Serializable {

private:
  quint32 editorId;

  quint32 digit;

public:
  Identifier(unsigned int editorId, unsigned int digit);

  Identifier() = default;

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;

  std::string toStdString(int level = 0) override;

  /**
   * Function to compare this identifier with another one
   * @param other the other to be compared to
   * @return 1 if this is greater, -1 if lower, 0 if equal
   */
  int compareTo(const Identifier &other);

  /**
   * Function to get the editor id of this identifier
   * @return the editor ID
   */
  quint32 getEditorId();

  /**
   * Function to get the digit associated with this identifier
   * @return  the digit
   */
  quint32 getDigit();
};

#endif // COOPERATIVEEDITOR_IDENTIFIER_H
