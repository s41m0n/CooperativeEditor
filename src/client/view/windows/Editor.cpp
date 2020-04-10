#include "Editor.h"

Editor::Editor(QWidget *parent)
    : QMainWindow(parent), mainWidget(new QWidget(this)),
      textEdit(new QTextEdit(mainWidget)),
      usersOnlineDisplayer(new QLabel("Users online: ")), usersOnlineList() {
  // TODO:fai finestra readonly per vedere info utente con vettore
  this->resize(1280, 760);
  this->setMinimumWidth(1000);

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

  layout->addWidget(textEdit, 2, 0, 5, 3);
  layout->addWidget(usersOnline, 3, 3, 1, 1);
  layout->addWidget(usersOnlineDisplayer, 2, 3, 1, 1);

  mainWidget->setLayout(layout);
  setCentralWidget(mainWidget);

  QObject::connect(textEdit->document(), &QTextDocument::contentsChange, this,
                   &Editor::onContentChanged);

  QObject::connect(textEdit, &QTextEdit::currentCharFormatChanged, this,
                   &Editor::onCharFormatChanged);

  QObject::connect(textEdit, &QTextEdit::cursorPositionChanged, this, [&]() {
    emit cursorChanged(textEdit->textCursor().position());
  });

  textEdit->setFocus();
}

void Editor::onFileTextLoad(FileText &text, QString &fName, QString &username,
                            unsigned int editorId) {
  this->setWindowTitle("CooperativeEditor - " + fName);
  usersOnlineList.clear();
  usersOnlineList.insert(editorId, username);
  refreshOnlineUsersView();

  isHandlingRemote = true;
  textEdit->document()->blockSignals(true);
  textEdit->blockSignals(true);
  textEdit->clear();
  for (Symbol &s : text) {
    textEdit->setCurrentCharFormat(s.getFormat());
    textEdit->insertPlainText(s.getChar());
  }
  textEdit->document()->blockSignals(false);
  textEdit->blockSignals(false);
  isHandlingRemote = false;
  textEdit->setFocus();
}

void Editor::onRemoteUserConnected(qint32 cId, const QString &username) {
  usersOnlineList.insert(cId, username);

  if (usersOnlineList.size() <= 5) {
    usersOnline->setFixedHeight(usersOnlineList.size() * 30);
  } else {
    usersOnline->setFixedHeight(150);
  }

  refreshOnlineUsersView();
  textEdit->setFocus();
}

void Editor::onRemoteUserDisconnected(qint32 cId) {
  usersOnlineList.remove(cId);
  refreshOnlineUsersView(); // it is not so easy to remove an element from the
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
    emit generateLink();
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
  const QIcon save = QIcon::fromTheme("document-save-as", QIcon(":images/mac/document-save-as"));
  const QIcon copy = QIcon::fromTheme("edit-copy", QIcon(":images/mac/edit-copy"));
  const QIcon cut = QIcon::fromTheme("edit-cut", QIcon(":images/mac/edit-cut"));
  const QIcon paste = QIcon::fromTheme("edit-paste", QIcon(":images/mac/edit-paste"));
  const QIcon undo = QIcon::fromTheme("edit-undo", QIcon(":images/mac/edit-undo"));
  const QIcon redo = QIcon::fromTheme("edit-redo", QIcon(":images/mac/edit-redo"));

  font = new QFontComboBox(toolBar);
  font->setWritingSystem(QFontDatabase::Latin);
  fontSize = new QSpinBox(toolBar);

  actionColorText = new QPushButton(toolBar);
  actionColorText->setText("A");
  actionColorBackground = new QPushButton(toolBar);
  actionColorBackground->setStyleSheet(
      "QPushButton {background-color: transparent;}");
  textEdit->setTextBackgroundColor(DEFAULT_BACKGROUND_COLOR);
  textEdit->setTextColor(DEFAULT_TEXT_COLOR);

  textEdit->setFont(QFont(DEFAULT_FONT_FAMILY));
  font->setFont(textEdit->currentFont());
  textEdit->setFontPointSize(DEFAULT_FONT_SIZE);
  fontSize->setValue(textEdit->fontPointSize());
  fontSize->setRange(MIN_FONT_SIZE, MAX_FONT_SIZE);
  textEdit->document()->setDefaultFont(textEdit->currentFont());

  toolBar->addAction(save, tr("&SaveAs"), this, &Editor::fileToPDF);
  toolBar->addSeparator();
  toolBar->addAction(copy, tr("&Copy"), textEdit, &QTextEdit::copy);
  toolBar->addSeparator();
  toolBar->addAction(paste, tr("&Paste"), textEdit, &QTextEdit::paste);
  toolBar->addSeparator();
  toolBar->addAction(cut, tr("&Cut"), textEdit, &QTextEdit::cut);
  toolBar->addSeparator();
  toolBar->addAction(redo, tr("&Redo"), textEdit, &QTextEdit::redo);
  toolBar->addSeparator();
  toolBar->addAction(undo, tr("&Undo"), textEdit, &QTextEdit::undo);
  toolBar->addSeparator();


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
  toolBar->addSeparator();
  toolBar->addWidget(actionColorText);
  toolBar->addSeparator();
  toolBar->addWidget(actionColorBackground);

  QObject::connect(actionColorText, &QPushButton::clicked, [&]() {
    auto picked = QColorDialog::getColor(Qt::white, this, "Pick a color");
    if (picked != nullptr) {
      onColorForegroundChanged(picked);
    }
  });
  QObject::connect(actionColorBackground, &QPushButton::clicked, [&]() {
    auto picked = QColorDialog::getColor(Qt::transparent, this, "Pick a color",
                                         QColorDialog::ShowAlphaChannel);
    if (picked != nullptr) {
      onColorBackgroundChanged(picked);
    }
  });

  connect(font, &QFontComboBox::currentFontChanged, this,
          &Editor::onFontFamilyChanged);

  connect(fontSize,
          static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
          &Editor::onFontSizeChanged);

  actionBold->setCheckable(true);
  actionItalic->setCheckable(true);
  actionUnderlined->setCheckable(true);

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

void Editor::refreshOnlineUsersView() {
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
  textEdit->mergeCurrentCharFormat(fmt);
}

void Editor::onComeBackFromEditProfileNoChanges() { this->setDisabled(false); }

void Editor::onContentChanged(int pos, int del, int add) {
  /*Checking if this event is due to remote op*/
  if (!isHandlingRemote) {
    auto cursor = textEdit->textCursor();
    for (int i = 0; i < del; i++)
      emit symbolDeleted(pos);

    for (int i = 0; i < add; i++) {
      cursor.setPosition(pos + i);
      emit symbolInserted(pos + i, textEdit->document()->characterAt(pos + i),
                          cursor.charFormat());
    }
  }
}

void Editor::onFontFamilyChanged(const QFont &newFont) {
  QTextCharFormat fmt;
  fmt.setFontFamily(newFont.family());
  textEdit->textCursor().mergeCharFormat(fmt);
  textEdit->mergeCurrentCharFormat(fmt);
  textEdit->setFocus();
}

void Editor::onFontSizeChanged(int newSize) {
  QTextCharFormat fmt;
  fmt.setFontPointSize(newSize);
  textEdit->textCursor().mergeCharFormat(fmt);
  textEdit->mergeCurrentCharFormat(fmt);
}

void Editor::onColorForegroundChanged(const QColor &color) {
  QTextCharFormat fmt;
  fmt.setForeground(color);
  textEdit->textCursor().mergeCharFormat(fmt);
  textEdit->mergeCurrentCharFormat(fmt);
  textEdit->setFocus();
}

void Editor::onColorBackgroundChanged(const QColor &color) {
  auto toAdd =
      (color.alpha() == 0 ? QColor("transparent") : QColor(color.name()));
  QTextCharFormat fmt;
  fmt.setBackground(toAdd);
  textEdit->textCursor().mergeCharFormat(fmt);
  textEdit->mergeCurrentCharFormat(fmt);
  textEdit->setFocus();
}

void Editor::onCharFormatChanged(const QTextCharFormat &f) {

  auto frontColor = f.foreground().color();
  auto backColor = f.background().color();

  actionBold->setChecked(f.fontWeight() == QFont::Bold);
  actionItalic->setChecked(f.fontItalic());
  actionUnderlined->setChecked(f.fontUnderline());

  font->blockSignals(true);
  font->setCurrentIndex(font->findText(QFontInfo(f.font()).family()));
  font->blockSignals(false);

  fontSize->blockSignals(true);
  fontSize->setValue((f.fontPointSize() >= MIN_FONT_SIZE ? f.fontPointSize()
                                                         : DEFAULT_FONT_SIZE));
  fontSize->blockSignals(false);

  actionColorText->setStyleSheet("QPushButton {color: " + frontColor.name() +
                                 ";}");

  actionColorBackground->setStyleSheet(
      "QPushButton {background-color: " +
      (backColor.alpha() == 0 ? "transparent" : backColor.name()) + ";}");
}

void Editor::onGenerateLinkAnswer(const QString& code) {
  QMessageBox::information(mainWidget, "CooperativeEditor",
                           "Share this code to invite contributors!\n" + code);
}
void Editor::onUserCursorChanged(quint32 clientId, int position) {
  // TODO: aggiunger mappa con clientId -> {colore_scelto - cursore_remoto}
  spdlog::debug("Update remote cursor {} {}", clientId, position);
}
