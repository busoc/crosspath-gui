#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include <QDialog>
#include <QThread>
#include "infopanel.h"
#include "points.h"
#include "reader.h"

namespace Ui {
class StatsDialog;
}

class StatsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatsDialog(QString file, QWidget *parent = nullptr);
    ~StatsDialog();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::StatsDialog *ui;
    InfoPanel* eclipse;
    InfoPanel* saa;
    QString trajectory;

    void updateInfo();

private slots:
    void closeDialog(int);
};


#endif // STATSDIALOG_H
