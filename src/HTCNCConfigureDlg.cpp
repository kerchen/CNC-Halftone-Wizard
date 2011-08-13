//
// Copyright 2011 Paul Kerchen
// Released under GNU GPL.  Enjoy.
//

#include "HTCNCConfigureDlg.h"

#include <QFileDialog>
#include <QRegExp>
#include <QMessageBox>


namespace HTCNCUI
{
	ConfigureDlg::
	ConfigureDlg(QWidget *parent)
	: QDialog( parent )
	{
		m_ui.setupUi(this);

	connect( m_ui.m_browseSourceToolButton,
		SIGNAL(clicked(bool)),
		SLOT(browseSourceButtonClicked()));

	}

	void ConfigureDlg::setStartingDirName( const QString& name )
	{
		m_ui.m_sourceDirLineEdit->setText(name);
	}
	
	void ConfigureDlg::setGoodSubdirName( const QString& name )
	{
		m_ui.m_goodDirLineEdit->setText(name);
	}
	
	void ConfigureDlg::setBadSubdirName( const QString& name )
	{
		m_ui.m_badDirLineEdit->setText(name);
	}
	
	QString	ConfigureDlg::getStartingDirName()
	{
		return m_ui.m_sourceDirLineEdit->text().trimmed();
	}
	
	
	QString	ConfigureDlg::getGoodSubdirName()
	{
		return m_ui.m_goodDirLineEdit->text().trimmed();
	}
	
	
	QString ConfigureDlg::getBadSubdirName()
	{
		return m_ui.m_badDirLineEdit->text().trimmed();
	}

	void ConfigureDlg::browseSourceButtonClicked() 
	{
		QString			startDir( m_ui.m_sourceDirLineEdit->text() );
		QFileDialog	dialog(this, tr("Select a directory"), startDir);

		dialog.setFileMode( QFileDialog::Directory );
		dialog.setAcceptMode( QFileDialog::AcceptOpen );

		if ( dialog.exec() )
		{
			QStringList	filenames = dialog.selectedFiles();

			m_ui.m_sourceDirLineEdit->setText( filenames[0] );
		}
	}

	void ConfigureDlg::accept()
	{
		QFileInfo	fi( m_ui.m_sourceDirLineEdit->text().trimmed() );
		
		if ( ! fi.exists() || ! fi.isDir() )
		{
			QMessageBox::warning( this, tr("Error"), tr("The specified source directory does not exist."));
			return;
		}
		
		/// Checking for valid filenames is tricky.  Due a rudimentary check, disallowing
		/// anything not in the set of characters that are valid under most modern OS platforms.
		QRegExp	invalidChars( "[^A-Za-z0-9 ._-()[\\]]" );
		QString	subdir( m_ui.m_goodDirLineEdit->text().trimmed() );

		if ( subdir.contains( invalidChars ) )
		{
			QMessageBox::warning( this, tr("Error"), tr("The specified 'good' subdirectory name contains illegal characters."));
			return;
		}
		
		subdir = m_ui.m_goodDirLineEdit->text().trimmed();

		if ( subdir.contains( invalidChars ) )
		{
			QMessageBox::warning( this, tr("Error"), tr("The specified 'bad' subdirectory name contains illegal characters."));
			return;
		}
		QDialog::accept();
	}
	
}	// namespace HTCNCUI


