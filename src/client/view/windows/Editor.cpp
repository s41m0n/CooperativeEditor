#include "Editor.h"

Editor::Editor(QWidget *parent)
        : QMainWindow(parent), mainWidget(new QWidget(this)),
          textEdit(new QTextEdit(mainWidget)),
          usersOnlineDisplayer(new QLabel("Users online: ")),
          usersOnlineList() {
  srand(time(NULL));

  //SIMONE:
    //TODO: cambia SQL in Database.cpp + cambia messaggi utente;
  //FRA:
    //TODO: messaggi invio nuovi dati + mostrare box in caso di pasword sbagliata;
    //TODO: se utente prova ad uscire gli chiedo conferma se vuole farlo?
    //TODO: cerca icona formato senza dover impazzire + togli che cambia colore in toolbar
    //TODO: sistema easter egg delle frasi su di noi
  //RICK:
    //TODO: quando cancelli codice in blocco come sposta puntatori remoti
    //TODO: sistema gestione messaggio non riconosciuto nel server
    //TODO: controllare eccezioni CRDT

  this->resize(1280, 760);
  this->setMinimumWidth(1000);

  auto layout = new QGridLayout(mainWidget);

  createTopBar(layout);
  createToolBar(layout);

  usersOnlineDisplayer->setFixedHeight(30); // 1 user is present

  usersOnline = new QListWidget(mainWidget);
  usersOnline->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  usersOnline->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  usersOnline->setSortingEnabled(true);
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

  QObject::connect(
          usersOnline, &QListWidget::itemDoubleClicked, this,
          [this](QListWidgetItem *item) {
              auto username = item->text();
              if (item->background().isOpaque()) {
                usersOnline->addItem(item->text());
                delete item; //it is not possible to find the default background color, so delete + insert
                usersOnline->clearSelection(); //to avoid the blue background on the selection
                textEdit->setFocus();
                emit getUserTextOriginal(username);
                for (int i = 0; i < usersOnline->count(); i++) {
                  auto *itemIterator = usersOnline->item(i);
                  if (itemIterator->background().isOpaque())
                    break;
                  if (i == usersOnline->count() - 1) {
                      textEdit->setReadOnly(false);
                      toolBar->setDisabled(false);
                  }
                }
              } else {
                auto color = clientColorCursor[usersOnlineList.key(username)].first;
                item->setBackgroundColor(color);
                usersOnline->clearSelection(); //to avoid the blue background on the selection
                textEdit->setFocus();
                emit getUserText(username);
                textEdit->setReadOnly(true);
                toolBar->setDisabled(true);
              }
          });

  textEdit->setFocus();
}

void Editor::onFileTextLoad(FileText &text, QString &fName, QString &username,
                            unsigned int editorId) {
  this->setWindowTitle("CooperativeEditor - File `" + fName + "`");
  usersOnlineList.clear();
  usersOnlineList.insert(editorId, username);
  QString colorGenerated = generateRandomColor().name();
  clientColorCursor.insert(editorId, std::make_pair(colorGenerated, nullptr));
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
  auto alreadyPresentId = usersOnlineList.key(username, -1);
  usersOnlineList.insert(cId, username);

  QColor color;
  if (alreadyPresentId == -1) {
    color = generateRandomColor();
  } else {
    color = QColor(clientColorCursor.value(alreadyPresentId).first);
  }

  QFont fontCursor("American Typewriter", 10, QFont::Bold);
  auto *remoteLabel = new QLabel(QString(username + "\n"), textEdit);
  remoteLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
  remoteLabel->setStyleSheet("color:" + color.name() +
                             ";background-color:transparent;border: 1px solid transparent;border-left-color:" +
                             color.name() + ";");
  remoteLabel->setFont(fontCursor);
  QTextCursor remoteCursor(textEdit->document());
  remoteCursor.setPosition(0);
  QRect curCoord = textEdit->cursorRect(remoteCursor);
  int height = curCoord.bottom() - curCoord.top();
  remoteLabel->resize(1000, height + 5);
  QFont l_font = remoteLabel->font();
  QTextCharFormat fmt = remoteCursor.charFormat();
  int font_size = static_cast<int>(fmt.fontPointSize());
  if (font_size == 0)
    font_size = 12;
  QFont new_font(l_font.family(), static_cast<int>((font_size / 2) + 3),
                 QFont::Bold);
  remoteLabel->setFont(new_font);
  remoteLabel->move(curCoord.left(),
                    curCoord.top() - (remoteLabel->fontInfo().pointSize() / 3));
  remoteLabel->setVisible(true);
  remoteLabel->raise();

  clientColorCursor.insert(cId, std::make_pair(color.name(), remoteLabel));

  cursorChanged(textEdit->textCursor().position());

  refreshOnlineUsersView();
  textEdit->setFocus();
}

void Editor::onRemoteUserDisconnected(qint32 cId) {
  usersOnlineList.remove(cId);
  auto &client = clientColorCursor.find(cId).value();
  delete client.second;
  clientColorCursor.remove(cId);
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
  toolBar = new QToolBar(mainWidget);
  const QIcon boldIcon =
          QIcon::fromTheme("format-text-bold",
                           QIcon(":/images/mac/textbold.png"));
  const QIcon italicIcon = QIcon::fromTheme(
          "format-text-italic", QIcon(":/images/mac/textitalic.png"));
  const QIcon underlineIcon = QIcon::fromTheme(
          "format-text-underline", QIcon(":images/mac/textunder.png"));
  const QIcon save = QIcon::fromTheme("document-save-as",
                                      QIcon(":images/mac/document-save-as"));
  const QIcon copy = QIcon::fromTheme("edit-copy",
                                      QIcon(":images/mac/edit-copy"));
  const QIcon cut = QIcon::fromTheme("edit-cut", QIcon(":images/mac/edit-cut"));
  const QIcon paste = QIcon::fromTheme("edit-paste",
                                       QIcon(":images/mac/edit-paste"));
  const QIcon undo = QIcon::fromTheme("edit-undo",
                                      QIcon(":images/mac/edit-undo"));
  const QIcon redo = QIcon::fromTheme("edit-redo",
                                      QIcon(":images/mac/edit-redo"));

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
          toolBar->addAction(boldIcon, tr("&Bold"), this,
                             &Editor::onActionClicked);
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
      auto picked = QColorDialog::getColor(Qt::transparent, this,
                                           "Pick a color",
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

  toolBar->setFixedHeight(30);
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
    usersOnline->setFixedHeight((int) usernames.size() * 30);
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

void Editor::onRemoteInsert(int index, Symbol &symbol) {
  for (int i = 0; i < usersOnline->count(); i++) {
    auto *item = usersOnline->item(i);
    if (item->text() != symbol.getGeneratorUsername())
      continue;
    auto remoteId = usersOnlineList.key(symbol.getGeneratorUsername());
    QColor color = clientColorCursor[remoteId].first;
    if (item->backgroundColor() == color) {
      symbol.getFormat().setBackground(QBrush(color));
    }
    break;
  }
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
    for (int i = 0; i < del; i++) {
      if (i == 0)
        cursorChanged(pos);
      emit symbolDeleted(pos);
    }

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

void Editor::onGenerateLinkAnswer(const QString &code) {
  QMessageBox::information(mainWidget, "CooperativeEditor",
                           "Share this code to invite contributors!\n" + code);
}

void Editor::onUserCursorChanged(quint32 clientId, int position) {
if (position > textEdit->document()->characterCount() - 1) return;
  spdlog::debug("Update remote cursor {} {}", clientId, position);
  auto &label = clientColorCursor.value(clientId).second;
  QTextCursor remoteCursor(textEdit->document());
  remoteCursor.setPosition(position);
  QRect remoteCoord = textEdit->cursorRect(remoteCursor);
  int height = remoteCoord.bottom() - remoteCoord.top();
  label->resize(1000, height + 5);

  /* update label dimension according to remote cursor position */
  QFont l_font = label->font();
  QTextCharFormat fmt = remoteCursor.charFormat();
  int font_size = static_cast<int>(fmt.fontPointSize());
  if (font_size == 0)
    font_size = 12;
  QFont new_font(l_font.family(), static_cast<int>((font_size / 2) + 3),
                 QFont::Bold);
  label->setFont(new_font);

  label->move(remoteCoord.left() + 2,
              remoteCoord.top() - (label->fontInfo().pointSize() / 3));
  label->setVisible(true);
}

void Editor::onUserTextReceived(const QList<int> &positions,
                                const QString &username) {
  this->blockSignals(true);

  auto clientId = usersOnlineList.key(username);
  QColor color = clientColorCursor[clientId].first;

  auto c = textEdit->textCursor(); //to remove the eventual selection
  c.clearSelection();
  textEdit->setTextCursor(c);

  for (int p : positions) {
    QTextCharFormat fmt;
    fmt.setBackground(QBrush(color));
    QTextCursor cursor(textEdit->document());
    cursor.movePosition(
            QTextCursor::Start); //I place it at the beginning of the document
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor,
                        p); //cursor is now where I want to update the text
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor,
                        1); //select 1 char
    cursor.mergeCharFormat(fmt);
    cursor.clearSelection();
  }

  this->blockSignals(false);
}

void
Editor::onUserOriginalTextReceived(const QMap<int, QBrush> &textAndColors) {
  this->blockSignals(true);

  auto c = textEdit->textCursor(); //to remove the eventual selection
  c.clearSelection();
  textEdit->setTextCursor(c);

  for (int p : textAndColors.keys()) {
    QTextCharFormat fmt;
    fmt.setBackground(textAndColors.value(p));
    QTextCursor cursor(textEdit->document());
    cursor.movePosition(
            QTextCursor::Start); //I place it at the beginning of the document
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor,
                        p); //cursor is now where I want to update the text
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor,
                        1); //select 1 char
    cursor.mergeCharFormat(fmt);
    cursor.clearSelection();
  }

  this->blockSignals(false);
}

QColor Editor::generateRandomColor() {
  while (true) {
    QColor color = QColor(rand() % 220, rand() % 220, rand() % 220);
    auto values = clientColorCursor.values();
    auto iterator = values.begin();
    while (iterator != values.end()) {
      if (iterator->first == color.name()) break;
      iterator++;
    }
    if (iterator == values.end())
      return color;
  }
}
