#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"highlighter.h"
#include"QDebug"
#include<iostream>
#include<fstream>
#include<ctime>
#include<windows.h>
#include<time.h>
#include<ostream>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_execute_clicked();

    void on_clear_clicked();



private:
    Ui::MainWindow *ui;
    Highlighter *highlighter;
    void printRecord();
    void printInfo();
    void printerror();
    int recordnum;
    clock_t begin,end;
};

#endif // MAINWINDOW_H
