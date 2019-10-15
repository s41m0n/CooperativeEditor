#include <spdlog/spdlog.h>
#include "FileVisualizer.h"

FileVisualizer::FileVisualizer(QWidget *parent) : QMainWindow(parent) {
}


void FileVisualizer::onFileListing(const QVector<QString> &filesArray) {

  this->setWindowTitle("File Selection");

  mainWidget = new QWidget(this);
  auto layout = new QVBoxLayout(mainWidget);

  setCentralWidget(mainWidget);
  mainWidget->setLayout(layout);

  titleOpen = new QLabel(this);
  titleOpen->setText(
          "Select the file you want to open:");
  layout->addWidget(titleOpen);

  filesBox = new QGroupBox(mainWidget);
  filesBox->setLayout(new QVBoxLayout());
  layout->addWidget(filesBox);

  if(filesArray.empty()){
    auto noFiles = new QLabel("There are not file to open on the server. Please create a new file.");
    filesBox->layout()->addWidget(noFiles);
  } else {
    for (const auto & i : filesArray) {
      auto fileButton = new QPushButton();
      fileButton->setText(i);
      fileButton->setAutoDefault(true);
      filesBox->layout()->addWidget(fileButton);

      //serve far emettere ad ogni pulsante un segnale per richiedere al server l'apertura del file desiderato

    }
  }

  areYouSureQuit = new QMessageBox();
  areYouSureQuit->setText("Are you sure you want to exit?");
  areYouSureQuit->setStandardButtons(QMessageBox::Yes | QMessageBox::No);

  fileCannotBeOpened = new QMessageBox();
  fileCannotBeOpened->setText("Sorry, the file you selected cannot be opened. Do you want to choose another file?");
  fileCannotBeOpened->setStandardButtons(QMessageBox::Yes | QMessageBox::No);

  titleCreate = new QLabel(this);
  titleCreate->setText(
          "Click here to create a new file:");
  layout->addWidget(titleCreate);

  buttonCreate = new QPushButton("New file");
  buttonCreate->setAutoDefault(true);
  layout->addWidget(buttonCreate);

  buttonExit = new QPushButton("Exit");
  buttonExit->setAutoDefault(true);
  layout->addWidget(buttonExit);

  buttonExit->setFocus();

  QObject::connect(buttonExit, &QAbstractButton::clicked, this,
                   [this]() {
                       int resultExit = areYouSureQuit->exec();

                       switch (resultExit) {
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

void FileVisualizer::onFileResult(bool result) {
  if(result) {
    //Il file può essere aperto/creato
    spdlog::debug("Action OK");
    //Va triggerata l'apertura dell'editor e inviato il contenuto del file
  } else {
    spdlog::debug("Action KO");

    int dialogResult = fileCannotBeOpened->exec();

    switch (dialogResult) {
      case QMessageBox::Yes:
        fileCannotBeOpened->close();
        break;
      case QMessageBox::No:
        this->close();
        break;
      default:
        //error, should never be reached
        break;
    }
  }
}
