#ifndef COOPERATIVEEDITOR_FILEVISUALIZER_H
#define COOPERATIVEEDITOR_FILEVISUALIZER_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QGroupBox>
#include <QListWidget>
#include <QString>
#include <QInputDialog>
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
    QGroupBox *boxCreate;
    QMessageBox *fileCannotBeOpened;

public:

    explicit FileVisualizer(QWidget *parent = nullptr);

public slots:

    ///Slot to notify the editor that the list of files has been received
    void onFileListing(const QVector<QString> &filesArray);

    ///Slot to notify the visualizer that the file can/cannot be opened/created
    void onFileResult(bool result);

signals:

  ///True = file exists, False = file does not exist
  void fileRequest(const QString &filename, bool exists);

};

#endif //COOPERATIVEEDITOR_FILEVISUALIZER_H
