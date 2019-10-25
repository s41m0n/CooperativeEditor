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

void Editor::onRemoteUpdate(const QString& text) {
  textEdit->setText(text);
}

bool Editor::eventFilter(QObject *object, QEvent *event) //TODO: gestisci ctrl+c e ctrl+v
{
  if (object == textEdit && event->type() == QEvent::KeyPress) {
    auto keyEvent = dynamic_cast<QKeyEvent *>(event);
    auto characterInserted = keyEvent->text();
    Qt::KeyboardModifiers modifiers = keyEvent->modifiers();

    if (keyEvent->matches(QKeySequence::Paste)) { //I manage the paste action
      auto clipboard = QApplication::clipboard();
      QString selectedText = clipboard->text();

      for (int i = 0; i < selectedText.size(); i++) {
        emit symbolInserted(getCursorPos() + i, selectedText.at(i));
      }

    } else if (modifiers & Qt::ControlModifier) {
        return false; //I don't emit any signal if I detect ctrl pressure
    } else { //every other key pressure
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
          if (getCursorPos() != 0) {
            emit symbolDeleted(getCursorPos() - 1);
          }
          break;
        }
        default: {
          if (!characterInserted.isEmpty()) {
            emit symbolInserted(getCursorPos(), characterInserted.at(0));
          }
          break;
        }
      }
    }
  }

  return false;
}

int Editor::getCursorPos(){
  QTextCursor cursorPos;
  cursorPos = textEdit->textCursor();
  return cursorPos.position();
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

