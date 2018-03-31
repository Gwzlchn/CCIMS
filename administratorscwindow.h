#ifndef ADMINISTRATORSCWINDOW_H
#define ADMINISTRATORSCWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "ccmis.h"
#include "table_funcs.h"
#include "administratoraddwindow.h"

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
    void ModItem(int year, int month, int day, int hour, int min, int sec, int onum, int inum, int money);

private slots:
    void time_dispose();
    void on_BackButton_clicked();

    void on_tableWidget_itemClicked(QTableWidgetItem *item);

    void on_DeleteButton_clicked();

    void on_InsertButton_clicked();

private:
    int mCurrentItem;
    Ui::AdministratorSCWindow *ui;
    CCMIS *mCCMIS;
    Admin_Table* mAdminTable;
};

#endif // ADMINISTRATORSCWINDOW_H
