#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPixmap"
#include "QBitmap"
using namespace std;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    highlighter = new Highlighter(ui->editer->document());
    ui->record->setAlternatingRowColors(true);
    ui->record->setStyleSheet("alternate-background-color:#C1CBD7;background:white");
    ui->show->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_execute_clicked()
{
    recordnum=0;
    QString all_input=ui->editer->document()->toPlainText();
    all_input=all_input+"quit;";
    ofstream outfile("tmp.in",ios::trunc);
    outfile<<all_input.toStdString();
    outfile.close();
    begin=clock();
    system("miniSQL1.exe");
    end=clock();
    printRecord();
    printInfo();
    printerror();
    system("DEL /f *.out");
    system("DEL /f *.in");
}

void MainWindow::printRecord(){
    //ui->record->setRowCount(valid);

    fstream _file;
    _file.open("data.out",ios::in);
    string str;
    if(!_file.is_open()){
        return;
    }
    ui->show->setVisible(false);
    ui->record->setVisible(true);
    getline(_file,str);
    QStringList attri_lst=QString::fromStdString(str).split(" ");
    ui->record->setColumnCount(attri_lst.size()-1);
    attri_lst.pop_back();
    ui->record->setHorizontalHeaderLabels(attri_lst);
    int row_num=0;
    while(!_file.eof()){
        recordnum++;
        getline(_file,str);
        attri_lst=QString::fromStdString(str).split(" ");
        attri_lst.pop_back();
        ui->record->setRowCount(row_num+1);
        for(int i=0;i<attri_lst.size();i++){
            ui->record->setItem(row_num,i,new QTableWidgetItem(attri_lst[i]));
        }
        row_num++;
    }

    ui->record->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::printInfo(){

    fstream _file;
    _file.open("show.out",ios::in);
    QString info;
    if(!_file.is_open())
        return;
    else{
        ui->show->setVisible(true);
        ui->record->setVisible(false);
        while(!_file.eof()){
            string tmp;
            getline(_file,tmp);
            info=info+QString::fromStdString(tmp)+"\n";
        }
        qDebug()<<info;
        ui->show->setText(info);
    }
}

void MainWindow::on_clear_clicked()
{
    ui->editer->clear();
    ui->record->clear();
    ui->show->clear();
}

void MainWindow::printerror(){
    fstream _file;
    _file.open("tmp.out",ios::in);
    string str;
    int tmprow=ui->error->rowCount();
    ui->error->setRowCount(ui->error->rowCount()+1);
    if(!_file.is_open()){
        SYSTEMTIME sys;
        GetLocalTime(&sys);
        QString tmptime;
        tmptime=QString::number(sys.wHour)+":"+QString::number(sys.wMinute)+":"+QString::number(sys.wSecond);
        ui->error->setItem(tmprow,0,new QTableWidgetItem(tmptime));
        ui->error->setItem(tmprow,1,new QTableWidgetItem(ui->editer->document()->toPlainText()));
        ui->error->setItem(tmprow,2,new QTableWidgetItem(QString::number(recordnum)+" row(s) affected"));
        ui->error->setItem(tmprow,3,new QTableWidgetItem(QString::number(end-begin)));
    }
    else{
        SYSTEMTIME sys;
        GetLocalTime(&sys);
        QString tmptime;
        tmptime=QString::number(sys.wHour)+":"+QString::number(sys.wMinute)+":"+QString::number(sys.wSecond);
        ui->error->setItem(tmprow,0,new QTableWidgetItem(tmptime));
        ui->error->setItem(tmprow,1,new QTableWidgetItem(ui->editer->document()->toPlainText()));
        getline(_file,str);
        getline(_file,str);
        getline(_file,str);
        QString info="Error Code:"+QString::fromStdString(str)+".";
        getline(_file,str);
        info=info+QString::fromStdString(str);
        ui->error->setItem(tmprow,2,new QTableWidgetItem(info));
        ui->error->setItem(tmprow,3,new QTableWidgetItem(QString::number(end-begin)));
    }

}
