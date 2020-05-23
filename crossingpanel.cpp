#include <QMessageBox>
#include <QTextStream>
#include "crossingpanel.h"
#include "ui_crossingpanel.h"
#include "reader.h"
#include "points.h"
#include "datetimedelegate.h"
#include "dhmdelegate.h"
#include "areadialog.h"

CrossingPanel::CrossingPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrossingPanel)
{
    ui->setupUi(this);
    ui->progress->hide();

    connect(ui->btnRem, SIGNAL(click()), this, SLOT(removeRequested()));
    connect(ui->btnEdit, SIGNAL(click()), this, SLOT(editRequested()));
}

CrossingPanel::CrossingPanel(Area a, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrossingPanel),
    model(new CrossingTableModel),
    area(a)
{
    ui->setupUi(this);
    setupCrossing();
    updateComponents();

    connect(ui->btnRem, SIGNAL(clicked()), this, SLOT(removeRequested()));
    connect(ui->btnEdit, SIGNAL(clicked()), this, SLOT(editRequested()));
}

CrossingPanel::~CrossingPanel()
{
    delete ui;
    stopWorker();
}

void CrossingPanel::stopWorker()
{
    if (running && !worker->isFinished())
    {
            worker->requestInterruption();
            worker->wait();
            delete worker;

            running = false;
    }
}

void CrossingPanel::setupCrossing()
{
    ui->crossing->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->crossing->setItemDelegateForColumn(2, new DateTimeDelegate(this));
    ui->crossing->setItemDelegateForColumn(5, new DateTimeDelegate(this));
    ui->crossing->setModel(model);
    ui->crossing->setAlternatingRowColors(true);
    ui->crossing->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void CrossingPanel::updateComponents(bool dhm)
{
    QString north = !dhm ? QString::number(area.north()) : toDMS(area.north());
    ui->north->setText(north);

    QString south = !dhm ? QString::number(area.south()) : toDMS(area.south());
    ui->south->setText(south);

    QString west = !dhm ? QString::number(area.west()) : toDMS(area.west());
    ui->west->setText(west);

    QString east = !dhm ? QString::number(area.east()) : toDMS(area.east());
    ui->east->setText(east);

    ui->from->setText(area.dtstart.toString("yyyy-MM-dd hh:mm:ss"));
    ui->to->setText(area.dtend.toString("yyyy-MM-dd hh:mm:ss"));

    ui->wrap->setTitle(area.label);
}

void CrossingPanel::editRequested()
{
    AreaDialog* dialog = new AreaDialog(area.label, this);
    dialog->setCoordinates(area.latitude, area.longitude, area.margin);
    dialog->setPeriods(area.dtstart, area.dtend);
    dialog->setNightAndDay(area.night, area.day);

    int result = dialog->exec();
    if (result != QDialog::Accepted)
    {
        return;
    }
    area.latitude = dialog->getLatitude();
    area.longitude = dialog->getLongitude();
    area.margin = dialog->getMargin();
    area.dtstart = dialog->getStarts();
    area.dtend = dialog->getEnds();
    area.day = dialog->getDay();
    area.night = dialog->getNight();

    updateComponents();
    emit panelUpdate();
}

void CrossingPanel::removeRequested()
{
    QString question = QString("Do you want to remove %1 area?").arg(area.label);

    int result = QMessageBox::question(this, "Remove Area", question);
    if (result == QMessageBox::Yes)
    {
        stopWorker();
        emit panelClose();
    }
}

void CrossingPanel::updateDMS(bool dhm)
{
    updateComponents(dhm);
    if (dhm)
    {
        ui->crossing->setItemDelegateForColumn(0, new DMSDelegate(this));
        ui->crossing->setItemDelegateForColumn(1, new DMSDelegate(this));
        ui->crossing->setItemDelegateForColumn(3, new DMSDelegate(this));
        ui->crossing->setItemDelegateForColumn(4, new DMSDelegate(this));
    } else {
        ui->crossing->setItemDelegateForColumn(0, nullptr);
        ui->crossing->setItemDelegateForColumn(1, nullptr);
        ui->crossing->setItemDelegateForColumn(3, nullptr);
        ui->crossing->setItemDelegateForColumn(4, nullptr);
    }
}

void CrossingPanel::pathFound(Path p)
{
    model->append(p);

}

void CrossingPanel::updatePaths(QString file)
{
    if (file.isEmpty())
    {
        return ;
    }
    auto ctm = new CrossingTableModel();
    ui->crossing->setModel(ctm);

    delete model;
    model = ctm;

    stopWorker();

    ui->progress->show();
    ui->progress->setRange(0, 0);

    worker = new CrossingThread(area, file, this);
    connect(worker, SIGNAL(finished()), this, SLOT(workerFinished()));
    connect(worker, SIGNAL(pathFound(const Path&)), this, SLOT(pathFound(const Path&)));
    worker->start();
    running = true;
}

void CrossingPanel::workerFinished()
{
    ui->progress->hide();
}

CrossingTableModel::CrossingTableModel(QObject *parent): QAbstractTableModel(parent) {};

CrossingTableModel::CrossingTableModel(const QVector<Path> &ps, QObject *parent): QAbstractTableModel(parent), paths(ps) {};

int CrossingTableModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : paths.size();
};

int CrossingTableModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : 8;
};

QVariant CrossingTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.row() >= paths.size() || index.row() < 0) {
        return QVariant();
    }
    if (role == Qt::DisplayRole) {
        Path p = paths.at(index.row());
        switch (index.column()) {
        case 0:
            return p.getFirst().lat;
        case 1:
            return p.getFirst().lng;
        case 2:
            return p.getFirst().when;
        case 3:
            return p.getLast().lat;
        case 4:
            return p.getLast().lng;
        case 5:
            return p.getLast().when;
        case 6:
            return p.elapsed();
        case 7:
            return p.distance();
        default:
            return QVariant();
        }
    }
    return QVariant();
};

QVariant CrossingTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return "latitude";
        case 1:
            return "longitude";
        case 2:
            return "time";
        case 3:
            return "latitude";
        case 4:
            return "longitude";
        case 5:
            return "time";
        case 6:
            return "duration";
        case 7:
            return "distance";
        default:
            return QVariant();
        }
    }
    return QVariant();
};


const QVector<Path>& CrossingTableModel::getPaths() const {
    return paths;
};

void CrossingTableModel::append(Path p) {
    beginInsertRows(QModelIndex(), paths.size(), paths.size());
    paths.append(p);
    endInsertRows();
}
