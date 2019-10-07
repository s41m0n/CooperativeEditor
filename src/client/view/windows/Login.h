#ifndef COOPERATIVEEDITOR_LOGIN_H
#define COOPERATIVEEDITOR_LOGIN_H

#include <QDialog>
#include <QWidget>
#include <QtWidgets/QMainWindow>

class Login : public QMainWindow {

    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);

public slots:
    void onLoginResponse(bool result);

signals:
    void loginRequest(std::string &username, std::string &password);
};


#endif //COOPERATIVEEDITOR_LOGIN_H
