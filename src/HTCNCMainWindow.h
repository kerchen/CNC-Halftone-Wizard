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

#ifndef HTCNCMAINWINDOW_H
#define HTCNCMAINWINDOW_H

#include "ui_MainWindow.h"

#include <QDir>
#include <QFileInfo>

#include <map>

// Forward declarations
class QLabel;
class QShortcut;


namespace HTCNCUI
{


/**@brief The main application window. */
class MainWindow : public QMainWindow
{
     Q_OBJECT

public:
	/// Standard constructor
	MainWindow(QWidget *parent = 0);
	/// Standard destructor
	virtual ~MainWindow();

	/**@brief Creates the context menu for the main application window.  
	 * Override of QMainWindow::createPopupMenu().
	 * Creates a popup context menu for the main application window.  The context
	 * menu includes items for toggling the visibility of the Toolbox window,
	 * the application toolbars and the console window.
	 */
	virtual QMenu * createPopupMenu ();

private slots:

	/// Responds to File->Open
	void onOpenActionTriggered();
	/// Responds to File->Generate G Code
	void onGenerateGCodeActionTriggered();
	/// Responds to the user requesting to exit the app.
	void onExitActionTriggered();

	/// Responds to requests to see the application's "about" info.
	//void onAboutActionTriggered();

	//void about();
	
	void recomputeOutput();

signals:

private:
	/**@brief Override of base function. */
	virtual void closeEvent( QCloseEvent* );

	void regenerate( bool generateGCode, const QString& filename = QString() );

	/// The Designer-generated user interface object.
	Ui::MainWindow		m_ui;

	QLabel*						m_sourceImageLabel;
	QLabel*						m_outputImageLabel;

	QString						m_sourceFilename;
	QString						m_gCodeFilename;

}; 

}	// namespace HTCNCUI

#endif	// HTCNCMAINWINDOW_H


