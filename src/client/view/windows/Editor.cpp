#include <spdlog/spdlog.h>
#include <sstream>
#include <QVBoxLayout>
#include <QAction>
#include "Editor.h"

Editor::Editor(QWidget *parent)  : QMainWindow(parent) {

  //this->setWindowTitle("File Selection"); //nome del file

  mainWidget = new QWidget(this);
  auto layout = new QVBoxLayout(mainWidget);

  setCentralWidget(mainWidget);
  mainWidget->setLayout(layout);

  topBar = new QMenuBar();

  auto file = new QMenu("File");
  topBar->addMenu(file);
  auto actionOpen = new QAction("Open", mainWidget);
  file->addAction(actionOpen);
  auto actionSave = new QAction("Save As...", mainWidget);
  file->addAction(actionSave);

  auto edit = new QMenu("Edit");
  topBar->addMenu(edit);
  auto actionEditProfile = new QAction("Edit your profile", mainWidget);
  edit->addAction(actionEditProfile);

  auto help = new QMenu("Help");
  topBar->addMenu(help);
  auto actionAbout = new QAction("About", mainWidget);
  help->addAction(actionAbout);

  layout->addWidget(topBar);

  textEdit = new QPlainTextEdit();
  layout->addWidget(textEdit);

}

void Editor::onRemoteUpdate(std::string text) {
  //RIAGGIORNARE LA VIEW CON IL NUOVO TESTO
  spdlog::debug("Un nuovo Crdt message arrivato");

}
