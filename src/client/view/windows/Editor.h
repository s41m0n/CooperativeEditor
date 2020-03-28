#ifndef COOPERATIVEEDITOR_EDITOR_H
#define COOPERATIVEEDITOR_EDITOR_H

#include <QMainWindow>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QListWidget>
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
#include <QCloseEvent>
#include <QInputDialog>
#include <src/components/Symbol.h>
#include <src/common/File.h>
#include <src/common/User.h>

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
    QListWidget *usersOnline;
    QAction *actionBold;
    QAction *actionItalic;
    QAction *actionUnderlined;
    QLabel *usersOnlineDisplayer;
    QLabel *linkLabel;
    QLineEdit *linkDisplayer;
    QString fileName;
    QMap<qint32, QString> usersOnlineList;
    qint32 clientId; //local clientID, useful to manage connections and deconnections of the same user

    void createTopBar(QGridLayout *layout);

    void createToolBar(QGridLayout *layout);

    int getCursorPos();

    void paste();

    bool deleteSelection(); //true = deleted, false = nothing to delete
    void simulateBackspacePression();

    void fileToPDF();

    void textBold();

    void textItalic();

    void textUnderlined();

    void refreshUserView();

public:

    explicit Editor(QWidget *parent = nullptr);

    bool eventFilter(QObject *editor, QEvent *event) override;

public slots:

    ///Slot to open the selected file in the editor the first time
    void onFileTextLoad(const FileText &text, const QString fileName,
                        const QString username, unsigned int editorId);

    ///Slot to notify the editor that a remote user has inserted a character
    void onRemoteInsert(int index, const QVector<Symbol> &symbol);

    ///Slot to notify the editor that a remote user has deleted a character
    void onRemoteDelete(int index, int size);

    ///Slot to notify the editor that a remote user has updated a character
    void onRemoteUpdate(int index, const QVector<Symbol> &symbol);

    void onRemoteUserConnected(qint32 clientId, const QString &username);

    void onRemoteUserDisconnected(qint32 clientId);

    void onComeBackFromEditProfileNoChanges();

signals:

    ///Signal emitted when the user wants to edit his profile
    void openEditProfileFromEditor();

    ///Signal emitted when the user wants to go back to the file visualizer
    void openVisualizerFromEditor();

    ///Signal emitted when the user inserts a symbol in the editor
    void symbolInserted(int position, QString character,
                        QVector<bool> attributes);

    ///Signal emitted when the user deletes a symbol in the editor
    void symbolDeleted(int position, int size);

    ///Signal emitted when the user updates a symbol in the editor
    void
    symbolUpdated(int position, int size, Attribute attribute, bool set);

    ///Signal emitted to inform the server the client has closed the file
    void fileClosed();

};

#endif //COOPERATIVEEDITOR_EDITOR_H
