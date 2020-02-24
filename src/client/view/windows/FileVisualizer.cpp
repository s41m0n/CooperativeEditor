#include "FileVisualizer.h"

FileVisualizer::FileVisualizer(QWidget *parent) : QMainWindow(parent) {
    this->setWindowTitle("File Selection");
    this->setFixedSize(this->minimumSize());

    mainWidget = new QWidget(this);
    auto layout = new QGridLayout(mainWidget);

    setCentralWidget(mainWidget);
    mainWidget->setLayout(layout);

    titleOpen = new QLabel(mainWidget);
    titleOpen->setText(
            "Select the file you want to open from the ones showed below:");
    layout->addWidget(titleOpen, 0, 0, 1, 2);

    list = new QListWidget(mainWidget);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setSortingEnabled(true);

    layout->addWidget(list, 1, 0, 3, 2);

    areYouSureQuit = new QMessageBox(this);
    areYouSureQuit->setText("Are you sure you want to exit?");
    areYouSureQuit->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    areYouSureQuit->setFixedSize(this->minimumSize());

    boxCreate = new QGroupBox("Click here to create a new file:", mainWidget);
    boxCreate->setLayout(new QVBoxLayout());
    layout->addWidget(boxCreate, 4, 0, 1, 2);

    buttonCreate = new QPushButton("New file", boxCreate);
    buttonCreate->setAutoDefault(true);
    boxCreate->layout()->addWidget(buttonCreate);

    buttonExit = new QPushButton("Exit", mainWidget);
    buttonExit->setAutoDefault(true);
    layout->addWidget(buttonExit, 5, 0, 1, 2);

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

    QObject::connect(buttonCreate, &QAbstractButton::clicked, this,
                     [this]() {
                         bool ok;
                         QString name = QInputDialog::getText(this,
                                                              "New File",
                                                              "Insert the name of the file you want to create:",
                                                              QLineEdit::Normal,
                                                              "",
                                                              &ok); //ok = button ok on the dialog

                         if (ok && !name.isEmpty()) { //ok clicked + text provided
                             emit fileRequest(name, false);
                         } else if (ok) { //ok clicked but no text provided
                             auto nameEmpty = new QMessageBox(this);
                             nameEmpty->setText(
                                     "You inserted an invalid name. Try again.");
                             nameEmpty->setFixedSize(this->minimumSize());
                             nameEmpty->show();
                         }
                     });

    QObject::connect(list, &QListWidget::itemDoubleClicked, this,
                     [this](QListWidgetItem *item) {
                         emit fileRequest(item->text(), true);
                     });
}

void FileVisualizer::onFileListing(const QVector<QString> &filesArray) {

  if (filesArray.size() <= 10) {
    list->setFixedHeight((int) filesArray.size() * 30);
  } else {
    list->setFixedHeight(300);
  }

  list->clear();
  if (filesArray.empty()) {
    list->addItem(
            "There are not file to open on the server. Please create a new file.");
  } else {
    for (const auto &i : filesArray) {
      list->addItem(i);
    }
  }
}
