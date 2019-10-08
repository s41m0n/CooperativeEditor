#ifndef COOPERATIVEEDITOR_EDITOR_H
#define COOPERATIVEEDITOR_EDITOR_H

#include <QMainWindow>

/**
 * Editor application windows
 */
class Editor : public QMainWindow {

Q_OBJECT

public:
    ///Constructor
    explicit Editor(QWidget *parent = nullptr);

public slots:

    ///Slot to notify the editor that the list of files has been received
    void onFileListing(std::string files);

    ///Slot to notify the editor that the file can/cannot be opened/created
    void onFileResult(bool result);

    ///Slot to notify the editor that a remote op. has been performed
    void onRemoteUpdate(std::string text);

};

#endif //COOPERATIVEEDITOR_EDITOR_H
