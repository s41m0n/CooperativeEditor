#ifndef COOPERATIVEEDITOR_EDITOR_H
#define COOPERATIVEEDITOR_EDITOR_H

#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMainWindow>

class Editor : public QMainWindow {

    Q_OBJECT

private:

public:
    explicit Editor(QWidget *parent = nullptr);

};

#endif //COOPERATIVEEDITOR_EDITOR_H
