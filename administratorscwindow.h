#ifndef ADMINISTRATORSCWINDOW_H
#define ADMINISTRATORSCWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "ccmis.h"
#include "aboutui.h"
#include "table_funcs.h"
#include "administratoradddialog.h"

namespace Ui {
class AdministratorSCWindow;
}

class AdministratorSCWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdministratorSCWindow(CCMIS* c, QWidget *parent = 0);
    ~AdministratorSCWindow();

    void connect_SIG_With_SLOTS();
    Admin_Table* GetTable();
    void UpdateItem();

private slots:
    void time_dispose();
    void on_BackButton_clicked();
    void on_InsertButton_clicked();
    void on_AlterButton_clicked();

private:
    Ui::AdministratorSCWindow *ui;
    CCMIS *mCCMIS;
    Admin_Table* mAdminTable;
};

#endif // ADMINISTRATORSCWINDOW_H
