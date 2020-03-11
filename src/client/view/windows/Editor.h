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
#include <QListWidget>
#include <QVBoxLayout>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QStandardPaths>
#include <QtPrintSupport/QPrinter>
#include <QTextStream>
#include <QGroupBox>

/**
 * Editor application window
 */
class Editor : public QMainWindow {

Q_OBJECT

private:
    QWidget *mainWidget;
    QTextEdit *textEdit;
    QMenuBar *topBar;
    QToolBar *toolBar;
    QMessageBox *fileCorrectlySaved;
    QMessageBox *editorInfo;
    QMessageBox *infoAboutUs;
    QListWidget *usersOnline;
    QAction *actionBold;
    QAction *actionItalic;
    QAction *actionUnderlined;
    QLabel *usersOnlineDisplayer;
    int usersOnlineNumber;
    QLabel *linkLabel;
    QTextEdit *linkDisplayer;

    void createTopBar(QGridLayout *layout);
    void createToolBar(QGridLayout *layout);
    int getCursorPos();
    void paste();
    bool deleteSelection(); //true = deleted, false = nothing to delete
    void simulateBackspacePression();
    void fileToPDF();
    void mergeFormat(const QTextCharFormat &format);

public:

    explicit Editor(QWidget *parent = nullptr);
    bool eventFilter(QObject *editor, QEvent *event) override ;

public slots:

    ///Slot to notify the editor that a remote op. has been performed
    void onRemoteUpdate(const QString& text);

signals:

    ///Signal emitted when the user wants to edit his profile
    void openEditProfileFromEditor();

    ///Signal emitted when the user wants to go back to the file visualizer
    void openVisualizerFromEditor();

    ///Signal emitted when the user inserts a symbol in the editor
    void symbolInserted(int position, QChar character);

    ///Signal emitted when the user deletes a symbol in the editor
    void symbolDeleted(int position);

};

#endif //COOPERATIVEEDITOR_EDITOR_H
