#include "FileVisualizer.h"

FileVisualizer::FileVisualizer(QWidget *parent) : QMainWindow(parent) {
  this->setWindowTitle("CooperativeEditor - Home");
  this->setFixedSize(this->minimumSize());

  auto mainWidget = new QWidget(this);
  auto layout = new QGridLayout(mainWidget);

  setCentralWidget(mainWidget);
  mainWidget->setLayout(layout);

  auto boxFileList =
      new QGroupBox("Select the file you want to open:", mainWidget);
  boxFileList->setLayout(new QVBoxLayout());
  layout->addWidget(boxFileList, 0, 0, 1, 2);

  list = new QListWidget(boxFileList);
  list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  list->setSortingEnabled(true);

  boxFileList->layout()->addWidget(list);

  auto boxCreate =
      new QGroupBox("Click here to create a new file:", mainWidget);
  boxCreate->setLayout(new QVBoxLayout());
  layout->addWidget(boxCreate, 1, 0, 1, 2);

  auto buttonCreate = new QPushButton("New file", boxCreate);
  buttonCreate->setAutoDefault(true);
  boxCreate->layout()->addWidget(buttonCreate);

  auto boxLink = new QGroupBox(
      "Insert here the shared code to collaborate to a file:", mainWidget);
  boxLink->setLayout(new QVBoxLayout());
  layout->addWidget(boxLink, 2, 0, 1, 2);

  lineLink = new QLineEdit(mainWidget);
  boxLink->layout()->addWidget(lineLink);

  auto buttonOpenLink = new QPushButton("Open link", boxLink);
  buttonOpenLink->setAutoDefault(true);
  boxLink->layout()->addWidget(buttonOpenLink);

  QObject::connect(buttonOpenLink, &QAbstractButton::clicked, this,
                   [&]() { emit insertInviteLink(lineLink->text()); });

  auto buttonEditProfile = new QPushButton("Edit Profile", boxLink);
  buttonEditProfile->setAutoDefault(true);
  layout->addWidget(buttonEditProfile, 3, 0, 1, 2);

  QObject::connect(buttonEditProfile, &QAbstractButton::clicked, this, &FileVisualizer::openEditProfile);

  auto buttonExit = new QPushButton("Exit", mainWidget);
  buttonExit->setAutoDefault(true);
  layout->addWidget(buttonExit, 4, 0, 1, 2);

  buttonExit->setFocus();

  QObject::connect(buttonExit, &QAbstractButton::clicked, this, [this]() {
    int resultExit = QMessageBox::question(this, "CooperativeEditor",
                                           "Are you sure you want to exit?",
                                           QMessageBox::Yes, QMessageBox::No);

    if (resultExit == QMessageBox::Yes) {
      this->close();
    }
  });

  QObject::connect(buttonCreate, &QAbstractButton::clicked, this, [this]() {
    bool ok;
    QString name = QInputDialog::getText(
        this, "New File",
        "Insert the name of the file you want to create:", QLineEdit::Normal,
        "",
        &ok); // ok = button ok on the dialog

    if (ok && !name.isEmpty()) { // ok clicked + text provided
      emit fileRequest(name, false);
    } else if (ok) { // ok clicked but no text provided
      auto nameEmpty = new QMessageBox(this);
      nameEmpty->setText("You inserted an invalid name. Try again.");
      nameEmpty->setFixedSize(this->minimumSize());
      nameEmpty->show();
    }
  });

  QObject::connect(
      list, &QListWidget::itemDoubleClicked, this,
      [this](QListWidgetItem *item) { emit fileRequest(item->text(), true); });
}

void FileVisualizer::onFileListing(const QVector<QString> &filesArray) {

  if (filesArray.size() <= 10) {
    list->setFixedHeight((int)filesArray.size() * 30);
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

void FileVisualizer::showEvent(QShowEvent *ev) {
  emit requestFileList();
}
