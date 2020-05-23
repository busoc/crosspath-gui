#ifndef AREADIALOG_H
#define AREADIALOG_H

#include <QDialog>

namespace Ui {
class AreaDialog;
}

enum Eclipse {Both, Night, Day};

class AreaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AreaDialog(QWidget *parent = nullptr);
    explicit AreaDialog(QString label, QWidget* parent=nullptr);
    ~AreaDialog();

    QString getLabel() const;

    double getNorth() const;
    double getSouth() const;
    double getEast() const;
    double getWest() const;

    bool getNight() const;
    bool getDay() const;

    QDateTime getStarts() const;
    QDateTime getEnds() const;

    double getLatitude() const;
    double getLongitude() const;
    double getMargin() const;

    void setCoordinates(double lat, double lng, double margin);
    void setPeriods(QDateTime fd, QDateTime td);
    void setNightAndDay(bool night, bool day);

private:
    Ui::AreaDialog *ui;
};

#endif // AREADIALOG_H
