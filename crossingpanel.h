#ifndef CROSSINGPANEL_H
#define CROSSINGPANEL_H

#include <QThread>
#include <QWidget>
#include <QThread>
#include "points.h"
#include "reader.h"

using namespace std;

namespace Ui {
class CrossingPanel;
}

class CrossingTableModel;
class CrossingThread;

class CrossingPanel : public QWidget
{
    Q_OBJECT

signals:
    void panelClose();
    void panelUpdate();

public:
    explicit CrossingPanel(Area a, QWidget *parent = nullptr);
    ~CrossingPanel();

    Area getArea() {
        return area;
    }

public slots:
    void updatePaths(QString file);
    void pathFound(Path p);
    void removeRequested();
    void editRequested();
    void refreshRequested();
    void updateDMS(bool dhm);

private slots:
    void workerFinished();

private:
    Ui::CrossingPanel *ui;
    CrossingTableModel* model;
    CrossingThread* worker;
    bool running = false;
    Area area;

    void updateComponents(bool dhm = false);
    void setupCrossing();
    void stopWorker();
};

class CrossingThread: public QThread
{
    Q_OBJECT

public:
    explicit CrossingThread(Area area, QString file, QObject *parent = nullptr):
        QThread(parent),
        area(area)
    {
        reader = new csv::Reader(file);
    }

    void virtual run() override
    {
        while(!reader->done())
        {
            if (isInterruptionRequested())
            {
                return;
            }
            Point first = getPoint(reader->read());
            if (area.accept(first))
            {

                Point last = first;
                Point prev;
                while(!reader->done())
                {
                    if (!area.accept(last))
                    {
                        last = prev;
                        break;
                    }
                    prev = last;
                    last = getPoint(reader->read());
                }
                Path p(first, last);

                emit pathFound(p);
            }
        }
    }

signals:
    void pathFound(Path p);

private:
    csv::Reader* reader;
    Area area;

    Point getPoint(QStringList row)
    {
        Point p;

        p.when = QDateTime::fromString(row[0], Qt::ISODateWithMs);
        p.alt = row[2].toDouble();
        p.lat = row[3].toDouble();
        p.lng = row[4].toDouble();
        p.eclipse = row[5].toInt() == 1;

        return p;
    }
};

class CrossingTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    CrossingTableModel(QObject *parent = nullptr);
    CrossingTableModel(const QVector<Path> &ps, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    const QVector<Path> &getPaths() const;
    void append(Path p);

private:
    QVector<Path> paths;
};

#endif // CROSSINGPANEL_H
