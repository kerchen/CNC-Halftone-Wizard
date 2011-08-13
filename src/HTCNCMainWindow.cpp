//
// Copyright 2011 Paul Kerchen
// Released under GNU GPL.  Enjoy.
//




#include <QUiLoader>
#include <QMessageBox>
#include <QLabel>
#include <QCloseEvent>
#include <QFileDialog>
#include <QTextStream>
#include <QShortcut>
#include <QTime>
#include <QFileInfo>

#include "HTCNCMainWindow.h"
#include "HTCNCConsole.h"
#include "HTCNCConfigureDlg.h"
#include "HTCNCHalftoner.h"

#include <assert.h>

static const QString	VERSION_STR( "0.1.0" );

using namespace HTCNC;

namespace HTCNCUI
{


MainWindow::MainWindow(QWidget *parent)
: QMainWindow( parent )
{
	m_ui.setupUi(this);

	QFont	f( "courier" );
	m_ui.m_logTextEdit->setFont(f);
	
	Console::Instance().setMessageSink( m_ui.m_logTextEdit );
	// Change WARN to DEBUG to get debugging messages in log window.
	Console::Instance().setSeverityThreshold( Console::WARN );	
	
	Console::Instance( Console::ALWAYS ) << tr("Greets from The CNC Halftoner, version %1.\n").arg(VERSION_STR);

	
	m_outputImageLabel = new QLabel();
	m_sourceImageLabel = new QLabel();

	m_ui.m_outputScrollArea->setWidget(m_outputImageLabel);
	m_ui.m_sourceScrollArea->setWidget(m_sourceImageLabel);

	connect(m_ui.actionOpen,
		SIGNAL(triggered()),
		SLOT(onOpenActionTriggered()));

	connect(m_ui.actionGenerateGCode,
		SIGNAL(triggered()),
		SLOT(onGenerateGCodeActionTriggered()));

	connect(m_ui.actionExit,
		SIGNAL(triggered()),
		SLOT(onExitActionTriggered()));

	connect(m_ui.m_zoomPreviewSlider,
				SIGNAL( valueChanged(int) ),
				SLOT(recomputeOutput()));

	connect(m_ui.m_stepSpinBox,
				SIGNAL( valueChanged(int) ),
				SLOT(recomputeOutput()));

	connect(m_ui.m_minDotGapLineEdit,
				SIGNAL( editingFinished() ),
				SLOT(recomputeOutput()));

	connect(m_ui.m_depthPercentageSpinBox,
				SIGNAL( valueChanged(int) ),
				SLOT(recomputeOutput()));

	connect(m_ui.m_toolDepthLineEdit,
				SIGNAL( editingFinished() ),
				SLOT(recomputeOutput()));

	connect(m_ui.m_toolWidthLineEdit,
				SIGNAL( editingFinished() ),
				SLOT(recomputeOutput()));

	//QMessageBox::about(this, "hi", "hi" );
}


MainWindow::~MainWindow()
{
}

QMenu * MainWindow::createPopupMenu()
{
	QMenu* myMenu = new QMenu();
#if 0
	myMenu->addAction(m_ui.actionToggleToolboxWindow);
	myMenu->addAction(m_ui.m_consoleDockWidget->toggleViewAction());
	myMenu->addAction(m_ui.m_statusDockWidget->toggleViewAction());
	myMenu->addAction(m_ui.m_activationDockWidget->toggleViewAction());
	myMenu->addAction(m_ui.m_watchDockWidget->toggleViewAction());
	myMenu->addAction(m_ui.actionToggleExperimentToolbar);
	myMenu->addAction(m_ui.actionToggleScenarioToolbar);
	myMenu->addAction(m_ui.actionToggleWindowToolbar);
#endif
	return(myMenu);
}



void MainWindow::onExitActionTriggered()
{
	close();
}




void MainWindow::onOpenActionTriggered()
{
	QString	filename;

	filename = QFileDialog::getOpenFileName( this, 
									tr("Choose a source image"), 
									QString(), 	// Starting dir
									tr("Images (*.png *.gif *.jpg)") );

	if ( ! filename.isEmpty() )
	{
		m_sourceFilename = filename;

		recomputeOutput();
	}
}


void MainWindow::onGenerateGCodeActionTriggered()
{
	if ( m_sourceFilename.isEmpty() )
	{
		return;
	}

	QString	filename(m_gCodeFilename);

	if ( m_gCodeFilename.isEmpty() )
	{
		QFileInfo	fi( m_sourceFilename );

		filename = fi.absolutePath() + "/" + fi.completeBaseName() + ".ngc";
	}

	filename = QFileDialog::getSaveFileName( this, 
									tr("Specify file to write G Code to"), 
									filename,
									tr("G Code (*.ngc)") );

	if ( ! filename.isEmpty() )
	{
		m_gCodeFilename = filename;

		regenerate(true, m_gCodeFilename);
	}
}



void MainWindow::closeEvent( QCloseEvent* event )
{
	// Copy the log to a file in the starting directory.
	QString	logFilename( "/CNCHalftonerLog_" );
	
	logFilename += QDate::currentDate().toString( "yyyy_MM_dd" );
	logFilename += QTime::currentTime().toString( "_HHmm" );
	logFilename += ".txt";
	
	QFile	logFile( logFilename );
	
	if ( ! logFile.open( QFile::WriteOnly ) )
	{
		if ( QMessageBox::No == QMessageBox::warning( this, tr("File error"), 
									tr("Couldn't write log data to '%1'.\nDo you still wish to exit?").arg( logFilename ), 
									QMessageBox::Yes | QMessageBox::No ) )
		{
			event->ignore();
			return;
		}
	}
	
	QTextStream out(&logFile);
    out << m_ui.m_logTextEdit->toPlainText();
	logFile.close();
	event->accept();
}


void MainWindow::recomputeOutput()
{
	if ( m_sourceFilename.isEmpty() )
		return;
	regenerate(false);
}

void MainWindow::regenerate( bool generateGCode, const QString& filename )
{
	int	scale_factor( m_ui.m_zoomPreviewSlider->value() );
	int	step( m_ui.m_stepSpinBox->value() );
	int depth_percentage( m_ui.m_depthPercentageSpinBox->value() );
	double tool_width( m_ui.m_toolWidthLineEdit->text().toDouble() );
	double tool_depth( m_ui.m_toolDepthLineEdit->text().toDouble() );
	double min_dot_gap( m_ui.m_minDotGapLineEdit->text().toDouble() );
	double fastZ( m_ui.m_fastZLineEdit->text().toDouble() );
	double max_dot_size( tool_width * depth_percentage / 100 );

	QPixmap	src_pm( m_sourceFilename );
	QImage	dst_img( src_pm.width()*scale_factor, src_pm.height()*scale_factor, QImage::Format_RGB32 );
	Halftoner::CNCParameters	params;

	params.m_step = step;
	params.m_fullToolDepth = tool_depth;
	params.m_fullToolWidth = tool_width;
	params.m_maxCutPercent = depth_percentage / 100.0;
	params.m_minDotGap = min_dot_gap;
	params.m_fastZ = fastZ;

	Halftoner	ht( src_pm, dst_img, scale_factor, generateGCode, params );

	int	cut_count( ht.getCutCount() );

	m_sourceImageLabel->setPixmap( src_pm );
	m_outputImageLabel->setPixmap( QPixmap::fromImage(dst_img));

	m_ui.m_outputWidthLabel->setText( QString::number(src_pm.width() * ( max_dot_size + min_dot_gap ) / step));
	m_ui.m_outputHeightLabel->setText( QString::number(src_pm.height() * ( max_dot_size + min_dot_gap ) / step));
	m_ui.m_outputCutsLabel->setText( tr("%1, requiring %2 minutes at 1 second/cut")
									.arg(QString::number(cut_count))
									.arg(QString::number(cut_count/60.0)) );

	if ( generateGCode )
	{
		QFile	file( filename );

		// TODO: Consider getting rid of Text flag.  It causes all line feeds
		// to be replaced with carriage return + line feed under Windows, which
		// is kind of an anachronism and makes the output file less portable
		// (since some linux apps still get heartburn from the CR+LF combo).
		if ( file.open( QIODevice::WriteOnly | QIODevice::Text ) )
		{
			// Write the preamble
			QString	preamble( "(" + tr("Generated by the CNC Halftone Wizard.") + ")\n" );

			preamble += "(";
			preamble += tr("Generated at ");
			preamble += QDateTime::currentDateTime().toString( tr("HH:mm:ss dd MMM yyyy") );
			preamble += ")\n";
			preamble += m_ui.m_gcodePreambleTextEdit->toPlainText();
			preamble += "\n";
			preamble += "F" + QString::number(m_ui.m_feedLineEdit->text().toDouble());
			preamble += "\n";
			preamble += "S" + QString::number(m_ui.m_speedLineEdit->text().toDouble());
			preamble += "\n";

			if ( m_ui.m_coolantCheckBox->isChecked() )
				preamble += "M08\n";

			file.write( preamble.toAscii() );
			file.write( ht.getGCode().toAscii() );

			QString	postamble;
			if ( m_ui.m_coolantCheckBox->isChecked() )
				postamble += "M09\n";
			postamble += "M30\n";

			file.write( postamble.toAscii() );
			file.close();

			Console::Instance( Console::ALWAYS ) << tr("G code written to %1.\n").arg(filename);
		}
		else
		{
			Console::Instance( Console::FATAL ) << tr("Could not open %1 for writing.\n").arg(filename);
		}
	}
}




}; // Namespace HTCNCUI


