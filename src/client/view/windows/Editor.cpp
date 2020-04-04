#include "Editor.h"

Editor::Editor(QWidget *parent)
    : QMainWindow(parent), mainWidget(new QWidget(this)),
      textEdit(new QTextEdit(mainWidget)),
      usersOnlineDisplayer(new QLabel("Users online: ")), usersOnlineList() {
  // TODO:fai finestra readonly per vedere info utente con vettore
  this->resize(1000, 500);
  this->setMinimumWidth(500);

  auto layout = new QGridLayout(mainWidget);

  createTopBar(layout);
  createToolBar(layout);

  usersOnlineDisplayer->setFixedHeight(30); // 1 user is present

  usersOnline = new QListWidget(mainWidget);
  usersOnline->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  usersOnline->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  usersOnline->setLayout(new QVBoxLayout);
  usersOnline->setFixedHeight(30);
  usersOnline->setFixedWidth(250);

  layout->addWidget(usersOnline, 3, 2, 1, 1);
  layout->addWidget(textEdit, 2, 0, 5, 2);
  layout->addWidget(usersOnlineDisplayer, 2, 2, 1, 1);

  mainWidget->setLayout(layout);
  setCentralWidget(mainWidget);

  QObject::connect(textEdit->document(), &QTextDocument::contentsChange, this,
                   &Editor::onContentChanged);

  connect(textEdit, &QTextEdit::cursorPositionChanged, this, [&]() {
    isSelecting = textEdit->textCursor().hasSelection();
    refreshActionToggle();
  });

  textEdit->setCurrentFont(QFont(DEFAULT_FONT_FAMILY));
  textEdit->setFontPointSize(DEFAULT_FONT_SIZE);
  textEdit->setFocus();
}

void Editor::onFileTextLoad(FileText &text, QString &fName, QString &username,
                            unsigned int editorId) {
  this->setWindowTitle(fName);
  usersOnlineList.insert(editorId, username);
  refresOnlineUsersView();

  textEdit->document()->blockSignals(true);
  textEdit->blockSignals(true);
  for (Symbol &s : text) {
    textEdit->setCurrentCharFormat(s.getFormat());
    textEdit->insertPlainText(s.getChar());
  }
  textEdit->document()->blockSignals(false);
  textEdit->blockSignals(false);
  textEdit->setFocus();
}

void Editor::onRemoteUserConnected(qint32 cId, const QString &username) {
  usersOnlineList.insert(cId, username);

  if (usersOnlineList.size() <= 5) {
    usersOnline->setFixedHeight(usersOnlineList.size() * 30);
  } else {
    usersOnline->setFixedHeight(150);
  }

  refresOnlineUsersView();
  textEdit->setFocus();
}

void Editor::onRemoteUserDisconnected(qint32 cId) {
  usersOnlineList.remove(cId);
  refresOnlineUsersView(); // it is not so easy to remove an element from the
                           // list, it's better to refresh
  textEdit->setFocus();
}

void Editor::createTopBar(QGridLayout *layout) {
  auto topBar = new QMenuBar(mainWidget);
  /*File menu*/
  auto file = new QMenu("File", topBar);
  auto actionSave = new QAction("Save As PDF", file);
  auto actionClose = new QAction("Select another file", file);
  auto actionQuit = new QAction("Quit", file);
  /*Edit menu*/
  auto edit = new QMenu("Edit", topBar);
  auto actionCopy = new QAction("Copy", edit);
  auto actionPaste = new QAction("Paste", edit);
  auto actionCut = new QAction("Cut", edit);
  auto actionGenerateLink = new QAction("Invite a contributor", edit);
  auto actionEditProfile = new QAction("Edit your profile", edit);
  /*Help Menu*/
  auto help = new QMenu("Help", topBar);
  auto actionAboutEditor = new QAction("About the Editor", help);
  auto actionAboutAuthors = new QAction("About Us", help);

  QObject::connect(actionCopy, &QAction::triggered, this, [this]() {
    QCoreApplication::postEvent(
        textEdit,
        new QKeyEvent(QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier));
  });

  QObject::connect(actionPaste, &QAction::triggered, this, [this]() {
    QCoreApplication::postEvent(
        textEdit,
        new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));
  });

  QObject::connect(actionCut, &QAction::triggered, this, [this]() {
    QCoreApplication::postEvent(
        textEdit,
        new QKeyEvent(QEvent::KeyPress, Qt::Key_X, Qt::ControlModifier));
  });

  QObject::connect(actionGenerateLink, &QAction::triggered, this, [this]() {
    // TODO: genera link + setta text di linkDisplayer
    QMessageBox::information(mainWidget, "CooperativeEditor",
                             "-------link-----");
  });

  QObject::connect(actionEditProfile, &QAction::triggered, this, [this]() {
    emit openEditProfileFromEditor();
    this->setDisabled(true);
  });

  QObject::connect(actionQuit, &QAction::triggered, this, &QMainWindow::close);

  QObject::connect(actionClose, &QAction::triggered, this, [this]() {
    emit openVisualizerFromEditor();
    emit fileClosed();
  });

  QObject::connect(actionAboutAuthors, &QAction::triggered, this, [this]() {
    QMessageBox::information(
        this, "CooperativeEditor",
        "Authors:\n  "
        "- Francesco Pavan: front-end developer di successo;\n  "
        "- Simone Magnani: vuole le icone tonde perché le ha Google;\n  "
        "- Riccardo Marchi: sa solo tagliare e cucinare cipolle;\n  "
        "- Francesco Palmieri: PETARDO.");
  });
  QObject::connect(actionAboutEditor, &QAction::triggered, this, [this]() {
    QMessageBox::information(
        this, "CooperativeEditor",
        "Editor totally developed in C++\n\n Copyright fastidioso");
  });
  QObject::connect(actionSave, &QAction::triggered, this, &Editor::fileToPDF);

  actionPaste->setShortcuts(QKeySequence::Paste);
  actionCopy->setShortcuts(QKeySequence::Copy);
  actionCut->setShortcuts(QKeySequence::Cut);
  edit->addAction(actionPaste);
  edit->addAction(actionCopy);
  edit->addAction(actionCut);
  edit->addSeparator();
  edit->addAction(actionGenerateLink);
  edit->addSeparator();
  edit->addAction(actionEditProfile);

  help->addAction(actionAboutEditor);
  help->addSeparator();
  help->addAction(actionAboutAuthors);

  file->addAction(actionSave);
  file->addSeparator();
  file->addAction(actionClose);
  file->addAction(actionQuit);

  topBar->addMenu(file);
  topBar->addSeparator();
  topBar->addMenu(edit);
  topBar->addSeparator();
  topBar->addMenu(help);
  topBar->setFixedHeight(34);

  layout->addWidget(topBar, 0, 0, 1, 2);
}

void Editor::createToolBar(QGridLayout *layout) {
  auto toolBar = new QToolBar(mainWidget);
  const QIcon boldIcon =
      QIcon::fromTheme("format-text-bold", QIcon(":/images/mac/textbold.png"));
  const QIcon italicIcon = QIcon::fromTheme(
      "format-text-italic", QIcon(":/images/mac/textitalic.png"));
  const QIcon underlineIcon = QIcon::fromTheme(
      "format-text-underline", QIcon(":images/mac/textunder.png"));
  font = new QFontComboBox(toolBar);
  fontSize = new QSpinBox(toolBar);
  font->setCurrentFont(QFont(DEFAULT_FONT_FAMILY));
  fontSize->setValue(DEFAULT_FONT_SIZE);
  fontSize->setRange(MIN_FONT_SIZE, MAX_FONT_SIZE);

  QFont bold, italic, underline;
  bold.setBold(true);
  italic.setItalic(true);
  underline.setUnderline(true);

  actionBold =
      toolBar->addAction(boldIcon, tr("&Bold"), this, &Editor::onActionClicked);
  toolBar->addSeparator();
  actionItalic = toolBar->addAction(italicIcon, tr("&Italic"), this,
                                    &Editor::onActionClicked);
  toolBar->addSeparator();
  actionUnderlined = toolBar->addAction(underlineIcon, tr("&Underline"), this,
                                        &Editor::onActionClicked);

  toolBar->addSeparator();
  toolBar->addWidget(font);
  toolBar->addSeparator();
  toolBar->addWidget(fontSize);
  connect(font, &QFontComboBox::currentFontChanged, this,
          &Editor::onFontFamilyChanged);

  connect(fontSize,
          static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
          &Editor::onFontSizeChanged);

  actionBold->setFont(bold);
  actionBold->setCheckable(true);
  actionItalic->setCheckable(true);
  actionItalic->setFont(italic);
  actionUnderlined->setFont(underline);
  actionUnderlined->setCheckable(true);
  actionUnderlined->setShortcut(Qt::CTRL + Qt::Key_U);

  toolBar->setFixedHeight(20);
  layout->addWidget(toolBar, 1, 0, 1, 2);
}

void Editor::fileToPDF() {

  QString name = QFileDialog::getSaveFileName(mainWidget, "Export to PDF",
                                              fileName, tr("*.pdf"));
  if (name.isEmpty())
    return;

  if (QFileInfo(name).suffix().isEmpty()) {
    fileName.append(".pdf");
  }

  QPrinter printer(QPrinter::PrinterResolution);
  printer.setOutputFormat(QPrinter::PdfFormat);
  printer.setPaperSize(QPrinter::A4);
  printer.setOutputFileName(name);

  auto doc = textEdit->document();
  doc->setPageSize(printer.pageRect().size());
  doc->print(&printer);
  QMessageBox::information(this, "CooperativeEditor", "File correctly saved");
}

void Editor::refresOnlineUsersView() {
  auto usernames = usersOnlineList.values();
  usernames.removeDuplicates(); // remove duplicates

  if (usernames.size() <= 5) {
    usersOnline->setFixedHeight((int)usernames.size() * 30);
  } else {
    usersOnline->setFixedHeight(150);
  }

  usersOnlineDisplayer->setText("Users online: " +
                                QString::number(usernames.size()));

  usersOnline->clear();
  for (const QString &s : usernames) {
    usersOnline->addItem(s);
  }
}

void Editor::onRemoteErase(int index) {
  isHandlingRemote = true;
  auto cursor = textEdit->textCursor();
  cursor.setPosition(index);
  cursor.deleteChar();
  isHandlingRemote = false;
}

void Editor::onRemoteUpdate(int index, Symbol &symbol) {
  isHandlingRemote = true;
  auto cursor = textEdit->textCursor();
  cursor.setPosition(index);
  cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
  cursor.setCharFormat(symbol.getFormat());
  isHandlingRemote = false;
}

void Editor::onRemoteInsert(int index, Symbol &symbol) {
  auto cursor = textEdit->textCursor();
  isHandlingRemote = true;
  cursor.setPosition(index);
  cursor.setCharFormat(symbol.getFormat());
  cursor.insertText(symbol.getChar());
  isHandlingRemote = false;
}

void Editor::onActionClicked() {
  QTextCharFormat fmt;
  fmt.setFontItalic(actionItalic->isChecked());
  fmt.setFontWeight(actionBold->isChecked() ? QFont::Bold : QFont::Normal);
  fmt.setFontUnderline(actionUnderlined->isChecked());
  textEdit->textCursor().mergeCharFormat(fmt);
  textEdit->mergeCurrentCharFormat(fmt);
}

void Editor::onComeBackFromEditProfileNoChanges() { this->setDisabled(false); }

void Editor::refreshActionToggle() {
  if (!isSelecting) {
    actionBold->setChecked(textEdit->textCursor().charFormat().fontWeight() ==
                           QFont::Bold);

    actionItalic->setChecked(textEdit->textCursor().charFormat().fontItalic());

    actionUnderlined->setChecked(
        textEdit->textCursor().charFormat().fontUnderline());
    font->setCurrentFont(textEdit->currentFont());
    fontSize->blockSignals(true);
    fontSize->setValue(textEdit->currentFont().pointSize());
    fontSize->blockSignals(false);
  }
}

void Editor::onContentChanged(int pos, int del, int add) {
  /*Checking if this event is due to remote op*/
  if (!isHandlingRemote) {
    QTextCursor cursor(textEdit->document());
    for (int i = 0; i < del; i++)
      emit symbolDeleted(pos);

    for (int i = 0; i < add; i++) {
      cursor.setPosition(pos + i + 1);
      emit symbolInserted(pos + i, textEdit->document()->characterAt(pos + i),
                          cursor.charFormat());
    }
  }
}

void Editor::onFontFamilyChanged(const QFont &newFont) {
  QTextCharFormat fmt;
  fmt.setFontFamily(newFont.family());
  textEdit->mergeCurrentCharFormat(fmt);
  textEdit->setFocus();
}

void Editor::onFontSizeChanged(int newSize) {
  QTextCharFormat fmt;
  fmt.setFontPointSize(newSize);
  textEdit->mergeCurrentCharFormat(fmt);
  textEdit->setFocus();
}
