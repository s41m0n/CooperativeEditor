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

  textEdit = new QTextEdit(mainWidget);
  textEdit->installEventFilter(this);
  layout->addWidget(textEdit);

}

void Editor::onRemoteUpdate(const QString &text) {
  textEdit->setText(text);
}

bool
Editor::eventFilter(QObject *object, QEvent *event) { //key pression management

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
          //undo (now ignored)
          return true;
        }
        case Qt::Key_Backspace: { //delete last word
          QTextCursor cursor = textEdit->textCursor();
          cursor.select(QTextCursor::WordUnderCursor);
          auto selection = cursor.selectedText();

          if(selection == ""){ //If the selection is empty (the cursor is after one or more space I don't delete anything)
            return true;
          }

          for (int i = 0; i < selection.size(); i++) {
            emit symbolDeleted(cursor.selectionStart());
          }

          break;
        }
          //we can add all the shortcuts we want here
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

          if(!deleteSelection()) { //If I already deleted the selection I don't delete again
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
  auto actionSave = new QAction("Save As...", file);
  file->addAction(actionSave);
  file->addSeparator();
  auto actionClose = new QAction("Close",
                                 file); //torno al file visualizer
  file->addAction(actionClose);
  auto actionQuit = new QAction("Quit", file); //chiudo tutto
  file->addAction(actionQuit);
  topBar->addSeparator();

  auto edit = new QMenu("Edit", topBar);
  topBar->addMenu(edit);
  auto actionCopy = new QAction("Copy", edit);
  edit->addAction(actionCopy);
  auto actionPaste = new QAction("Paste", edit);
  edit->addAction(actionPaste);
  auto actionCut = new QAction("Cut", edit);
  edit->addAction(actionCut);
  edit->addSeparator();
  auto actionEditProfile = new QAction("Edit your profile", edit);
  edit->addAction(actionEditProfile);
  topBar->addSeparator();

  auto help = new QMenu("Help", topBar);
  topBar->addMenu(help);
  auto actionAboutEditor = new QAction("About the Editor", help);
  help->addAction(actionAboutEditor);
  help->addSeparator();
  auto actionAboutAuthors = new QAction("About Us", help);
  help->addAction(actionAboutAuthors);

  layout->addWidget(topBar);

  //TODO: aggiungere tutte le connect con le varie azioni, come quella qui sotto

  QObject::connect(actionEditProfile, &QAction::triggered, this,
                   [this]() {
                       emit openEditProfileFromEditor(); //versione non definitva mancano dati utente (crea classe utente e fatti tornare dati al login)
                       this->close();
                   });
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

