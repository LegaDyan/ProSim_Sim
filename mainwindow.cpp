#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "stdafx.h"
#include <windows.h>
#include "FSUIPC_User.h"
#include "SimConnect.h"

#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

int quit = 0;
HANDLE  hSimConnect = NULL;

struct Struct1
{
	char    title[256];
	double  kohlsmann;
	double  altAboveG;
	double  latitude;
	double  longitude;
	double  trueHdg;
	double  magHdg;
};

double trueHdg;

static enum EVENT_ID {
	EVENT_SIM_START,
};

static enum DATA_DEFINE_ID {
	DEFINITION_1,
};

static enum DATA_REQUEST_ID {
	REQUEST_1,
};

void CALLBACK MyDispatchProcRD(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext)
{
	HRESULT hr;

	switch (pData->dwID)
	{
	case SIMCONNECT_RECV_ID_EVENT:
	{
		SIMCONNECT_RECV_EVENT *evt = (SIMCONNECT_RECV_EVENT*)pData;

		switch (evt->uEventID)
		{
		case EVENT_SIM_START:

			// Now the sim is running, request information on the user aircraft
			hr = SimConnect_RequestDataOnSimObjectType(hSimConnect, REQUEST_1, DEFINITION_1, 0, SIMCONNECT_SIMOBJECT_TYPE_USER);

			break;

		default:
			break;
		}
		break;
	}

	case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE:
	{
		SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE*)pData;

		switch (pObjData->dwRequestID)
		{
		case REQUEST_1:
		{
			DWORD ObjectID = pObjData->dwObjectID;
			Struct1 *pS = (Struct1*)&pObjData->dwData;
			if (SUCCEEDED(StringCbLengthA(&pS->title[0], sizeof(pS->title), NULL))) // security check
			{
				qDebug() <<  pS->trueHdg;
				trueHdg = pS->trueHdg;
				quit = 1;
			}
			break;
		}

		default:
			break;
		}
		break;
	}


	case SIMCONNECT_RECV_ID_QUIT:
	{
		quit = 1;
		break;
	}

	default:
		printf("\nReceived:%d", pData->dwID);
		break;
	}
}

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
	printf("Hahaha");
}

void MainWindow::on_pushButton_2_clicked()
{
	HRESULT hr;

	if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Request Data", NULL, 0, 0, 0)))
	{
		qDebug() << ("\nConnected to Flight Simulator!");

		quit = 0;
		// Set up the data definition, but do not yet do anything with it
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Title", NULL, SIMCONNECT_DATATYPE_STRING256);
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Kohlsman setting hg", "inHg");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "PLANE ALT ABOVE GROUND", "feet");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Plane Latitude", "degrees");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Plane Longitude", "degrees");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Plane Heading Degrees True", "degrees");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Plane Heading Degrees Magnetic", "degrees");

		// Request an event when the simulation starts
		hr = SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_START, "sim");

		while (0 == quit)
		{
			SimConnect_CallDispatch(hSimConnect, MyDispatchProcRD, NULL);
		}

		//while (0 == quit) {
		//	hr = SimConnect_CallDispatch(hSimConnect, MyDispatchProcRD, NULL);
		//	Sleep(1000);
		//}

		hr = SimConnect_Close(hSimConnect);
	}
	ui->label_2->setText(QString::number(int(trueHdg), 10));
}
