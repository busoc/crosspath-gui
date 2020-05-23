#include "statsdialog.h"
#include "ui_statsdialog.h"
#include "reader.h"


StatsDialog::StatsDialog(QString file, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatsDialog),
    trajectory(file)
{
    ui->setupUi(this);
    setSizeGripEnabled(false);

    connect(this, SIGNAL(finished(int)), this, SLOT(closeDialog(int)));

    eclipse = new InfoPanel(file, 5, this);
    saa = new InfoPanel(file, 6, this);

    ui->tabs->addTab(eclipse, "Eclipses");
    ui->tabs->addTab(saa, "SAA Crossing");

    updateInfo();
}

StatsDialog::~StatsDialog()
{
    delete ui;
}

void StatsDialog::closeDialog(int result)
{
    Q_UNUSED(result);
    eclipse->stop();
    saa->stop();
}

void StatsDialog::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event);
    closeDialog(0);

}

void StatsDialog::updateInfo()
{
    QFile file(trajectory);
    file.open(QIODevice::Text|QIODevice::ReadOnly);

    QTextStream fin(&file);

    for (int i = 0; i < 2; i++)
    {
        fin.readLine();
    }

    QString d = fin.readLine().mid(QString("#trajectory duration ").size());
    QString i = fin.readLine().mid(QString("#trajectory interval ").size());

    ui->source->setText(trajectory);
    ui->duration->setText(d);
    ui->interval->setText(i);

    for (int i = 0; i < 5; i++)
    {
        fin.readLine();
    }
    QString r1 = fin.readLine().remove(0, 1);
    QString r2 = fin.readLine().remove(0, 1);

    ui->row1->setText(r1);
    ui->row2->setText(r2);

    file.close();
}

