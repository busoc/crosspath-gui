#include "areadialog.h"
#include "ui_areadialog.h"

AreaDialog::AreaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AreaDialog)
{
    ui->setupUi(this);
    setSizeGripEnabled(false);
}

AreaDialog::AreaDialog(QString label, QWidget* parent):
    AreaDialog(parent)
{
    ui->area->setText(label);
    ui->area->setReadOnly(true);
}

AreaDialog::~AreaDialog()
{
    delete ui;
}

double AreaDialog::getLatitude() const
{
    return ui->latitude->value();
}

double AreaDialog::getLongitude() const
{
    return ui->longitude->value();
}

double AreaDialog::getMargin() const
{
    return ui->margin->value();
}

void AreaDialog::setCoordinates(double lat, double lng, double margin)
{
    ui->latitude->setValue(lat);
    ui->longitude->setValue(lng);
    ui->margin->setValue(margin);
}

void AreaDialog::setPeriods(QDateTime fd, QDateTime td)
{
    ui->dtstart->setDateTime(fd);
    ui->dtend->setDateTime(td);
}

void AreaDialog::setNightAndDay(bool night, bool day)
{
    if (night)
    {
        ui->nightOnly->setChecked(true);
    }
    else if (day)
    {
        ui->dayOnly->setChecked(true);
    }
    else {
        ui->both->setChecked(true);
    }
}


QString AreaDialog::getLabel() const {
    return ui->area->text();
}

QDateTime AreaDialog::getStarts() const {
    return ui->dtstart->dateTime();
}

QDateTime AreaDialog::getEnds() const {
    return ui->dtend->dateTime();
}

bool AreaDialog::getNight() const {
    return ui->nightOnly->isChecked();
}

bool AreaDialog::getDay() const {
    return ui->dayOnly->isChecked();
}

double AreaDialog::getNorth() const {
    return ui->latitude->value() + ui->margin->value();
}

double AreaDialog::getSouth() const {
    return ui->latitude->value() - ui->margin->value();
}

double AreaDialog::getWest() const {
    return ui->longitude->value() - ui->margin->value();
}

double AreaDialog::getEast() const {
    return ui->longitude->value() + ui->margin->value();
}
