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


