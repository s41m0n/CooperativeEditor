#ifndef COOPERATIVEEDITOR_FILE_H
#define COOPERATIVEEDITOR_FILE_H

#include <QString>
#include <mutex>

#include <src/components/Symbol.h>
#include "Serializable.h"

class File : public Serializable {

protected:

    QString fileName;

    FileText fileText;

public:

    explicit File(QString name, FileText text = {});

    File() = default;

    QString getFileName();

    FileText &getFileText();

    std::string toStdString(int level) override;

    QDataStream &serialize(QDataStream &stream) const override;

    QDataStream &deserialize(QDataStream &stream) override;

    quint32 objectSize() override;

};

#endif //COOPERATIVEEDITOR_FILE_H
