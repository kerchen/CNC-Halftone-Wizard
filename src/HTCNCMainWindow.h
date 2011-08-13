//
// Copyright 2011 Paul Kerchen
// Released under GNU GPL.  Enjoy.
//

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


