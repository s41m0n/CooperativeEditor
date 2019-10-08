#ifndef COOPERATIVEEDITOR_EDITOR_H
#define COOPERATIVEEDITOR_EDITOR_H

#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMainWindow>

class Editor : public QMainWindow {

    Q_OBJECT

private:
    QWidget* mainWidget;
    QLabel* title;
    QGroupBox* loginBox;
    QLabel* usernameLabel;
    QLineEdit* usernameTextField;
    QLabel* passwordLabel;
    QLineEdit* passwordTextField;
    QPushButton* buttonEnter;
    QPushButton* buttonCancel;

public:
    explicit Editor(QWidget *parent = nullptr);

public slots:
    void onLoginResponse(bool result);
    void emitLoginSignal();
    void cancel();

signals:
  void loginRequest(std::string username, std::string password);
};

#endif //COOPERATIVEEDITOR_EDITOR_H
