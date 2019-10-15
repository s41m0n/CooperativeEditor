#include <spdlog/spdlog.h>
#include "Editor.h"

Editor::Editor(QWidget *parent) : QMainWindow(parent) {

  //this->setWindowTitle("File Selection"); //nome del file

  mainWidget = new QWidget(this);
  auto layout = new QVBoxLayout(mainWidget);

  setCentralWidget(mainWidget);
  mainWidget->setLayout(layout);

  createTopBar(layout);

  createToolBar(layout);

  textEdit = new QPlainTextEdit();
  layout->addWidget(textEdit);

}

void Editor::onRemoteUpdate(QString text) {
  //RIAGGIORNARE LA VIEW CON IL NUOVO TESTO
  spdlog::debug("Un nuovo Crdt message arrivato");

}

void Editor::createTopBar(QVBoxLayout *layout) {

  topBar = new QMenuBar();

  auto file = new QMenu("File");
  topBar->addMenu(file);
  auto actionSave = new QAction("Save As...", mainWidget);
  file->addAction(actionSave);
  file->addSeparator();
  auto actionClose = new QAction("Close",
                                 mainWidget); //torno al file visualizer
  file->addAction(actionClose);
  auto actionQuit = new QAction("Quit", mainWidget); //chiudo tutto
  file->addAction(actionQuit);
  topBar->addSeparator();

  auto edit = new QMenu("Edit");
  topBar->addMenu(edit);
  auto actionCopy = new QAction("Copy", mainWidget);
  edit->addAction(actionCopy);
  auto actionPaste = new QAction("Paste", mainWidget);
  edit->addAction(actionPaste);
  auto actionCut = new QAction("Cut", mainWidget);
  edit->addAction(actionCut);
  edit->addSeparator();
  auto actionEditProfile = new QAction("Edit your profile", mainWidget);
  edit->addAction(actionEditProfile);
  topBar->addSeparator();

  auto help = new QMenu("Help");
  topBar->addMenu(help);
  auto actionAboutEditor = new QAction("About the Editor", mainWidget);
  help->addAction(actionAboutEditor);
  help->addSeparator();
  auto actionAboutAuthors = new QAction("About Us", mainWidget);
  help->addAction(actionAboutAuthors);

  layout->addWidget(topBar);

}

void Editor::createToolBar(QVBoxLayout *layout) {

  toolBar = new QToolBar();

  auto actionBold = new QAction("Bold");
  toolBar->addAction(actionBold);
  toolBar->addSeparator();

  auto actionItalic = new QAction("Italic");
  toolBar->addAction(actionItalic);
  toolBar->addSeparator();

  auto actionUnderlined = new QAction("Underlined");
  toolBar->addAction(actionUnderlined);

  layout->addWidget(toolBar);
}
