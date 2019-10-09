#ifndef COOPERATIVEEDITOR_FILEVISUALIZER_H
#define COOPERATIVEEDITOR_FILEVISUALIZER_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QString>

/**
 * File Visualizer application windows
 */
class FileVisualizer : public QMainWindow {

Q_OBJECT

private:
    QWidget *mainWidget;
    QLabel *title;
    QPushButton *buttonExit;
    QMessageBox *areYouSureQuit;

public:
    ///Constructor
    explicit FileVisualizer(QWidget *parent = nullptr);

public slots:

    ///Slot to notify the visualizer that the list of files has been received
    void onFileListing(std::string files);

    ///Slot to notify the visualizer that the file can/cannot be opened/created
    void onFileResult(bool result);

};

#endif //COOPERATIVEEDITOR_FILEVISUALIZER_H
