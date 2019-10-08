#ifndef COOPERATIVEEDITOR_EDITOR_H
#define COOPERATIVEEDITOR_EDITOR_H

#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMainWindow>

#include "client/model/Model.h"

class Editor : public QMainWindow {

    Q_OBJECT

private:

    Model *model;

public:
    explicit Editor(QWidget *parent = nullptr);

public slots:

    void onFileListing(std::string files);

    void onFileResult(bool result);

    void onRemoteUpdate(std::string text);

};

#endif //COOPERATIVEEDITOR_EDITOR_H
