#include "mainwindow.h"
#include <QApplication>
#include "stdafx.h"
#include <windows.h>
#include "FSUIPC_User.h"

int main(int argc, char *argv[])
{
	DWORD dwResult;
	
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
