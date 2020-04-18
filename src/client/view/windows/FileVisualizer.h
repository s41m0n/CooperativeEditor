#ifndef COOPERATIVEEDITOR_FILEVISUALIZER_H
#define COOPERATIVEEDITOR_FILEVISUALIZER_H

#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

/**
 * File Visualizer application window
 */
class FileVisualizer : public QMainWindow {

  Q_OBJECT

private:
  QLineEdit *lineLink{};
  QListWidget *list;

protected:
  /**
   * Function to be called when SHOW
   * @param ev
   */
  void showEvent(QShowEvent *ev) override;

public:
  explicit FileVisualizer(QWidget *parent = nullptr);

public slots:

  /**
   * Slot to handle file listing message
   * @param filesArray  the list of available user files
   */
  void onFileListing(const QVector<QString> &filesArray);

signals:

  /**
   * Signal to require file list
   */
  void requestFileList();

  /**
   * Signal to notify file opening/creating
   * @param filename the desired file name
   * @param exists  true if opening, false if creating
   */
  void fileRequest(const QString &filename, bool exists);

  /**
   * Signal to notify invite link insertion
   * @param code  the invite link
   */
  void insertInviteLink(const QString &code);

  /**
   * Signal to notify view that user wants to edit its profile
   */
   void openEditProfile();
};

#endif // COOPERATIVEEDITOR_FILEVISUALIZER_H
