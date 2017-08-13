#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "stdafx.h"
#include <windows.h>
#include "FSUIPC_User.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	unsigned int speed;
	DWORD dwResult;
	int sta = FSUIPC_Open(SIM_ANY, &dwResult);
	
	FSUIPC_Read(0x02BC, 4, &speed, &dwResult);
	FSUIPC_Process(&dwResult);
	ui->label->setText(QString::number(speed, 10));
	// speed = 100;
	// ui->label->setText(QString::number(speed, 10));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    ui->label->setText(QString::number(value, 10));
    qDebug() << value;
}

void MainWindow::on_pushButton_clicked()
{
    DWORD dwResult;
    unsigned int speed;
    int sta = FSUIPC_Open(SIM_ANY, &dwResult);

    FSUIPC_Read(0x02BC, 4, &speed, &dwResult);
    FSUIPC_Process(&dwResult);
    ui->label->setText(QString::number(speed, 10));
}
