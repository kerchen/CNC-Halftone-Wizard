//
// Copyright 2011 Paul Kerchen
// Released under GNU GPL.  Enjoy.
//

#include "HTCNCMainWindow.h"
#include "HTCNCConsole.h"

#include "ui_MainWindow.h"

#include <QMessageBox>


#include <iostream>
#include <fstream>

//#include <windows.h>
//#include <Wincon.h>


void qtMessageHandler(QtMsgType type, const char *msg)
{
	HTCNC::Console&	con = HTCNC::Console::Instance();

	switch (type) {
	case QtDebugMsg:
		con << "QtDebug: ";
		break;
	case QtWarningMsg:
		con << "QtWarning: ";
		break;
	case QtCriticalMsg:
		con << "QtCritical: ";
		break;
	case QtFatalMsg:
		con << "QtFatal: ";
		break;
	}
	con << msg << "\n";
}


int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("WhirlingChair");
	QCoreApplication::setOrganizationDomain("whirlingchair.com");
	QCoreApplication::setApplicationName("CNC Halftone Wizard");

	QApplication app(argc, argv);
	HTCNCUI::MainWindow mainWindow;

	qInstallMsgHandler(qtMessageHandler);

	//AllocConsole(); 
	//freopen("conin$","r",stdin); 
	//freopen("conout$","w",stdout); 
	//freopen("conout$","w",stderr);

	mainWindow.show();

	return app.exec();
} 


