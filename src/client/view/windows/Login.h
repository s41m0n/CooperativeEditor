#ifndef COOPERATIVEEDITOR_LOGIN_H
#define COOPERATIVEEDITOR_LOGIN_H

#include <QMainWindow>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QMessageBox>
#include <QGridLayout>
#include <src/common/User.h>

/**
 * Login application window
 */
class Login : public QMainWindow {

Q_OBJECT

private:
    QLineEdit *usernameTextField;
    QLineEdit *passwordTextField;

public:

    explicit Login(QWidget *parent = nullptr);

signals:

    /**
     * Signal to notify login attempt
     * @param user to be logged
     */
    void loginRequest(User user);

    /**
     * Signal to notify to open signup view
     */
    void signUp();
};


#endif //COOPERATIVEEDITOR_LOGIN_H
