#include <spdlog/spdlog.h>
#include "Editor.h"

Editor::Editor(QWidget *parent) : QMainWindow(parent) {

  //TODO: fileVisualizer mi passa il nome del file
  //this->setWindowTitle("File Selection");

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
  editorInfo->setText("Editor totally developed in C++\n\n Copyright fastidioso");
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
  layout->addWidget(textEdit, 2, 0, 4, 2);

  QObject::connect(textEdit, &QTextEdit::selectionChanged, this,
                   [this]() { //adapt the buttons to the style of the current selected text
                       actionBold->setChecked(
                               textEdit->textCursor().charFormat().fontWeight() ==
                               QFont::Bold);

                       actionItalic->setChecked(
                               textEdit->textCursor().charFormat().fontItalic());

                       actionUnderlined->setChecked(
                               textEdit->textCursor().charFormat().fontUnderline());
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

  auto users = new QLabel("Users online:");
  users->setFixedHeight(30);
  layout->addWidget(users, 2, 2, 1, 1);

  userOnline = new QGroupBox(mainWidget);
  userOnline->setLayout(new QVBoxLayout);

  //TODO: devi farti inviare i client connessi al file
  auto user1 = new QLabel("User 1");
  userOnline->layout()->addWidget(user1);
  auto user2 = new QLabel("User 2");
  userOnline->layout()->addWidget(user2);

  userOnline->setFixedHeight(70); //TODO: setta a seconda del numero di client
  userOnline->setFixedWidth(250);

  layout->addWidget(userOnline, 3, 2, 1, 1);

}

void Editor::onRemoteUpdate(const QString &text) {
  textEdit->setText(text);
}

bool
Editor::eventFilter(QObject *object, QEvent *event) { //key pression manager

  if (object == textEdit && event->type() == QEvent::KeyPress) {

    auto keyEvent = dynamic_cast<QKeyEvent *>(event);
    Qt::KeyboardModifiers modifiers = keyEvent->modifiers();

    if (modifiers & Qt::ControlModifier) {

      switch (keyEvent->key()) {
        case Qt::Key_V: { //paste
          paste();
          break;
        }
        case Qt::Key_X: { //cut
          deleteSelection();
          break;
        }
        case Qt::Key_Z: {
          //undo (ignored)
          return true;
        }
        case Qt::Key_Backspace: { //delete last word
          QTextCursor cursor = textEdit->textCursor();
          cursor.select(QTextCursor::WordUnderCursor);
          auto selection = cursor.selectedText();

          if (selection ==
              "") { //If the cursor is after one or more space I delete 1 space
            if (getCursorPos() != 0) {
              emit symbolDeleted(getCursorPos() - 1);
              cursor.deletePreviousChar();
              return true;
            }
          } else {
            for (int i = 0; i < selection.size(); i++) {
              emit symbolDeleted(cursor.selectionStart());
            }
          }

          break;
        }
          //add desired shortcut here
        default: {
          //all the other shortcuts are handled by the default handler
          return false;
        }
      }
    } else {

      auto characterInserted = keyEvent->text();

      switch (keyEvent->key()) {
        case Qt::Key_Escape: {
          break;
        }
        case Qt::Key_Delete: {
          if(!deleteSelection()) { //If I already deleted the selection I don't delete again
            if (getCursorPos() != textEdit->toPlainText().size()) {
              emit symbolDeleted(getCursorPos());
            }
          }
          break;
        }
        case Qt::Key_Backspace: {

          if (!deleteSelection()) { //If I already deleted the selection I don't delete again
            if (getCursorPos() != 0) {
              emit symbolDeleted(getCursorPos() - 1);
            }
          }
          break;
        }
        default: {
          if (!characterInserted.isEmpty()){

            simulateBackspacePression();

            QTextCharFormat fmt;
            if (actionBold->isChecked() || actionItalic->isChecked() ||
                actionUnderlined->isChecked()) {
              fmt.setFontWeight(actionBold->isChecked() ? QFont::Bold
                                                        : QFont::Normal); //write in bold
              fmt.setFontItalic(actionItalic->isChecked()); //write in italic
              fmt.setFontUnderline(
                      actionUnderlined->isChecked()); //write underlined
              mergeFormat(fmt);
            }

            emit symbolInserted(getCursorPos(), characterInserted.at(
                    0)); //TODO: cambiare symbol inserted per passare i flag
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

  for (int i = 0; i < selectedText.size(); i++) {
    emit symbolInserted(getCursorPos() + i, selectedText.at(i));
  }
}

bool Editor::deleteSelection() {

  QTextCursor textCursor = textEdit->textCursor();
  auto selection = textCursor.selectedText();

  if (!selection.isEmpty()) {
    for (int i = 0; i < selection.size(); i++) {
      emit symbolDeleted(textCursor.selectionStart());
    }
    selection.clear();
    return true;
  }

  return false;
}

void Editor::simulateBackspacePression(){
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

void Editor::createToolBar(
        QGridLayout *layout) { //TODO: aggiungi emissione segnali per dire al server che un carattere è stato modificato

  toolBar = new QToolBar(mainWidget);

  const QIcon boldIcon = QIcon::fromTheme("format-text-bold",
                                          QIcon(":/images/mac/textbold.png"));
  actionBold = new QAction(boldIcon, "Bold", toolBar);
  actionBold->setCheckable(true);
  QObject::connect(actionBold, &QAction::triggered, this,
                   [this]() {
                       QTextCharFormat format;
                       if (!textEdit->textCursor().selectedText().isEmpty()) { //some text is selected
                         if (textEdit->textCursor().charFormat().fontWeight() ==
                             QFont::Bold) {
                           format.setFontWeight(QFont::Normal);
                           textEdit->textCursor().mergeCharFormat(format);
                           actionBold->setChecked(false);
                         } else {
                           format.setFontWeight(QFont::Bold);
                           textEdit->textCursor().mergeCharFormat(format);
                           actionBold->setChecked(true);
                         }
                       }
                   });
  toolBar->addAction(actionBold);

  toolBar->addSeparator();

  const QIcon italicIcon = QIcon::fromTheme("format-text-italic",
                                            QIcon(":/images/mac/textitalic.png"));
  actionItalic = new QAction(italicIcon, "Italic", toolBar);
  actionItalic->setCheckable(true);
  QObject::connect(actionItalic, &QAction::triggered, this,
                   [this]() {
                       QTextCharFormat format;
                       if (!textEdit->textCursor().selectedText().isEmpty()) { //some text is selected
                         if (textEdit->textCursor().charFormat().fontItalic()) {
                           format.setFontItalic(false);
                           textEdit->textCursor().mergeCharFormat(format);
                           actionItalic->setChecked(false);
                         } else {
                           format.setFontItalic(true);
                           textEdit->textCursor().mergeCharFormat(format);
                           actionItalic->setChecked(true);
                         }
                       }
                   });
  toolBar->addAction(actionItalic);

  toolBar->addSeparator();

  const QIcon underlineIcon = QIcon::fromTheme("format-text-underline",
                                               QIcon(":images/mac/textunder.png"));
  actionUnderlined = new QAction(underlineIcon, "Underlined", toolBar);
  actionUnderlined->setCheckable(true);
  QObject::connect(actionUnderlined, &QAction::triggered, this,
                   [this]() {
                       QTextCharFormat format;
                       if (!textEdit->textCursor().selectedText().isEmpty()) { //some text is selected
                         if (textEdit->textCursor().charFormat().fontUnderline()) {
                           format.setFontUnderline(false);
                           textEdit->textCursor().mergeCharFormat(format);
                           actionUnderlined->setChecked(false);
                         } else {
                           format.setFontUnderline(true);
                           textEdit->textCursor().mergeCharFormat(format);
                           actionUnderlined->setChecked(true);
                         }
                       }
                   });
  toolBar->addAction(actionUnderlined);

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

void Editor::mergeFormat(const QTextCharFormat &format) {
  QTextCursor cursor = textEdit->textCursor();
  cursor.mergeCharFormat(format);
  textEdit->mergeCurrentCharFormat(format);
}
