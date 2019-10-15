#ifndef COOPERATIVEEDITOR_EDITUSERPROFILE_H
#define COOPERATIVEEDITOR_EDITUSERPROFILE_H

#include <QMainWindow>
#include <QGridLayout>

/**
 * Editor of user profile window
 */
class EditUserProfile : public QMainWindow {

    Q_OBJECT

private:
    QWidget *mainWidget;

public:

    explicit EditUserProfile(QWidget *parent = nullptr);

public slots:
    ///Slot to notify the client that the user wants to edit his profile
    void onEditProfile();

};

#endif //COOPERATIVEEDITOR_EDITUSERPROFILE_H
