#ifndef COOPERATIVEEDITOR_LOGIN_H
#define COOPERATIVEEDITOR_LOGIN_H

#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMainWindow>
#include <QString>

class Login : public QMainWindow {

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
    explicit Login(QWidget *parent = nullptr);

public slots:
    void onLoginResponse(bool result);
    void emitLoginSignal();
    void cancel();

signals:
    void loginRequest(QString username, QString password);
};


#endif //COOPERATIVEEDITOR_LOGIN_H
