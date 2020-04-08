#ifndef COOPERATIVEEDITOR_EDITUSERPROFILE_H
#define COOPERATIVEEDITOR_EDITUSERPROFILE_H

#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <src/common/User.h>

/**
 * Editor of user profile window
 */
class EditUserProfile : public QMainWindow {

  Q_OBJECT

private:
  QGroupBox *imageBorder;
  QLabel *displayImage;
  QLineEdit *nameTextField;
  QLineEdit *surnameTextField;
  QLineEdit *usernameTextField;
  QLineEdit *emailTextField;
  QLineEdit *oldPasswordTextField;
  QLineEdit *newPasswordTextField;
  QLineEdit *newPasswordTextFieldConfirm;
  QPushButton *buttonSelectImage;
  QImage userImage;

protected:
  /**
   * Function to be called when SHOW
   * @param ev
   */
  void showEvent(QShowEvent *ev) override;

  /**
   * Function to be called when CLOSED
   * @param event
   */
  void closeEvent(QCloseEvent *event) override;

public:
  explicit EditUserProfile(QWidget *parent = nullptr);

public slots:

  /**
   * Slot to handle user profile info loaded
   * @param image
   * @param name
   * @param surname
   * @param email
   * @param username
   */
  void onUserProfileInfo(User &user);

signals:

  /**
   * Signal to require user info
   */
  void requestUserProfile();

  /**
   * Signal to go back to editor
   */
  void openEditorFromEditProfileNoChanges();
};

#endif // COOPERATIVEEDITOR_EDITUSERPROFILE_H
