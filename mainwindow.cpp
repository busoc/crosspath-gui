#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "areadialog.h"
#include "statsdialog.h"
#include "points.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    ui->content->setLayout(layout);

    setupMenus();
    readSettings();
    updateRecentFiles();
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupMenus()
{
    ui->menuMain->addAction(ui->actionOpen);
    ui->menuMain->addSeparator();
    for (int i = 0; i < MaxHistSize; i++)
    {
        QAction* a = new QAction(this);
        a->setEnabled(false);
        a->setText("file");
        connect(a, SIGNAL(triggered()), this, SLOT(openRecentFile()));

        recentFiles[i] = a;
        ui->menuMain->addAction(a);
    }
    ui->menuMain->addSeparator();
    ui->menuMain->addAction(ui->actionExit);

    ui->menuEdit->addAction(ui->actionNew);
    ui->menuEdit->addAction(ui->actionReload);
    ui->menuEdit->addSeparator();
    ui->menuEdit->addAction(ui->actionStats);

    ui->menuView->addAction(ui->actionDHM);
    ui->menuView->addSeparator();
    ui->menuView->addAction(ui->actionScreen);

    ui->menuhelp->addAction(ui->actionHelp);

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exit()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(createArea()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(ui->actionReload, SIGNAL(triggered()), this, SLOT(reloadFile()));
    connect(ui->actionStats, SIGNAL(triggered()), this, SLOT(showStats()));
    connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(showHelp()));
    connect(ui->actionScreen, SIGNAL(triggered()), this, SLOT(toggleFullscreen()));
    connect(ui->actionDHM, SIGNAL(triggered()), this, SLOT(toggleCoordinates()));

    ui->actionReload->setEnabled(false);
    ui->actionStats->setEnabled(false);
}

void MainWindow::writeSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "busoc", "crosspath-gui");
    settings.setValue("trajectory", trajectory);

    QLayout* layout = ui->content->layout();
    settings.beginWriteArray("areas", layout->count());
    for (int i = 0; i < layout->count(); i++)
    {
        settings.setArrayIndex(i);

        CrossingPanel* panel = qobject_cast<CrossingPanel*>(layout->itemAt(i)->widget());
        const auto a = panel->getArea();

        settings.setValue("label", a.label);
        settings.setValue("latitude", a.latitude);
        settings.setValue("longitude", a.longitude);
        settings.setValue("margin", a.margin);
        settings.setValue("dtstart", a.dtstart);
        settings.setValue("dtend", a.dtend);
        settings.setValue("day", a.day);
        settings.setValue("night", a.night);
    }
    settings.endArray();

    settings.setValue("history", history);
}

void MainWindow::readSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "busoc", "crosspath-gui");
    trajectory = settings.value("trajectory").toString();
    int size = settings.beginReadArray("areas");
    for (int i = 0; i < size; i++)
    {
        settings.setArrayIndex(i);

        Area a;
        a.label = settings.value("label").toString();
        a.latitude = settings.value("latitude").toDouble();
        a.longitude = settings.value("longitude").toDouble();
        a.margin= settings.value("margin").toDouble();
        a.dtstart = settings.value("dtstart").toDateTime();
        a.dtend= settings.value("dtend").toDateTime();
        a.day = settings.value("day").toBool();
        a.night = settings.value("night").toBool();

        CrossingPanel* panel = new CrossingPanel(a);
        ui->content->layout()->addWidget(panel);

        connect(this, SIGNAL(fileChanged(QString)), panel, SLOT(updatePaths(QString)));
        connect(panel, SIGNAL(panelClose()), this, SLOT(closePanel()));
        connect(panel, SIGNAL(panelUpdate()), this, SLOT(updatePanel()));
        connect(this, SIGNAL(toggleCoordinates(bool)), panel, SLOT(updateDMS(bool)));
        panel->updatePaths(trajectory);
    }
    settings.endArray();

    history = settings.value("history").toStringList();
    if (history.size() > 0 && history[0] != trajectory)
    {
        history.prepend(trajectory);
    }

    if (!trajectory.isEmpty())
    {
        ui->actionReload->setEnabled(true);
        ui->actionStats->setEnabled(true);
    }
}

void MainWindow::toggleCoordinates()
{
    emit toggleCoordinates(ui->actionDHM->isChecked());
}

void MainWindow::updateRecentFiles()
{
    int size = qMin(history.size(), (int)MaxHistSize);
    for (int i = 0; i < size; i++)
    {
        QString label = QString("%1) %2").arg(i+1).arg(QFileInfo(history[i]).fileName());
        recentFiles[i]->setText(label);
        recentFiles[i]->setData(history[i]);
        recentFiles[i]->setEnabled(true);
    }
    for (int i = size; i < MaxHistSize; i++)
    {
        recentFiles[i]->setVisible(false);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    writeSettings();
}

void MainWindow::closePanel()
{
    CrossingPanel* panel = qobject_cast<CrossingPanel*>(sender());
    if (!panel)
    {
        return;
    }
    panel->close();
    ui->content->layout()->removeWidget(panel);
    delete panel;
}

void MainWindow::updatePanel()
{
    CrossingPanel* panel = qobject_cast<CrossingPanel*>(sender());
    if (!panel)
    {
        return;
    }
    panel->updatePaths(trajectory);
}

void MainWindow::exit()
{
    writeSettings();
    QApplication::quit();
}

void MainWindow::reloadFile()
{
    if (trajectory.isEmpty()) {
        return ;
    }
    emit fileChanged(trajectory);
}

void MainWindow::readFile(QString filename)
{
    emit fileChanged(filename);
    trajectory = filename;

    ui->actionReload->setEnabled(true);
    ui->actionStats->setEnabled(true);

    if (history.size() >= MaxHistSize)
    {
        history.removeLast();
    }
    history.removeAll(trajectory);
    history.prepend(trajectory);
    qDebug() << history;

    updateRecentFiles();
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        readFile(action->data().toString());
    }
}

void MainWindow::openFile()
{
    QString dir = QDir::homePath();
    if (!trajectory.isEmpty())
    {
        QFileInfo fi(trajectory);
        dir = fi.absoluteDir().absolutePath();
    }
    QString filename = QFileDialog::getOpenFileName(this, "select trajectory", dir);
    if (filename.isEmpty() || filename == trajectory) {
        return;
    }
    readFile(filename);
}

void MainWindow::showStats()
{
    StatsDialog dialog(trajectory, this);
    dialog.exec();
}

void MainWindow::showHelp()
{
    QMessageBox::about(this, "crosspath-gui", "help text: TBW");
}

void MainWindow::toggleFullscreen()
{
    isFullScreen() ? showNormal() : showFullScreen();
}

void MainWindow::createArea()
{
    AreaDialog dialog(this);
    int result = dialog.exec();

    if (result == QDialog::Accepted)
    {
        Area a;
        a.day = dialog.getDay();
        a.night = dialog.getNight();
        a.label = dialog.getLabel();
        a.latitude = dialog.getLatitude();
        a.longitude = dialog.getLongitude();
        a.margin = dialog.getMargin();

        a.dtstart = dialog.getStarts();
        a.dtend = dialog.getEnds();

        if (a.label == "") {
            QLayout* layout = ui->content->layout();
            a.label = QString("%1-%2").arg("area").arg(layout->count());
        }

        CrossingPanel* panel = new CrossingPanel(a);
        ui->content->layout()->addWidget(panel);

        connect(this, SIGNAL(fileChanged(QString)), panel, SLOT(updatePaths(QString)));
        connect(panel, SIGNAL(panelClose()), this, SLOT(closePanel()));
        connect(panel, SIGNAL(panelUpdate()), this, SLOT(updatePanel()));
        connect(this, SIGNAL(toggleCoordinates(bool)), panel, SLOT(updateDMS(bool)));

        panel->updatePaths(trajectory);
    }
}

