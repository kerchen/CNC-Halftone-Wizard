//
// Copyright 2011 Paul Kerchen
// Released under GNU GPL.  Enjoy.
//



#include "HTCNCConsole.h"

#include <QPlainTextEdit>
#include <QDateTime>


HTCNC::Console* HTCNC::Console::s_instance = NULL;

namespace HTCNC
{

void Console::setMessageSink( QPlainTextEdit* sink )
{
	m_sink = sink;
}

void Console::appendText( const QString& msg )
{
	if ( m_nextMessageSeverity != ALWAYS &&
		   ( m_severityOutputThreshold == NONE || m_nextMessageSeverity == NONE ) )
		return;

	if ( m_sink )
	{
		if ( m_nextMessageSeverity >= m_severityOutputThreshold )
		{
			QString	completeMessage;
			bool		hasHTML(false);

			m_sink->moveCursor( QTextCursor::End );
			if ( m_printTimestamp )
			{
				QDateTime	t( QDateTime::currentDateTime() );
				completeMessage += t.toString("hh:mm:ss");
				if ( m_nextMessageSeverity == DEBUG )
					completeMessage += QObject::tr(" (DEBUG): ");
				else if ( m_nextMessageSeverity == WARN )
				{
					completeMessage += "<b><font color=\"#FFA000\">";
					completeMessage += QObject::tr(" (WARN): ");
					completeMessage += "</font></b>";
					hasHTML = true;
				}
				else if ( m_nextMessageSeverity == FATAL )
				{
					completeMessage += "<b><font color=\"#FF0000\">";
					completeMessage += QObject::tr(" (ERROR): ");
					completeMessage += "</font></b>";
					hasHTML = true;
				}
				else if ( m_nextMessageSeverity == ALWAYS )
					completeMessage += QObject::tr(": ");
				m_printTimestamp = false;
			}
			completeMessage += msg;
			if (hasHTML)
			{
				m_sink->appendHtml(completeMessage);
				m_lastTextWasHTML = true;
			}
			else
			{
				if ( m_lastTextWasHTML )
				{
					m_sink->insertPlainText("\n");
					m_sink->moveCursor( QTextCursor::End );
					m_lastTextWasHTML = false;
				}
				m_sink->insertPlainText(completeMessage);
			}
			m_sink->ensureCursorVisible();
		}
	}
}


HTCNC::Console& Console::operator<<( const QString& msg )
{
	appendText(msg);
	return *this;
}


HTCNC::Console::Console()
	: m_sink(NULL)
	, m_nextMessageSeverity(DEBUG)
	, m_severityOutputThreshold(DEBUG) 
	, m_printTimestamp(true)
	, m_lastTextWasHTML(false)
{
}


}	// namespace HTCNC


