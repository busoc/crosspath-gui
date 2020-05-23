#ifndef INFOPANEL_H
#define INFOPANEL_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QThread>
#include <QAbstractTableModel>
#include "points.h"
#include "reader.h"


namespace Ui {
class InfoPanel;
}

class PathTableModel;
class DateTimeFilterProxyModel;
class TrajectoryThread;

class InfoPanel : public QWidget
{
    Q_OBJECT

public:
    explicit InfoPanel(QString file, int index, QWidget *parent = nullptr);
    ~InfoPanel();

private:
    Ui::InfoPanel *ui;
    QDateTime dtstart;
    QDateTime dtend;
    PathTableModel *model;
    DateTimeFilterProxyModel *proxy;
    TrajectoryThread* worker;

    void load();

public slots:
    void stop();
    void dtstartChanged(const QDateTime& dt);
    void dtendChanged(const QDateTime& dt);

private slots:
    void pathFound(Path p);
    void detailFound(double duration, double distance);
};

class PathTableModel: public QAbstractTableModel
{
public:
    PathTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void append(Path p);


private:
    QVector<Path> paths;
};

class TrajectoryThread: public QThread
{
    Q_OBJECT

signals:
    void pathFound(const Path& p);
    void pathDetail(double duration, double distance);

public:
    explicit TrajectoryThread(QString file, int index, QObject *parent=nullptr):
        QThread(parent),
        column(index)
    {
        reader = new csv::Reader(file);
    }

    void virtual run() override
    {
        QStringList row;
        double distance = 0;
        double duration = 0;
        while (!reader->done())
        {
            if (isInterruptionRequested())
            {
                break;
            }
            row = reader->read();
            if (row[column] == "1")
            {
                Point first = getPoint(row);
                Point last = first;
                while(!reader->done())
                {
                    if (row[column] == "0")
                    {
                        break;
                    }
                    row = reader->read();
                    last = getPoint(row);
                }
                Path p(first, last);
                emit pathFound(p);

                duration += p.elapsed();
                distance += p.distance();
            }
        }
        emit pathDetail(duration, distance);
    }

private:
    int column;
    csv::Reader *reader;

    Point getPoint(QStringList row)
    {
        Point p;

        p.when = QDateTime::fromString(row[0], Qt::ISODateWithMs);
        p.alt = row[2].toDouble();
        p.lat = row[3].toDouble();
        p.lng = row[4].toDouble();

        return p;
    }
};

class DateTimeFilterProxyModel: public QSortFilterProxyModel
{
public:
    DateTimeFilterProxyModel(QObject* parent=nullptr);

    void setStartDate(QDateTime dt)
    {
        dtstart = dt;
        invalidateFilter();
    }

    void setEndDate(QDateTime dt)
    {
        dtend = dt;
        invalidateFilter();
    }

protected:
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

private:
    QDateTime dtstart;
    QDateTime dtend;
    bool dateInRange(QDateTime dt) const;
};

#endif // INFOPANEL_H
