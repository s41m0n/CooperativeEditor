#ifndef COOPERATIVEEDITOR_EDITOR_H
#define COOPERATIVEEDITOR_EDITOR_H

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDir>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStandardPaths>
#include <QString>
#include <QTextStream>
#include <QToolBar>
#include <QVBoxLayout>
#include <QTextDocumentFragment>
#include <QtPrintSupport/QPrinter>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QColorDialog>
#include <src/include/lib/spdlog/spdlog.h>

#include <src/common/File.h>
#include <src/common/User.h>
#include <src/components/Symbol.h>

#define DEFAULT_FONT_SIZE 12.0
#define MAX_FONT_SIZE 80.0
#define MIN_FONT_SIZE 1.0
#define DEFAULT_FONT_FAMILY "Noto Mono"
#define DEFAULT_TEXT_COLOR "white"
#define DEFAULT_BACKGROUND_COLOR "transparent"

/**
 * Editor application window
 */
class Editor : public QMainWindow {

  Q_OBJECT

private:
  QWidget *mainWidget;
  QTextEdit *textEdit;
  QListWidget *usersOnline;
  QAction *actionBold;
  QAction *actionItalic;
  QAction *actionUnderlined;
  QPushButton *actionColorText;
  QPushButton *actionColorBackground;
  QLabel *usersOnlineDisplayer;
  QString fileName;
  QMap<qint32, QString> usersOnlineList;
  QFontComboBox *font;
  QSpinBox *fontSize;
  bool isHandlingRemote;

  void createTopBar(QGridLayout *layout);

  void createToolBar(QGridLayout *layout);

  void fileToPDF();

  void refreshOnlineUsersView();

public:
  explicit Editor(QWidget *parent = nullptr);

private slots:
  void onContentChanged(int pos, int del, int add);
  void onActionClicked();
  void onFontFamilyChanged(const QFont& font);
  void onFontSizeChanged(int newSize);
  void onColorForegroundChanged(const QColor& color);
  void onColorBackgroundChanged(const QColor& color);
  void onCharFormatChanged(const QTextCharFormat &f);

public slots:

  /// Slot to open the selected file in the editor the first time
  void onFileTextLoad(FileText &text, QString &fileName, QString &username,
                      unsigned int editorId);

  /// Slot to notify the editor that a remote user has performed an operation on the content
  void onRemoteInsert(int index, Symbol &s);
  void onRemoteUpdate(int index, Symbol &s);
  void onRemoteErase(int index);

  void onRemoteUserConnected(qint32 clientId, const QString& username);

  void onRemoteUserDisconnected(qint32 clientId);
  void onComeBackFromEditProfileNoChanges();

signals:

  /// Signal emitted when the user wants to edit his profile
  void openEditProfileFromEditor();

  /// Signal emitted when the user wants to go back to the file visualizer
  void openVisualizerFromEditor();

  /// Signal emitted when the user inserts a symbol in the editor
  void symbolInserted(int position, QChar character,
                      QTextCharFormat format);

  /// Signal emitted when the user deletes a symbol in the editor
  void symbolDeleted(int position);

  /// Signal emitted to inform the server the client has closed the file
  void fileClosed();
};

#endif // COOPERATIVEEDITOR_EDITOR_H
