#include "infopanel.h"
#include "ui_infopanel.h"
#include "datetimedelegate.h"

InfoPanel::InfoPanel(QString file, int index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoPanel)
{
    ui->setupUi(this);
    ui->view->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    model = new PathTableModel;
    proxy = new DateTimeFilterProxyModel(this);
    proxy->setSourceModel(model);
    ui->view->setModel(proxy);
    ui->view->setAlternatingRowColors(true);
    ui->view->setItemDelegateForColumn(0, new DateTimeDelegate);
    ui->view->setItemDelegateForColumn(1, new DateTimeDelegate);

    worker = new TrajectoryThread(file, index, this);
    connect(worker, SIGNAL(pathFound(Path)), this, SLOT(pathFound(Path)));
    connect(worker, SIGNAL(pathDetail(double, double)), this, SLOT(detailFound(double, double)));
    worker->start();

    connect(ui->dtstart, SIGNAL(dateTimeChanged(const QDateTime&)), this, SLOT(dtstartChanged(const QDateTime&)));
    connect(ui->dtend, SIGNAL(dateTimeChanged(const QDateTime&)), this, SLOT(dtendChanged(const QDateTime&)));
}

InfoPanel::~InfoPanel()
{
    delete ui;
    delete worker;
}

void InfoPanel::dtstartChanged(const QDateTime& dt)
{
    ui->dtend->setMinimumDateTime(dt);
    proxy->setStartDate(dt);
}

void InfoPanel::dtendChanged(const QDateTime& dt)
{
    ui->dtstart->setMaximumDateTime(dt);
    proxy->setEndDate(dt);
}

void InfoPanel::stop()
{
    if (!worker->isFinished())
    {
        worker->requestInterruption();
        worker->wait();
    }
}

void InfoPanel::pathFound(Path p)
{
    QDateTime when = p.getFirst().when;
    if (!dtstart.isValid() || when < dtstart) {
        dtstart = when;
        ui->dtstart->setDateTime(dtstart);
        ui->dtstart->setMinimumDateTime(dtstart);
        ui->dtend->setMinimumDateTime(dtstart);
    }
    if (!dtend.isValid() || when > dtend)
    {
        dtend = when;
        ui->dtend->setDateTime(dtend);
        ui->dtend->setMaximumDateTime(dtend);
        ui->dtstart->setMaximumDateTime(dtend);
    }
    model->append(p);
}

void InfoPanel::detailFound(double duration, double distance)
{
    ui->duration->setText(QString::number(duration, 'f', 3));
    ui->distance->setText(QString::number(distance, 'f', 3));
}

PathTableModel::PathTableModel(QObject* parent):
    QAbstractTableModel(parent) {}

int PathTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : paths.size();
}

int PathTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 4;
}

QVariant PathTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    if (index.row() >= paths.size() || index.row() < 0)
    {
        return QVariant();
    }
    if (role == Qt::DisplayRole)
    {
        Path p = paths.at(index.row());
        switch (index.column())
        {
        case 0:
            return p.getFirst().when;
        case 1:
            return p.getLast().when;
        case 2:
             return p.elapsed();
        case 3:
            return p.distance();
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QVariant PathTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }
    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return "starts";
        case 1:
            return "ends";
        case 2:
            return "duration";
        case 3:
            return "distance";
        default:
            return QVariant();
        }
    }
    return QVariant();
}

void PathTableModel::append(Path p)
{
    beginInsertRows(QModelIndex(), paths.size(), paths.size());
    paths.append(p);
    endInsertRows();
}

DateTimeFilterProxyModel::DateTimeFilterProxyModel(QObject* parent): QSortFilterProxyModel(parent)
{}

bool DateTimeFilterProxyModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    QModelIndex ix = sourceModel()->index(row, 0, parent);
    return dateInRange(sourceModel()->data(ix).toDateTime());
}

bool DateTimeFilterProxyModel::dateInRange(QDateTime dt) const
{
    return dt >= dtstart && dt <= dtend;
}

