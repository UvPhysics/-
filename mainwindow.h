#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pigfarm.h"
#include "QToolButton"
#include "QDebug"
#include "QPainter"
#include "QTimer"
#include "QTabWidget"
#include "QWidget"
#include "QMessageBox"
#include "QDialog"
#include "QPlainTextEdit"
#include "QInputDialog"
#include "QSpinBox"
#include "QFormLayout"
#include "QDialogButtonBox"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDataStream>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    PigFarm * myFarm=new PigFarm;
    QToolButton* styButtons[100];
    QString logInfo;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QTimer * timerDay;
    //把猪圈图标放到tabWidget
    void setStyButtons();
    //然后做个批量链接
    void connectStyButtons();

    //更新猪圈感染图标
    void connectStyButtonsInfected();
    //更新猪圈全是健康猪的图标
    void connectStyButtonsHealthy();
    //更新猪圈没有猪的图标
    void connectStyButtonsNoPig();
    //自动读取信息并显示
    void autoRead();
    //自动保存信息
    void autoSave();



private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
