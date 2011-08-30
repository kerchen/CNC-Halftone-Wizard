/******************************************************************************
* CNC Halftone Wizard
* Copyright (C) 2011 Paul Kerchen
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

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


