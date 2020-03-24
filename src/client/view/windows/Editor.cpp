#include <spdlog/spdlog.h>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QInputDialog>
#include "Editor.h"

Editor::Editor(QWidget *parent) : QMainWindow(parent), usersOnlineNumber(1) {

  //TODO: fileVisualizer mi passa il nome del file
  //this->setWindowTitle("File Selection");
  //TODO: implementa con segnale + slot l'aggiornamento degli user online (fai setText con nuovo numero utenti connessi) (serve backend)

  this->setMinimumWidth(500);

  mainWidget = new QWidget(this);
  auto layout = new QGridLayout(mainWidget);

  setCentralWidget(mainWidget);
  mainWidget->setLayout(layout);

  createTopBar(layout);

  createToolBar(layout);

  fileCorrectlySaved = new QMessageBox();
  fileCorrectlySaved->setText(
          "File was correctly saved in your home directory");
  fileCorrectlySaved->setStandardButtons(QMessageBox::Close);
  fileCorrectlySaved->setFixedSize(this->minimumSize());

  editorInfo = new QMessageBox();
  editorInfo->setText(
          "Editor totally developed in C++\n\n Copyright fastidioso");
  editorInfo->setStandardButtons(QMessageBox::Close);
  editorInfo->setFixedSize(this->minimumSize());

  infoAboutUs = new QMessageBox();
  infoAboutUs->setText(
          "Authors:\n  "
          "- Francesco Pavan: front-end developer di successo;\n  "
          "- Simone Magnani: vuole le icone tonde perché le ha Google;\n  "
          "- Riccardo Marchi: sa solo tagliare e cucinare cipolle;\n  "
          "- Francesco Palmieri: PETARDO.");
  infoAboutUs->setStandardButtons(QMessageBox::Close);
  infoAboutUs->setFixedSize(this->minimumSize());

  textEdit = new QTextEdit(mainWidget);
  textEdit->installEventFilter(this);
  layout->addWidget(textEdit, 2, 0, 5, 2);

  QObject::connect(textEdit, &QTextEdit::selectionChanged, this,
                   [this]() { //adapt the buttons to the style of the current selected text
                       auto cursor = textEdit->textCursor();
                       cursor.setPosition(
                               textEdit->textCursor().selectionEnd()); //I consider the char at the end of selection
                       actionBold->setChecked(
                               cursor.charFormat().fontWeight() ==
                               QFont::Bold);

                       actionItalic->setChecked(
                               cursor.charFormat().fontItalic());

                       actionUnderlined->setChecked(
                               cursor.charFormat().fontUnderline());
                   });

  QObject::connect(textEdit, &QTextEdit::cursorPositionChanged, this,
                   [this]() { //adapt the buttons to the style of the current position of the cursor
                       actionBold->setChecked(
                               textEdit->textCursor().charFormat().fontWeight() ==
                               QFont::Bold);

                       actionItalic->setChecked(
                               textEdit->textCursor().charFormat().fontItalic());

                       actionUnderlined->setChecked(
                               textEdit->textCursor().charFormat().fontUnderline());
                   });

  usersOnlineDisplayer = new QLabel(
          "Users online: " + QString::number(usersOnlineNumber), mainWidget);
  usersOnlineDisplayer->setFixedHeight(30);
  layout->addWidget(usersOnlineDisplayer, 2, 2, 1, 1);

  usersOnline = new QListWidget(mainWidget);
  usersOnline->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  usersOnline->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  usersOnline->setLayout(new QVBoxLayout);

  //TODO: devi farti inviare i client connessi al file
  usersOnline->addItem("User 1");
  usersOnline->addItem("User 2");

  usersOnline->setFixedHeight(
          60); //TODO: setta a seconda del numero di client 30 * #client
  usersOnline->setFixedWidth(250);

  layout->addWidget(usersOnline, 3, 2, 1, 1);

  linkLabel = new QLabel(
          "Share this link to add contributors:");
  linkLabel->setFixedHeight(30);
  linkLabel->hide();
  layout->addWidget(linkLabel, 4, 2, 1, 1);

  linkDisplayer = new QLineEdit(mainWidget);
  linkDisplayer->setText("----- link -----");
  linkDisplayer->setReadOnly(true);
  linkDisplayer->setFixedWidth(250);
  linkDisplayer->hide();
  layout->addWidget(linkDisplayer, 5, 2, 1, 1);

}

void Editor::onFileTextLoad(const FileText &text) {
  for (Symbol s : text) {
    QTextCharFormat fmt;
    fmt.setFontWeight(s.isAttributeSet(BOLD) ? QFont::Bold : QFont::Normal);
    fmt.setFontItalic(s.isAttributeSet(ITALIC));
    fmt.setFontUnderline(s.isAttributeSet(UNDERLINED));
    textEdit->mergeCurrentCharFormat(fmt);
    textEdit->insertPlainText(s.getChar());
  }
}

void Editor::onRemoteInsert(int index, const QVector<Symbol> &symbol) {
  auto cursor = textEdit->textCursor(); //I retrieve the cursor
  cursor.movePosition(QTextCursor::Start); //I place it at the beginning of the document
  cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, index); //cursor is now where I want to insert the text
  for(Symbol s : symbol){
    QTextCharFormat fmt;
    fmt.setFontWeight(s.isAttributeSet(BOLD) ? QFont::Bold : QFont::Normal);
    fmt.setFontItalic(s.isAttributeSet(ITALIC));
    fmt.setFontUnderline(s.isAttributeSet(UNDERLINED));
    cursor.insertText(s.getChar(), fmt);
  }
}

void Editor::onRemoteDelete(int index, int size) {
  auto cursor = textEdit->textCursor(); //I retrieve the cursor
  cursor.movePosition(QTextCursor::Start); //I place it at the beginning of the document
  cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, index); //cursor is now where I want to remove the text
  for(int i = 0; i < size; i++){
    cursor.deleteChar();
  }
}

void Editor::onRemoteUpdate(int index, const QVector<Symbol> &symbol) {
  auto cursor = textEdit->textCursor(); //I retrieve the cursor
  cursor.movePosition(QTextCursor::Start); //I place it at the beginning of the document
  cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, index); //cursor is now where I want to update the text
  for(Symbol s : symbol){
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
    QTextCharFormat fmt;
    fmt.setFontWeight(s.isAttributeSet(BOLD) ? QFont::Bold : QFont::Normal);
    fmt.setFontItalic(s.isAttributeSet(ITALIC));
    fmt.setFontUnderline(s.isAttributeSet(UNDERLINED));
    cursor.mergeCharFormat(fmt);
  }
}

bool
Editor::eventFilter(QObject *object, QEvent *event) { //key pression manager

  if (object == textEdit && event->type() == QEvent::KeyPress) {

    auto keyEvent = dynamic_cast<QKeyEvent *>(event);
    Qt::KeyboardModifiers modifiers = keyEvent->modifiers();

    if (modifiers & Qt::ControlModifier) { //ctrl is clicked

      switch (keyEvent->key()) {
        case Qt::Key_V: { //paste
          deleteSelection();
          paste();
          break;
        }
        case Qt::Key_X: { //cut
          deleteSelection();
          break;
        }
        case Qt::Key_Z: { //undo
          //(ignored)
          return true;
        }
        case Qt::Key_Backspace: { //delete last word
          QTextCursor cursor = textEdit->textCursor();
          cursor.select(QTextCursor::WordUnderCursor);
          auto selection = cursor.selectedText();

          if (selection ==
              "") { //If the cursor is after one or more space I delete 1 space
            if (getCursorPos() != 0) {
              emit symbolDeleted(getCursorPos() - 1, 1);
              cursor.deletePreviousChar();
              return true;
            }
          } else {
            emit symbolDeleted(cursor.selectionStart(), selection.size());
          }
          break;
        }
          //add desired shortcut here
        default: {
          //all the other shortcuts are handled by the default handler
          return false;
        }
      }
    } else { //ctrl is not clicked

      auto characterInserted = keyEvent->text();

      switch (keyEvent->key()) {
        case Qt::Key_Escape: {
          break;
        }
        case Qt::Key_Delete: {
          if (!deleteSelection()) { //If I have already deleted the selection I don't delete it again
            if (getCursorPos() != textEdit->toPlainText().size()) {
              emit symbolDeleted(getCursorPos(), 1);
            }
          }
          break;
        }
        case Qt::Key_Backspace: {
          if (!deleteSelection()) { //If I have already deleted the selection I don't delete it again
            if (getCursorPos() != 0) {
              emit symbolDeleted(getCursorPos() - 1, 1);
            }
          }
          break;
        }
        default: {
          if (!characterInserted.isEmpty()) {

            simulateBackspacePression();

            QVector<bool> arrayOfStyle = {actionBold->isChecked(),
                                          actionItalic->isChecked(),
                                          actionUnderlined->isChecked()};

            emit symbolInserted(getCursorPos(), {characterInserted.at(
                    0)}, arrayOfStyle);
          }
          break;
        }
      }
    }
  }

  return false;
}

int Editor::getCursorPos() {
  QTextCursor cursorPos = textEdit->textCursor();
  return cursorPos.position();
}

void Editor::paste() {
  auto clipboard = QApplication::clipboard();
  QString selectedText = clipboard->text();

  QVector<bool> arrayOfStyle = {actionBold->isChecked(),
                                actionItalic->isChecked(),
                                actionUnderlined->isChecked()};

  emit symbolInserted(getCursorPos(), selectedText, arrayOfStyle);
}

bool Editor::deleteSelection() {

  QTextCursor textCursor = textEdit->textCursor();
  auto selection = textCursor.selectedText();

  if (!selection.isEmpty()) {
    emit symbolDeleted(textCursor.selectionStart(), selection.size());
    selection.clear();
    return true;
  }

  return false;
}

void Editor::simulateBackspacePression() {
  QTextCursor textCursor = textEdit->textCursor();
  auto selection = textCursor.selectedText();

  if (!selection.isEmpty()) { //I need this trick because the delete selection function does not keep the right position of the cursor
    QKeyEvent pressEvent = QKeyEvent(QEvent::KeyPress,
                                     Qt::Key_Backspace,
                                     Qt::NoModifier);
    QKeyEvent releaseEvent = QKeyEvent(QEvent::KeyRelease,
                                       Qt::Key_Backspace,
                                       Qt::NoModifier);
    QCoreApplication::sendEvent(textEdit, &pressEvent);
    QCoreApplication::sendEvent(textEdit, &releaseEvent);
  }
}

void Editor::createTopBar(QGridLayout *layout) {

  topBar = new QMenuBar(mainWidget);

  auto file = new QMenu("File", topBar);
  topBar->addMenu(file);

  auto actionSave = new QAction("Save As PDF", file);
  QObject::connect(actionSave, &QAction::triggered, this,
                   [this]() {
                       fileToPDF();
                       int result = fileCorrectlySaved->exec();

                       switch (result) {
                         case QMessageBox::Close:
                           editorInfo->close();
                           break;
                         default:
                           //error, should never be reached
                           break;
                       }
                   });
  file->addAction(actionSave);

  file->addSeparator();

  auto actionClose = new QAction("Select another file",
                                 file);
  QObject::connect(actionClose, &QAction::triggered, this,
                   [this]() {
                       emit openVisualizerFromEditor();
                       this->hide();
                   });
  file->addAction(actionClose);

  auto actionQuit = new QAction("Quit", file);
  QObject::connect(actionQuit, &QAction::triggered, this,
                   &QMainWindow::close);
  file->addAction(actionQuit);

  topBar->addSeparator();

  auto edit = new QMenu("Edit", topBar);
  topBar->addMenu(edit);

  auto actionCopy = new QAction("Copy", edit);
  actionCopy->setShortcuts(QKeySequence::Copy);
  QObject::connect(actionCopy, &QAction::triggered, this,
                   [this]() {
                       auto event = new QKeyEvent(QEvent::KeyPress, Qt::Key_C,
                                                  Qt::ControlModifier);
                       QCoreApplication::postEvent(textEdit, event);
                   });
  edit->addAction(actionCopy);

  auto actionPaste = new QAction("Paste", edit);
  actionPaste->setShortcuts(QKeySequence::Paste);
  QObject::connect(actionPaste, &QAction::triggered, this,
                   [this]() {
                       auto event = new QKeyEvent(QEvent::KeyPress, Qt::Key_V,
                                                  Qt::ControlModifier);
                       QCoreApplication::postEvent(textEdit, event);
                   });
  edit->addAction(actionPaste);

  auto actionCut = new QAction("Cut", edit);
  actionCut->setShortcuts(QKeySequence::Cut);
  QObject::connect(actionCut, &QAction::triggered, this,
                   [this]() {
                       auto event = new QKeyEvent(QEvent::KeyPress, Qt::Key_X,
                                                  Qt::ControlModifier);
                       QCoreApplication::postEvent(textEdit, event);
                   });
  edit->addAction(actionCut);

  edit->addSeparator();

  auto actionGenerateLink = new QAction("Invite a contributor", edit);
  QObject::connect(actionGenerateLink, &QAction::triggered, this,
                   [this]() {
                       //TODO: genera link + setta text di linkDisplayer
                       linkLabel->show();
                       linkDisplayer->show();
                   });
  edit->addAction(actionGenerateLink);

  edit->addSeparator();

  auto actionEditProfile = new QAction("Edit your profile", edit);
  QObject::connect(actionEditProfile, &QAction::triggered, this,
                   [this]() {
                       emit openEditProfileFromEditor();
                       this->hide();
                   });
  edit->addAction(actionEditProfile);
  topBar->addSeparator();

  auto help = new QMenu("Help", topBar);
  topBar->addMenu(help);

  auto actionAboutEditor = new QAction("About the Editor", help);
  QObject::connect(actionAboutEditor, &QAction::triggered, this,
                   [this]() {
                       int resultExit = editorInfo->exec();

                       switch (resultExit) {
                         case QMessageBox::Close:
                           editorInfo->close();
                           break;
                         default:
                           //error, should never be reached
                           break;
                       }
                   });
  help->addAction(actionAboutEditor);

  help->addSeparator();

  auto actionAboutAuthors = new QAction("About Us", help);
  QObject::connect(actionAboutAuthors, &QAction::triggered, this,
                   [this]() {
                       int result = infoAboutUs->exec();

                       switch (result) {
                         case QMessageBox::Close:
                           infoAboutUs->close();
                           break;
                         default:
                           //error, should never be reached
                           break;
                       }
                   });
  help->addAction(actionAboutAuthors);

  topBar->setFixedHeight(34);

  layout->addWidget(topBar, 0, 0, 1, 2);
}

void Editor::createToolBar(QGridLayout *layout) {

  toolBar = new QToolBar(mainWidget);

  const QIcon boldIcon = QIcon::fromTheme("format-text-bold",
                                          QIcon(":/images/mac/textbold.png"));
  actionBold = toolBar->addAction(boldIcon, tr("&Bold"), this,
                                  &Editor::textBold);
  QFont bold;
  bold.setBold(true);
  actionBold->setFont(bold);
  actionBold->setCheckable(true);

  toolBar->addSeparator();

  const QIcon italicIcon = QIcon::fromTheme("format-text-italic",
                                            QIcon(":/images/mac/textitalic.png"));
  actionItalic = toolBar->addAction(italicIcon, tr("&Italic"), this,
                                    &Editor::textItalic);
  QFont italic;
  italic.setItalic(true);
  actionItalic->setFont(italic);
  actionItalic->setCheckable(true);

  toolBar->addSeparator();

  const QIcon underlineIcon = QIcon::fromTheme("format-text-underline",
                                               QIcon(":images/mac/textunder.png"));
  actionUnderlined = toolBar->addAction(underlineIcon, tr("&Underline"), this,
                                        &Editor::textUnderlined);
  actionUnderlined->setShortcut(Qt::CTRL + Qt::Key_U);
  QFont underline;
  underline.setUnderline(true);
  actionUnderlined->setFont(underline);
  actionUnderlined->setCheckable(true);

  toolBar->setFixedHeight(20);

  layout->addWidget(toolBar, 1, 0, 1, 2);
}

void Editor::fileToPDF() {

  QString fileName = "Prova"; //TODO: metti nome file vero
  fileName.append(".pdf");

  QPrinter printer(QPrinter::PrinterResolution);
  printer.setOutputFormat(QPrinter::PdfFormat);
  printer.setPaperSize(QPrinter::A4);
  printer.setOutputFileName(fileName);

  QTextDocument doc;
  doc.setPlainText(textEdit->toPlainText());
  doc.setPageSize(printer.pageRect().size());
  doc.print(&printer);
}

void Editor::textBold() {
  QTextCharFormat fmt;
  fmt.setFontWeight(actionBold->isChecked() ? QFont::Bold : QFont::Normal);
  textEdit->mergeCurrentCharFormat(fmt);

  QTextCursor textCursor = textEdit->textCursor();
  auto selection = textCursor.selectedText();

  if (!selection.isEmpty()) {

    QVector<bool> arrayOfStyle = {actionBold->isChecked(),
                                  actionItalic->isChecked(),
                                  actionUnderlined->isChecked()};


    emit symbolUpdated(textCursor.selectionStart(), selection.size(),
                       arrayOfStyle);
  }
}

void Editor::textItalic() {
  QTextCharFormat fmt;
  fmt.setFontItalic(actionItalic->isChecked());
  textEdit->mergeCurrentCharFormat(fmt);

  QTextCursor textCursor = textEdit->textCursor();
  auto selection = textCursor.selectedText();

  if (!selection.isEmpty()) {

    QVector<bool> arrayOfStyle = {actionBold->isChecked(),
                                  actionItalic->isChecked(),
                                  actionUnderlined->isChecked()};

    emit symbolUpdated(textCursor.selectionStart(), selection.size(),
                       arrayOfStyle);
  }
}

void Editor::textUnderlined() {
  QTextCharFormat fmt;
  fmt.setFontUnderline(actionUnderlined->isChecked());
  textEdit->mergeCurrentCharFormat(fmt);

  QTextCursor textCursor = textEdit->textCursor();
  auto selection = textCursor.selectedText();

  if (!selection.isEmpty()) {

    QVector<bool> arrayOfStyle = {actionBold->isChecked(),
                                  actionItalic->isChecked(),
                                  actionUnderlined->isChecked()};

    emit symbolUpdated(textCursor.selectionStart(), selection.size(),
                       arrayOfStyle);
  }
}
