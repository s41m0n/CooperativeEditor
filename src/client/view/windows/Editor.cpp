#include <spdlog/spdlog.h>
#include <sstream>
#include <QGridLayout>
#include "Editor.h"

Editor::Editor(QWidget *parent) : QMainWindow(parent) {
}

void Editor::onFileListing(std::string files) {

  //parsing of files string
  std::replace(files.begin(), files.end(), ';', ' ');
  std::vector<std::string> filesArray;
  std::istringstream iss(files);
  for(std::string s; iss >> s; ) {
    filesArray.push_back(s);
  }

  int nFiles = filesArray.size();

  this->setWindowTitle("File Selection");

  mainWidget = new QWidget(this);
  auto layout = new QVBoxLayout(mainWidget);

  setCentralWidget(mainWidget);
  mainWidget->setLayout(layout);

  title = new QLabel(this);
  title->setText("Select the file you want to open from the ones showed below:");
  layout->addWidget(title);

  for(int i = 0; i < nFiles; i++) {
    auto fileButton = new QPushButton();
    fileButton->setText(QString::fromStdString(filesArray[i]));
    fileButton->setAutoDefault(true);
    layout->addWidget(fileButton);

    //serve far emettere ad ogni pulsante un segnale per triggerare l'apertura del file desiderato

  }

  areYouSureQuit = new QMessageBox();
  areYouSureQuit->setText("Are you sure you want to exit?");
  areYouSureQuit->setStandardButtons(QMessageBox::Yes | QMessageBox::No);

  buttonExit = new QPushButton("Exit");
  buttonExit->setAutoDefault(true);
  layout->addWidget(buttonExit);

  buttonExit->setFocus();

  QObject::connect(buttonExit, &QAbstractButton::clicked, this,
                   [this]() {
                       int result = areYouSureQuit->exec();

                       switch (result) {
                         case QMessageBox::Yes:
                           this->close();
                           break;
                         case QMessageBox::No:
                           areYouSureQuit->close();
                           break;
                         default:
                           //error, should never be reached
                           break;
                       }
                   });
}

void Editor::onFileResult(bool result) {
  if(result) {
    //Il file può essere aperto/creato
    spdlog::debug("Action OK");
  } else {
    spdlog::debug("Action KO");
  }
}

void Editor::onRemoteUpdate(std::string text) {
  //RIAGGIORNARE LA VIEW CON IL NUOVO TESTO
  spdlog::debug("Un nuovo Crdt message arrivato");

}
