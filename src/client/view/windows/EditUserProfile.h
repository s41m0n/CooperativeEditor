#ifndef COOPERATIVEEDITOR_EDITUSERPROFILE_H
#define COOPERATIVEEDITOR_EDITUSERPROFILE_H

#include <QMainWindow>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QMessageBox>
#include <QGridLayout>
#include <QFileDialog>

/**
 * Editor of user profile window
 */
class EditUserProfile : public QMainWindow {

Q_OBJECT

private:
    QWidget *mainWidget;
    QGroupBox *registerBox;
    QLabel *imageLabel;
    QLabel *nameLabel;
    QLineEdit *nameTextField;
    QLabel *surnameLabel;
    QLineEdit *surnameTextField;
    QLabel *usernameLabel;
    QLineEdit *usernameTextField;
    QLabel *emailLabel;
    QLineEdit *emailTextField;
    QLabel *passwordLabel;
    QLineEdit *passwordTextField;
    QLabel *passwordLabelConfirm;
    QLineEdit *passwordTextFieldConfirm;
    QPushButton *buttonSelectImage;
    QPushButton *buttonExit;
    QPushButton *buttonSaveAndBackToEditor;
    QMessageBox *errorMessageEmptyFields;
    QMessageBox *errorMessageDifferentPasswords;
    QMessageBox *areYouSureQuit;
    QString userImage;

public:

    explicit EditUserProfile(QWidget *parent = nullptr);

};

#endif //COOPERATIVEEDITOR_EDITUSERPROFILE_H
