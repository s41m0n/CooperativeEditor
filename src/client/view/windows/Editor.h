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

public slots:

    void onFileListing(std::string files);

};

#endif //COOPERATIVEEDITOR_EDITOR_H
