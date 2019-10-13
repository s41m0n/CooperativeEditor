#ifndef COOPERATIVEEDITOR_EDITOR_H
#define COOPERATIVEEDITOR_EDITOR_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QMenuBar>
#include <QToolBar>
#include <QPlainTextEdit>
#include <QString>
#include <QVBoxLayout>
#include <QAction>

/**
 * Editor application window
 */
class Editor : public QMainWindow {

Q_OBJECT

private:
    QWidget *mainWidget;
    QPlainTextEdit *textEdit;
    QMenuBar *topBar;
    QToolBar *toolBar;
    void createTopBar(QVBoxLayout* layout);
    void createToolBar(QVBoxLayout* layout);

public:
    ///Constructor
    explicit Editor(QWidget *parent = nullptr);

public slots:

    ///Slot to notify the editor that a remote op. has been performed
    void onRemoteUpdate(std::string text);

};

#endif //COOPERATIVEEDITOR_EDITOR_H