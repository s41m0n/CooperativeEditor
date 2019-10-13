#ifndef COOPERATIVEEDITOR_FILEVISUALIZER_H
#define COOPERATIVEEDITOR_FILEVISUALIZER_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QGroupBox>
#include <QString>
#include <QVBoxLayout>

/**
 * File Visualizer application window
 */
class FileVisualizer : public QMainWindow {

Q_OBJECT

private:
    QWidget *mainWidget;
    QLabel *titleOpen;
    QLabel *titleCreate;
    QPushButton *buttonExit;
    QPushButton *buttonCreate;
    QMessageBox *areYouSureQuit;
    QGroupBox *filesBox;
    QMessageBox *fileCannotBeOpened;

public:
    ///Constructor
    explicit FileVisualizer(QWidget *parent = nullptr);

public slots:

    ///Slot to notify the editor that the list of files has been received
    void onFileListing(const std::vector<std::string>& filesArray);

    ///Slot to notify the visualizer that the file can/cannot be opened/created
    void onFileResult(bool result);

};

#endif //COOPERATIVEEDITOR_FILEVISUALIZER_H
