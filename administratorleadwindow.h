#ifndef ADMINISTRATORLEADWINDOW_H
#define ADMINISTRATORLEADWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "ccmis.h"
#include "aboutui.h"

namespace Ui {
class AdministratorLeadWindow;
}

class AdministratorLeadWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdministratorLeadWindow(CCMIS* c, QWidget *parent = 0);
    ~AdministratorLeadWindow();

private slots:
    void time_dispose();
    void on_BackButton_clicked();
    void on_WorkButton_clicked();

private:
    Ui::AdministratorLeadWindow *ui;
    CCMIS *mCCMIS;
};

#endif // ADMINISTRATORLEADWINDOW_H
