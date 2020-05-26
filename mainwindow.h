#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "crossingpanel.h"
#include "points.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void createArea();
    void openFile();
    void openRecentFile();
    void reloadFile();
    void showStats();
    void showHelp();
    void exit();
    void toggleFullscreen();
    void closePanel();
    void updatePanel();
    void toggleDMS();

signals:
    void fileChanged(QString file);
    void toggleCoordinates(bool dhm);

protected:
    void closeEvent(QCloseEvent *event);

private:
    void readFile(QString file);
    void readSettings();
    void writeSettings();
    void setupMenus();
    void updateRecentFiles();
    void createPanel(Area a);

    Ui::MainWindow *ui;
    QString trajectory;
    QStringList history;

    enum {MaxHistSize = 6};
    QAction* recentFiles[MaxHistSize];
};
#endif // MAINWINDOW_H
