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
#include <QTextDocumentFragment>
#include <QTextStream>
#include <QToolBar>
#include <QVBoxLayout>
#include <QtPrintSupport/QPrinter>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSpinBox>
#include <src/include/lib/spdlog/spdlog.h>

#include <src/common/File.h>
#include <src/common/User.h>
#include <src/components/Symbol.h>

#define DEFAULT_FONT_SIZE 12.0
#define MAX_FONT_SIZE 80.0
#define MIN_FONT_SIZE 1.0
#define DEFAULT_FONT_FAMILY "Noto Sans"
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
  QAction *actionBold{};
  QAction *actionItalic{};
  QAction *actionUnderlined{};
  QPushButton *actionColorText{};
  QPushButton *actionColorBackground{};
  QLabel *usersOnlineDisplayer;
  QString fileName;
  QMap<qint32, QString> usersOnlineList;
  QFontComboBox *font{};
  QSpinBox *fontSize{};
  bool isHandlingRemote{};
  std::map<quint32, std::pair<QString, QLabel*>> clientColorCursor;

  /**
   * Function to handle topbar creation
   * @param layout  the layour where it has to be put
   */
  void createTopBar(QGridLayout *layout);

  /**
   * Function to handle toolbar creation
   * @param layout  the layout where it has to be put
   */
  void createToolBar(QGridLayout *layout);

  /**
   * Function to handle save as PDF
   */
  void fileToPDF();

  /**
   * Function to refresh user online list
   */
  void refreshOnlineUsersView();

public:
  explicit Editor(QWidget *parent = nullptr);

private slots:
  /**
   * Slot to handle content changed
   * @param pos the position where the chages applies
   * @param del  the number of deleted characters
   * @param add  the nyumber of inserted characters
   */
  void onContentChanged(int pos, int del, int add);

  /**
   * Slot to handle that an action has been clicked
   */
  void onActionClicked();

  /**
   * Slot to handle current font family changed
   * @param font  the new font family
   */
  void onFontFamilyChanged(const QFont &font);

  /**
   * Slot to handle current font size changed
   * @param newSize  the new size
   */
  void onFontSizeChanged(int newSize);

  /**
   * Slot to handle current foreground changed
   * @param color  the new color
   */
  void onColorForegroundChanged(const QColor &color);

  /**
   * Slot to handle current background color changed
   * @param color  the new color
   */
  void onColorBackgroundChanged(const QColor &color);

  /**
   * Slot to handle current char format changed
   * @param f  the new format
   */
  void onCharFormatChanged(const QTextCharFormat &f);

public slots:

  /**
   * Slot to handle file text loaded
   * @param text
   * @param fileName
   * @param username
   * @param editorId
   */
  void onFileTextLoad(FileText &text, QString &fileName, QString &username,
                      unsigned int editorId);

  /**
   * Slot to handle remote user insert
   * @param index  the index of the inserted symbol
   * @param s the inserted symbol
   */
  void onRemoteInsert(int index, Symbol &s);

  /**
   * Slot to handle remote user erase
   * @param index the index of the erased symbol
   */
  void onRemoteErase(int index);

  /**
   * Slot to handle remote user connect
   * @param clientId remote user id
   * @param username  remote username
   */
  void onRemoteUserConnected(qint32 clientId, const QString &username);

  /**
   * Slot to handle remote user disconnect
   * @param clientId
   */
  void onRemoteUserDisconnected(qint32 clientId);

  /**
   * Slot to handle edit profile closure
   */
  void onComeBackFromEditProfileNoChanges();

  /**
   * Slot to handle generate link answer
   * @param code  the invite code
   */
  void onGenerateLinkAnswer(const QString &code);

  /**
   * Slot to handle remote cursor position changed
   * @param clientId  the remote clientId
   * @param position  the remote cursor new position
   */
  void onUserCursorChanged(quint32 clientId, int position);

signals:

  /**
   * Signal to notify edit profile
   */
  void openEditProfileFromEditor();

  /**
   * Signal to notify to open file visualizer
   */
  void openVisualizerFromEditor();

  /**
   * Signal to notify local insert
   * @param position  the position of the symbol inserted
   * @param character  the value of the symbol inserted
   * @param format  the current format
   */
  void symbolInserted(int position, QChar character, QTextCharFormat format);

  /**
   * Signal to notify local erase
   * @param position  the position of the erased symbol
   */
  void symbolDeleted(int position);

  /**
   * Signal to notify file has been closed
   */
  void fileClosed();

  /**
   * Signal to notify generate link action
   */
  void generateLink();

  /**
   * Signal to notify local cursor position changed
   * @param position  the new position
   */
  void cursorChanged(int position);
};

#endif // COOPERATIVEEDITOR_EDITOR_H
