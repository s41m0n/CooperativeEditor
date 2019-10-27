#include <spdlog/spdlog.h>
#include "Editor.h"

Editor::Editor(QWidget *parent) : QMainWindow(parent) {

  //TODO: fileVisualizer mi passa il nome del file
  //this->setWindowTitle("File Selection");

  mainWidget = new QWidget(this);
  auto layout = new QVBoxLayout(mainWidget);

  setCentralWidget(mainWidget);
  mainWidget->setLayout(layout);

  createTopBar(layout);

  createToolBar(layout);

  fileCorrectlySaved = new QMessageBox();
  fileCorrectlySaved->setText("File was correctly saved in your home directory");
  fileCorrectlySaved->setStandardButtons(QMessageBox::Close);
  fileCorrectlySaved->setFixedSize(this->minimumSize());

  editorInfo = new QMessageBox();
  editorInfo->setText("Editor totally developed in C++\n\n Copyright coglione");
  editorInfo->setStandardButtons(QMessageBox::Close);
  editorInfo->setFixedSize(this->minimumSize());

  infoAboutUs = new QMessageBox();
  infoAboutUs->setText(
          "Authors:\n  "
          "- Francesco Pavan: front-end developer di successo;\n  "
          "- Simone Magnani: il suo refactoring non va;\n  "
          "- Riccardo Marchi: non fa un commit dal '62;\n  "
          "- Francesco Palmieri: PETARDO.");
  infoAboutUs->setStandardButtons(QMessageBox::Close);
  infoAboutUs->setFixedSize(this->minimumSize());

  textEdit = new QTextEdit(mainWidget);
  textEdit->installEventFilter(this);
  layout->addWidget(textEdit);

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
          if (getCursorPos() != textEdit->toPlainText().size()) {
            emit symbolDeleted(getCursorPos());
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
          if (!characterInserted.isEmpty()) {
            deleteSelection();
            emit symbolInserted(getCursorPos(), characterInserted.at(0));
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

  for (auto i : selectedText) {
    emit symbolInserted(getCursorPos(), i);
  }
}

bool Editor::deleteSelection() {
  QTextCursor textCursor = textEdit->textCursor();
  auto selection = textCursor.selectedText();

  if (!selection.isEmpty()) {
    for (int i = 0; i < selection.size(); i++) {
      emit symbolDeleted(textCursor.selectionStart());
    }
    return true;
  }

  return false;
}

void Editor::createTopBar(QVBoxLayout *layout) {

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
                       this->close();
                   });
  file->addAction(actionClose);

  auto actionQuit = new QAction("Quit", file);
  QObject::connect(actionQuit, &QAction::triggered, this,
                   [this]() {
                       this->close();
                   });
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
                       emit openEditProfileFromEditor(); //TODO:versione non definitva mancano dati utente (crea classe utente e fatti tornare dati al login)
                       this->close();
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

  layout->addWidget(topBar);
}

void Editor::createToolBar(QVBoxLayout *layout) {

  toolBar = new QToolBar(mainWidget);

  auto actionBold = new QAction("Bold", toolBar);
  toolBar->addAction(actionBold);
  toolBar->addSeparator();

  auto actionItalic = new QAction("Italic", toolBar);
  toolBar->addAction(actionItalic);
  toolBar->addSeparator();

  auto actionUnderlined = new QAction("Underlined", toolBar);
  toolBar->addAction(actionUnderlined);

  layout->addWidget(toolBar);

  //TODO: aggiungere tutte le connect con le varie azioni dei pulsanti
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

