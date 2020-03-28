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
    QGroupBox *imageBorder;
    QLabel *displayImage;
    QLabel *nameLabel;
    QLineEdit *nameTextField;
    QLabel *surnameLabel;
    QLineEdit *surnameTextField;
    QLabel *usernameLabel;
    QLineEdit *usernameTextField;
    QLabel *emailLabel;
    QLineEdit *emailTextField;
    QLabel *oldPasswordLabel;
    QLineEdit *oldPasswordTextField;
    QLabel *newPasswordLabel;
    QLineEdit *newPasswordTextField;
    QLabel *newPasswordLabelConfirm;
    QLineEdit *newPasswordTextFieldConfirm;
    QPushButton *buttonSelectImage;
    QPushButton *buttonExit;
    QPushButton *buttonSaveAndBackToEditor;
    QPushButton *buttonDeleteProfile;
    QImage userImage;

protected:
    void showEvent(QShowEvent *ev);

public:
    explicit EditUserProfile(QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *event) override;

public slots:

    void onUserProfileInfo(const QImage &image, const QString &name,
                           const QString &surname, const QString &email,
                           const QString &username);

signals:

    void requestUserProfile();
    void openEditorFromEditProfileNoChanges();

};

#endif //COOPERATIVEEDITOR_EDITUSERPROFILE_H
