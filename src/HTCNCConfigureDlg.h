//
// Copyright 2011 Paul Kerchen
// Released under GNU GPL.  Enjoy.
//


#ifndef HTCNCCONFIGUREDLG_H
#define HTCNCCONFIGUREDLG_H

#include <QDialog>

#include "ui_ConfigureDlg.h"

// Forward declarations
class QString;



namespace HTCNCUI
{

class ConfigureDlg : public QDialog
{
	Q_OBJECT

public:
	/**@brief Standard constructor.	 
	 */
	ConfigureDlg(QWidget *parent = 0);

	void setStartingDirName( const QString& name );
	void setGoodSubdirName( const QString& name );
	void setBadSubdirName( const QString& name );
	
	QString	getStartingDirName();
	QString	getGoodSubdirName();
	QString getBadSubdirName();
	
protected slots:
	void browseSourceButtonClicked();
	
protected:
	/// Override base functionality to allow for error checking.
	void accept();
	
	/// The Designer-built UI object.
	Ui::ConfigureDlg m_ui;
	

};

}	// namespace HTCNCUI

#endif	// HTCNCCONFIGUREDLG_H


