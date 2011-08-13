//
// Written by Paul Kerchen
// Released under GNU GPL.  Enjoy.
//




#include "MDApp.h"
#include "TECException.h"
#include "XMLHelp.h"

#if defined(Q_OS_WIN)
#include <shlobj.h>	// For SHGetFolderPath()
#endif

#include "MDClassAPI.h"
#include "MDAPIParser.h"
#include "TECBaseExperiment.h"

#include "JavaScriptInterpreter.h"
#include "scenario.h"

#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qsciapis.h>

#include <QFileInfo>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QTextStream>
#include <QLocale>
#include <QMessageBox>
#include <QCoreApplication>

#include <assert.h>


#if defined(Q_OS_WIN)
#define COMPANY_DIR_NAME "Realtime Technologies"
#define PRODUCT_DIR_NAME "Maneuver Designer"
#elif defined(Q_OS_UNIX)
#define COMPANY_DIR_NAME ".RealtimeTechnologies"
#define PRODUCT_DIR_NAME "Maneuver Designer"
#endif
#define APP_VERSION "0.28.1"

using namespace TEC;


static const QString	gSettingsRootTag( "MDSettings" );

namespace MD
{
App* App::s_instance = NULL;


/// Used in XML data--do not translate.
const QString	gBraceMatchingStyleString[App::BraceMatchingStyleCOUNT] =
{
	"none",
	"strict",
	"sloppy"
};


/// Used in XML data--do not translate.
const QString gMLLIndicatorStyleString[App::MLLIndicatorStyleCOUNT] =
{
	"none",
	"verticalLine",
	"color"
};


/// Used in XML data--do not translate.
const QString gSyntaxItemString[App::SyntaxItemCOUNT] =
{
	"DefaultSyntaxItem",
	"CommentSyntaxItem",
	"NumberSyntaxItem",
	"KeywordSyntaxItem",
	"DoubleQuotedStringSyntaxItem",
	"SingleQuotedStringSyntaxItem",
	"OperatorSyntaxItem",
	"IdentifierSyntaxItem",
	"UnclosedStringSyntaxItem"
};

/// Used in XML data--do not translate.
const QString gLineWrapStyleString[App::LineWrapStyleCOUNT] =
{
	"none",
	"word",
	"character"
};


/// Used in XML data--do not translate.
const QString gLineWrapIndicatorStyleString[App::LineWrapIndicatorStyleCOUNT] =
{
	"indentOnly",
	"text",
	"border"
};

/// Used in XML data--do not translate.
const QString gWhitespaceStyleString[App::WhitespaceStyleCOUNT] =
{
	"invisible",
	"visible",
	"visibleAfterIndent"
};



App::App()
: s_versionString( APP_VERSION )
, m_displayLocale(NULL)
, m_localeLanguage(QLocale::English)
, m_localeCountry(QLocale::UnitedStates)
, m_referenceLexer( new QsciLexerJavaScript )
, m_classNameDocTag("@class")
, m_summaryDocTag("@summary")
, m_descriptionDocTag("@description")
, m_functionOwnerDocTag("@methodOf")
, m_typeDocTag("@type")
, m_braceMatchingStyle( No_BraceMatchingStyle )
, m_MLLIndicatorStyle(No_MLLIndicatorStyle)
, m_maxLineLength(80)
, m_MLLIndicatorColor( Qt::red )
, m_lineWrapStyle( No_LineWrapStyle )
, m_lineWrapIndicatorStyle( IndentOnly_LineWrapIndicatorStyle )
, m_lineWrapIndent(0)
, m_autoIndent(false)
, m_tabSize(3)
, m_indentSize(3)
, m_keepTabs(false)
, m_whitespaceStyle(Invisible_WhitespaceStyle)
, m_showEOL(false)
, m_showLineNumbers(true)
, m_fileDialogStart(LastUsed_FileDialogStart)
, m_fileDialogDirectory(QCoreApplication::applicationDirPath())
, m_useExperimentFile(false)
, m_activeExperiment(new BaseExperiment( "", false, false))
{
	m_font = m_referenceLexer->defaultFont(QsciLexerJavaScript::Default);

	m_classDocTemplate = 
		" * " + m_classNameDocTag + "\n"
		" * @augments Maneuver\n"
		" * " + m_summaryDocTag + "\n"
		" * " + m_descriptionDocTag + "\n";

	m_functionDocTemplate = 
		" * " + m_functionOwnerDocTag + "\n"
		" * " + m_typeDocTag + "\n"
		" * " + m_descriptionDocTag + "\n";

	m_memberVariableDocTemplate = 
		" * " + m_typeDocTag + "\n"
		" * " + m_descriptionDocTag + "\n";

	m_parameterDocTemplate = 
		" * " + m_descriptionDocTag + "\n";

	m_itemForegroundColor[ Default_SyntaxItem ] = m_referenceLexer->defaultColor(QsciLexerJavaScript::Default);
	m_itemForegroundColor[ Comment_SyntaxItem ] = m_referenceLexer->defaultColor(QsciLexerJavaScript::Comment);
	m_itemForegroundColor[ Number_SyntaxItem ] = m_referenceLexer->defaultColor(QsciLexerJavaScript::Number);
	m_itemForegroundColor[ Keyword_SyntaxItem ] = m_referenceLexer->defaultColor(QsciLexerJavaScript::Keyword);
	m_itemForegroundColor[ DoubleQuotedString_SyntaxItem ] = m_referenceLexer->defaultColor(QsciLexerJavaScript::DoubleQuotedString);
	m_itemForegroundColor[ SingleQuotedString_SyntaxItem ] = m_referenceLexer->defaultColor(QsciLexerJavaScript::SingleQuotedString);
	m_itemForegroundColor[ Operator_SyntaxItem ] = m_referenceLexer->defaultColor(QsciLexerJavaScript::Operator);
	m_itemForegroundColor[ Identifier_SyntaxItem ] = m_referenceLexer->defaultColor(QsciLexerJavaScript::Identifier);
	m_itemForegroundColor[ UnclosedString_SyntaxItem ] = m_referenceLexer->defaultColor(QsciLexerJavaScript::UnclosedString);

	m_itemBackgroundColor[ Default_SyntaxItem ] = m_referenceLexer->defaultPaper(QsciLexerJavaScript::Default);
	m_itemBackgroundColor[ Comment_SyntaxItem ] = m_referenceLexer->defaultPaper(QsciLexerJavaScript::Comment);
	m_itemBackgroundColor[ Number_SyntaxItem ] = m_referenceLexer->defaultPaper(QsciLexerJavaScript::Number);
	m_itemBackgroundColor[ Keyword_SyntaxItem ] = m_referenceLexer->defaultPaper(QsciLexerJavaScript::Keyword);
	m_itemBackgroundColor[ DoubleQuotedString_SyntaxItem ] = m_referenceLexer->defaultPaper(QsciLexerJavaScript::DoubleQuotedString);
	m_itemBackgroundColor[ SingleQuotedString_SyntaxItem ] = m_referenceLexer->defaultPaper(QsciLexerJavaScript::SingleQuotedString);
	m_itemBackgroundColor[ Operator_SyntaxItem ] = m_referenceLexer->defaultPaper(QsciLexerJavaScript::Operator);
	m_itemBackgroundColor[ Identifier_SyntaxItem ] = m_referenceLexer->defaultPaper(QsciLexerJavaScript::Identifier);
	m_itemBackgroundColor[ UnclosedString_SyntaxItem ] = m_referenceLexer->defaultPaper(QsciLexerJavaScript::UnclosedString);

	if ( s_languageCodeMap.empty() )
	{
		s_languageCodeMap[ QLocale::German ] = "de";
		s_languageCodeMap[ QLocale::English ] = "en";
		s_languageCodeMap[ QLocale::Chinese ] = "zh";
		s_languageCodeMap[ QLocale::Czech ] = "cs";
		s_languageCodeMap[ QLocale::Danish ] = "da";
		s_languageCodeMap[ QLocale::Dutch ] = "nl";
		s_languageCodeMap[ QLocale::Finnish ] = "fi";
		s_languageCodeMap[ QLocale::French ] = "fr";
		s_languageCodeMap[ QLocale::Greek ] = "el";
		s_languageCodeMap[ QLocale::Italian ] = "it";
		s_languageCodeMap[ QLocale::Japanese ] = "ja";
		s_languageCodeMap[ QLocale::Korean ] = "ko";
		s_languageCodeMap[ QLocale::Polish ] = "pl";
		s_languageCodeMap[ QLocale::Portuguese ] = "pt";
		s_languageCodeMap[ QLocale::Romanian ] = "ro";
		s_languageCodeMap[ QLocale::Russian ] = "ru";
		s_languageCodeMap[ QLocale::Spanish ] = "es";
		s_languageCodeMap[ QLocale::Swedish ] = "sv";
		s_languageCodeMap[ QLocale::Turkish ] = "tr";
	}
	if ( s_countryCodeMap.empty() )
	{
		s_countryCodeMap[ QLocale::Argentina ] = "AR"; 
		s_countryCodeMap[ QLocale::Australia ] = "AU"; 
		s_countryCodeMap[ QLocale::Austria ] = "AT"; 
		s_countryCodeMap[ QLocale::Bahrain ] = "BH"; 
		s_countryCodeMap[ QLocale::Belgium ] = "BE"; 
		s_countryCodeMap[ QLocale::Brazil ] = "BR"; 
		s_countryCodeMap[ QLocale::Canada ] = "CA"; 
		s_countryCodeMap[ QLocale::China ] = "CN"; 
		s_countryCodeMap[ QLocale::Colombia ] = "CO"; 
		s_countryCodeMap[ QLocale::CostaRica ] = "CR"; 
		s_countryCodeMap[ QLocale::Cyprus ] = "CY"; 
		s_countryCodeMap[ QLocale::CzechRepublic ] = "CZ"; 
		s_countryCodeMap[ QLocale::Denmark ] = "DK"; 
		s_countryCodeMap[ QLocale::Finland ] = "FI"; 
		s_countryCodeMap[ QLocale::France ] = "FR"; 
		s_countryCodeMap[ QLocale::MetropolitanFrance ] = "FX"; 
		s_countryCodeMap[ QLocale::Germany ] = "DE"; 
		s_countryCodeMap[ QLocale::Greece ] = "GR"; 
		s_countryCodeMap[ QLocale::Guam ] = "GU"; 
		s_countryCodeMap[ QLocale::HongKong ] = "HK"; 
		s_countryCodeMap[ QLocale::Hungary ] = "HU"; 
		s_countryCodeMap[ QLocale::Iceland ] = "IS"; 
		s_countryCodeMap[ QLocale::India ] = "IN"; 
		s_countryCodeMap[ QLocale::Indonesia ] = "ID"; 
		s_countryCodeMap[ QLocale::Ireland ] = "IE"; 
		s_countryCodeMap[ QLocale::Israel ] = "IL"; 
		s_countryCodeMap[ QLocale::Italy ] = "IT"; 
		s_countryCodeMap[ QLocale::Japan ] = "JP"; 
		s_countryCodeMap[ QLocale::DemocraticRepublicOfKorea ] = "KP"; 
		s_countryCodeMap[ QLocale::RepublicOfKorea ] = "KR"; 
		s_countryCodeMap[ QLocale::Liechtenstein ] = "LI"; 
		s_countryCodeMap[ QLocale::Lithuania ] = "LT"; 
		s_countryCodeMap[ QLocale::Luxembourg ] = "LU"; 
		s_countryCodeMap[ QLocale::Mexico ] = "MX"; 
		s_countryCodeMap[ QLocale::Netherlands ] = "NL"; 
		s_countryCodeMap[ QLocale::NetherlandsAntilles ] = "AN"; 
		s_countryCodeMap[ QLocale::NewZealand ] = "NZ"; 
		s_countryCodeMap[ QLocale::Norway ] = "NO"; 
		s_countryCodeMap[ QLocale::Poland ] = "PL"; 
		s_countryCodeMap[ QLocale::Portugal ] = "PT"; 
		s_countryCodeMap[ QLocale::PuertoRico ] = "PR"; 
		s_countryCodeMap[ QLocale::Romania ] = "RO"; 
		s_countryCodeMap[ QLocale::RussianFederation ] = "RU"; 
		s_countryCodeMap[ QLocale::Spain ] = "ES"; 
		s_countryCodeMap[ QLocale::Sweden ] = "SE"; 
		s_countryCodeMap[ QLocale::Switzerland ] = "CH"; 
		s_countryCodeMap[ QLocale::Taiwan ] = "TW"; 
		s_countryCodeMap[ QLocale::Turkey ] = "TR"; 
		s_countryCodeMap[ QLocale::Ukraine ] = "UA"; 
		s_countryCodeMap[ QLocale::UnitedKingdom ] = "GB"; 
		s_countryCodeMap[ QLocale::UnitedStates ] = "US"; 
		s_countryCodeMap[ QLocale::Yugoslavia ] = "YU"; 
	}
}

App::~App()
{
	saveSettings();
	delete m_displayLocale;
}



bool App::init( QString& failureMsg )
{
	QString	settingsFile( getSettingsFilePath() );

	if ( ! createSettingsFile(false) )
	{
		failureMsg = tr("Could not open application settings file %1").
									arg(settingsFile);
		return false;
	}

	// Read the app settings.
	QDomDocument	doc;
	QFile					f( settingsFile );
	QString				errorStr;
	int						errorLine, errorColumn;

	f.open(QIODevice::ReadOnly);

	if ( ! doc.setContent( &f, true, &errorStr, &errorLine, &errorColumn ) )
	{
		failureMsg = tr("Error parsing application settings file '%1' at line %2, column %3:\n%4\n")
									.arg(settingsFile)
									.arg(errorLine)
									.arg(errorColumn)
									.arg(errorStr);
		return false;
	}

	QDomElement	root = doc.documentElement();
	if ( root.tagName() != gSettingsRootTag )
	{
		failureMsg = tr("The file '%1' does not appear to be a Maneuver Designer settings file.\n")
									.arg(settingsFile);
		return false;
	}

	QString	str = root.attribute( "version" );
	if ( str.isEmpty() )
	{
		failureMsg = tr("The Maneuver Designer settings file '%1' does not have a version number.\n")
								.arg(settingsFile);
		return false;
	}

	QDomNode	node = root.firstChild();
	QString		errorMsg;

	while ( ! node.isNull() )
	{
		QDomElement const& element = node.toElement();

		if ( element.tagName() == "App" )
		{
			if ( ! parseAppNode( element, errorMsg ) || ! errorMsg.isEmpty()  )
			{
				failureMsg = tr("The Maneuver Designer settings file %1 has one or more errors in it:\n%1\n")
											.arg(errorMsg);
				return false;
			}
		}
		node = node.nextSibling();
	}
	
	// No locale was defined in the settings file, so use good ol' "C" locale.
	if ( ! m_displayLocale )
		m_displayLocale = new QLocale("C");

	// Nobody wants to see the thousands separator, right?
	m_displayLocale->setNumberOptions(QLocale::OmitGroupSeparator);

	// Set the default locale to "C" so that we don't get hosed by the OS locale.
	QLocale	defaultLocale("C");
	QLocale::setDefault(defaultLocale);

	// Populate the class APIs
	QDir		apiDir( getInstallDir() + "api" );
	QStringList	filters( "*.xml");
	QStringList	apiFiles( apiDir.entryList( filters, QDir::Files ) );

	for ( int i = 0; i < apiFiles.count(); ++i )
	{
		QString	apiFilename( getInstallDir() + "api/" + apiFiles[i] );

		try
		{
			APIParser	apiParser( apiFilename, m_referenceLexer );
			const APIParser::ClassAPIMap&	apiMap( apiParser.getAPIMap() );
			APIParser::ClassAPIMap::const_iterator it( apiMap.begin() );

			while ( it != apiMap.end() )
			{
				if ( m_classAPIMap.find((*it).first) != m_classAPIMap.end() )
				{
					failureMsg = tr("Duplicate class '%1' found in API data.").arg((*it).first);
					return false;
				}
				m_classAPIMap[ (*it).first ] = (*it).second;
				++it;
			}
		}
		catch( Exception& e )
		{
			failureMsg = e.what();
			return false;
		}
	}

	// Set the active experiment.  Failure to load the experiment isn't fatal, but
	// failureMsg should be non-empty in that case.
	if ( m_useExperimentFile )
		setActiveExperiment( m_experimentFilename, failureMsg );

	//QMessageBox::warning(this, "attach to me", "attach to me" );

	// Create a new javascript interpreter and a context for it
  m_jsInterp = new tec::JavaScriptInterpreter;
  m_jsContext = m_jsInterp->NewContext();

  // Initialize the javascript Scenario object in SimCreator
  jsScenarioInit(m_jsContext->jsContext, m_jsContext->jsGlobal);

	return true;
}


bool App::setActiveExperiment( const QString& filename, QString& failureMsg )
{
	if ( filename.isEmpty() )
	{
		// User is clearing the active experiment.
		if ( m_activeExperiment )
		{
			// If the current experiment is already the "empty" experiment, we're done.
			if ( m_activeExperiment->getSaveFilename().isEmpty() )
				return true;

			// Otherwise, blow away the current experiment and recreate the "empty" experiment.
			delete m_activeExperiment;
		}
		m_activeExperiment = new BaseExperiment( "", false, false );
		return true;
	}

	try
	{
		TEC::BaseExperiment*	exp( new BaseExperiment( filename, true, true ) );
		if ( m_activeExperiment )
			delete m_activeExperiment;
		m_activeExperiment = exp;
		m_experimentFilename = filename;
		m_useExperimentFile = true;
		return true;
	}
	catch( TEC::Exception& e )
	{
		failureMsg = e.what();
		return false;
	}
}


bool App::parseAppNode( const QDomElement& root, QString& errorMsg )
{
	QString	displayLocaleStr("en_US");

	QDomNode	node = root.firstChild();
	while ( ! node.isNull() )
	{
		const QDomElement& element = node.toElement();

		if ( element.tagName() == "Locale" )
			displayLocaleStr = element.text().trimmed();
		else if ( element.tagName() == "ExperimentFile" )
		{
			m_useExperimentFile = true;
			m_experimentFilename = element.text().trimmed();
		}
		else if ( element.tagName() == "EditorPrefs" )
		{
			try {
				parseEditorPrefsNode( element );
			}
			catch ( Exception& e )
			{
				errorMsg += tr("Error while parsing editor preferences:\n");
				errorMsg += e.what();
				return false;
			}
		}
		node = node.nextSibling();
	}

	// String should be of the form 'll_CC', where ll is the two-character
	// ISO 639 language code and CC is the two-character ISO 3166 country
	// code.
	m_displayLocale = new QLocale(displayLocaleStr);

	QStringList	locList = displayLocaleStr.split( "_" );

	if ( locList.count() > 1 )
	{
		for(std::map< QLocale::Language, QString >::const_iterator it = s_languageCodeMap.begin();
			it != s_languageCodeMap.end();
			++it )
		{
			if ( locList[0] == (*it).second )
			{
				m_localeLanguage = (*it).first;
				break;
			}
		}

		for(std::map< QLocale::Country, QString >::const_iterator it = s_countryCodeMap.begin();
			it != s_countryCodeMap.end();
			++it )
		{
			if ( locList[1] == (*it).second )
			{
				m_localeCountry = (*it).first;
				break;
			}
		}
	}
	return true;
}






QString App::getInstallDir() const
{
	if ( m_installDir.isEmpty() )
	{
		char* appRootDir = getenv( "RTI_MDESIGNER_INSTALL_DIR" );

		if( appRootDir != NULL )
			m_installDir = appRootDir;
		else // Default to dir relative to executable.
			m_installDir = QCoreApplication::applicationDirPath() + "/../installData/";

		if ( ! m_installDir.endsWith( "/" ) && ! m_installDir.endsWith( "\\" ) )
			m_installDir += "/";
	}

  return m_installDir;
}


void App::saveSettings()
{
	createSettingsFile(true);
}



QString App::getSettingsFilePath() const
{
	QString		home, subDir, fullPath;
	QFileInfo	fileInfo;

	getSettingsDirectory( home, subDir );
	fullPath = home;
	fullPath += "/";
	fullPath += subDir;

	fullPath += "/prefs.xml";
	fileInfo.setFile( fullPath );

	return fileInfo.absoluteFilePath();
}

QString App::getFileDialogStartDirectory() const
{
	if ( m_fileDialogStart == AppRelative_FileDialogStart )
		return QCoreApplication::applicationDirPath() + "/" + getFileDialogDirectory();
	return getFileDialogDirectory();
}

void App::setDisplayLocale( QLocale::Language lc, QLocale::Country cc )
{
	m_localeLanguage = lc;
	m_localeCountry = cc;
}

QString App::getLocaleLanguageCode( QLocale::Language lang )
{
	return s_languageCodeMap[ lang ];
}



QString App::getLocaleCountryCode( QLocale::Country country )
{
	return s_countryCodeMap[ country ];
}


const QString& App::getJavaScriptPreamble()
{
	return m_JavaScriptPreamble;
}


const QString& App::getClassDocTemplate()
{
	return m_classDocTemplate;
}


const QString& App::getFunctionDocTemplate()
{
	return m_functionDocTemplate;
}

const QString& App::getMemberVariableDocTemplate()
{
	return m_memberVariableDocTemplate;
}

const QString& App::getParameterDocTemplate()
{
	return m_parameterDocTemplate;
}


bool App::evaluateJavaScript( const QString& script, std::string& resultString )
{
  bool evalSucceeded = m_jsInterp->Eval(m_jsContext, script.toStdString(),
		                                   resultString);
	return evalSucceeded;
}


QStringList App::getClassNames() const
{
	QStringList	classList;

	for ( APIParser::ClassAPIMap::const_iterator it = m_classAPIMap.begin(); it != m_classAPIMap.end(); ++it )
	{
		classList.push_back( (*it).first );
	}

	return classList;
}


bool App::isValidClassName( const QString& name ) const
{
	return ( m_classAPIMap.find(name) != m_classAPIMap.end() );
}

const MD::ClassAPI* App::getGlobalAPI() const
{
	return getClassAPI( "GlobalObject" );
}

const MD::ClassAPI* App::getClassAPI( const QString& className ) const
{
	APIParser::ClassAPIMap::const_iterator	it = m_classAPIMap.find( className );

	if ( it != m_classAPIMap.end() )
	{
		return (*it).second;
	}

	// Special case for Array[typename].
	if ( className.startsWith("Array[") )
		return getClassAPI( "Array" );

	return 0;
}


bool App::createSettingsFile( bool overwriteExisting )
{
	QString		home, subDir, fullPath;
	QDir			dir;
	QFileInfo	fileInfo;

	getSettingsDirectory( home, subDir );
	fullPath = home;
	fullPath += "/";
	fullPath += subDir;

	dir.setPath( fullPath );
	if ( ! dir.exists() )
	{
		dir.setPath( home );
		dir.mkpath( subDir );
		dir.setPath( fullPath );
		if ( ! dir.exists() )
			return false;
	}
	fullPath += "/prefs.xml";
	fileInfo.setFile( fullPath );
	if ( overwriteExisting || !fileInfo.exists() )
	{
		const int Indent = 4;
		QDomDocument	doc;
		QDomElement		root = doc.createElement(gSettingsRootTag);
		QFile					f(fullPath);
		QDomAttr			attr = doc.createAttribute("version");
		QDomElement		parent, child;
		QDomText			textNode;

		attr.setValue( "1.0" );
		root.setAttributeNode( attr );

		doc.appendChild(root);
		
		QDomElement	appElement = doc.createElement( "App" );
		root.appendChild( appElement );

		child = doc.createElement( "Locale" );
		QString locVal = s_languageCodeMap[m_localeLanguage] + "_" + s_countryCodeMap[m_localeCountry];
		textNode = doc.createTextNode( locVal );
		child.appendChild( textNode );
		appElement.appendChild( child );

		if ( m_useExperimentFile )
		{
			child = doc.createElement( "ExperimentFile" );
			textNode = doc.createTextNode( m_experimentFilename );
			child.appendChild( textNode );
			appElement.appendChild( child );
		}

		writeEditorPrefsNode( doc, appElement );

		f.open(QIODevice::WriteOnly);
		QTextStream	out( &f );
		doc.save(out, Indent);
	}

	fileInfo.setFile( fullPath );
	return fileInfo.exists();
}



void App::writeEditorPrefsNode( QDomDocument& doc, QDomElement& parent )
{
	QDomElement		root = doc.createElement("EditorPrefs");
	QDomElement		element;
	QDomAttr			attr;

	parent.appendChild(root);

	// Font & colors
	element = doc.createElement( "Font" );
	root.appendChild( element );

	attr = doc.createAttribute( "family" );
	attr.setValue(m_font.family());
	element.setAttributeNode(attr);

	attr = doc.createAttribute( "pointSize" );
	attr.setValue(QString("%1").arg(m_font.pointSize()));
	element.setAttributeNode(attr);

	for ( int i = 0; i < SyntaxItemCOUNT; ++i )
	{
		element = doc.createElement( gSyntaxItemString[i] );
		root.appendChild( element );

		attr = doc.createAttribute( "fgRed" );
		attr.setValue(QString("%1").arg(m_itemForegroundColor[i].red()));
		element.setAttributeNode(attr);

		attr = doc.createAttribute( "fgGreen" );
		attr.setValue(QString("%1").arg(m_itemForegroundColor[i].green()));
		element.setAttributeNode(attr);

		attr = doc.createAttribute( "fgBlue" );
		attr.setValue(QString("%1").arg(m_itemForegroundColor[i].blue()));
		element.setAttributeNode(attr);

		attr = doc.createAttribute( "bgRed" );
		attr.setValue(QString("%1").arg(m_itemBackgroundColor[i].red()));
		element.setAttributeNode(attr);

		attr = doc.createAttribute( "bgGreen" );
		attr.setValue(QString("%1").arg(m_itemBackgroundColor[i].green()));
		element.setAttributeNode(attr);

		attr = doc.createAttribute( "bgBlue" );
		attr.setValue(QString("%1").arg(m_itemBackgroundColor[i].blue()));
		element.setAttributeNode(attr);
	}

	// Brace matching
	element = doc.createElement( "BraceMatching" );
	root.appendChild( element );
	attr = doc.createAttribute( "style" );
	attr.setValue(gBraceMatchingStyleString[m_braceMatchingStyle]);
	element.setAttributeNode(attr);

	// Line length
	element = doc.createElement( "MaxLineLengthIndicator" );
	root.appendChild( element );

	attr = doc.createAttribute( "maxLength" );
	attr.setValue( QString("%1").arg(m_maxLineLength) );
	element.setAttributeNode(attr);

	attr = doc.createAttribute( "style" );
	attr.setValue( gMLLIndicatorStyleString[m_MLLIndicatorStyle]);
	element.setAttributeNode(attr);

	attr = doc.createAttribute( "r" );
	attr.setValue( QString("%1").arg(m_MLLIndicatorColor.red()) );
	element.setAttributeNode(attr);

	attr = doc.createAttribute( "g" );
	attr.setValue( QString("%1").arg(m_MLLIndicatorColor.green()) );
	element.setAttributeNode(attr);

	attr = doc.createAttribute( "b" );
	attr.setValue( QString("%1").arg(m_MLLIndicatorColor.blue()) );
	element.setAttributeNode(attr);

	// Line wrapping
	element = doc.createElement( "LineWrapping" );
	root.appendChild( element );

	attr = doc.createAttribute( "style" );
	attr.setValue(gLineWrapStyleString[m_lineWrapStyle]);
	element.setAttributeNode(attr);

	attr = doc.createAttribute( "indicator" );
	attr.setValue(gLineWrapIndicatorStyleString[m_lineWrapIndicatorStyle]);
	element.setAttributeNode(attr);

	attr = doc.createAttribute( "indent" );
	attr.setValue(QString("%1").arg(m_lineWrapIndent));
	element.setAttributeNode(attr);

	// Tabs & indentation
	element = doc.createElement( "Indentation" );
	root.appendChild( element );

	attr = doc.createAttribute( "autoindent" );
	attr.setValue(QString("%1").arg( (m_autoIndent?1:0) ));
	element.setAttributeNode(attr);

	attr = doc.createAttribute( "tabSize" );
	attr.setValue(QString("%1").arg(m_tabSize));
	element.setAttributeNode(attr);

	attr = doc.createAttribute( "indentSize" );
	attr.setValue(QString("%1").arg(m_indentSize));
	element.setAttributeNode(attr);

	attr = doc.createAttribute( "keepTabs" );
	attr.setValue(QString("%1").arg( (m_keepTabs?1:0) ));
	element.setAttributeNode(attr);

	// Whitespace
	element = doc.createElement( "Whitespace" );
	root.appendChild( element );

	attr = doc.createAttribute( "style" );
	attr.setValue( gWhitespaceStyleString[m_whitespaceStyle]);
	element.setAttributeNode(attr);

	attr = doc.createAttribute( "showEOL" );
	attr.setValue(QString("%1").arg( (m_showEOL?1:0) ));
	element.setAttributeNode(attr);

	/// Documentation
	element = doc.createElement( "Documentation" );
	root.appendChild( element );

	QDomElement	templateElement = doc.createElement( "Templates" );
	element.appendChild( templateElement );

	if ( ! m_JavaScriptPreamble.isEmpty() )
	{
		QDomElement	subElement = doc.createElement( "JavaScriptPreamble" );
		QDomText		text = doc.createTextNode(m_JavaScriptPreamble);

		templateElement.appendChild( subElement );
		subElement.appendChild(text);
	}

	if ( ! m_classDocTemplate.isEmpty() )
	{
		QDomElement	subElement = doc.createElement( "ClassDocTemplate" );
		QDomText		text = doc.createTextNode(m_classDocTemplate);

		templateElement.appendChild( subElement );
		subElement.appendChild(text);
	}

	if ( ! m_functionDocTemplate.isEmpty() )
	{
		QDomElement	subElement = doc.createElement( "FunctionDocTemplate" );
		QDomText		text = doc.createTextNode(m_functionDocTemplate);

		templateElement.appendChild( subElement );
		subElement.appendChild(text);
	}

	if ( ! m_memberVariableDocTemplate.isEmpty() )
	{
		QDomElement	subElement = doc.createElement( "MemberVariableDocTemplate" );
		QDomText		text = doc.createTextNode(m_memberVariableDocTemplate);

		templateElement.appendChild( subElement );
		subElement.appendChild(text);
	}

	if ( ! m_parameterDocTemplate.isEmpty() )
	{
		QDomElement	subElement = doc.createElement( "ParameterDocTemplate" );
		QDomText		text = doc.createTextNode(m_parameterDocTemplate);

		templateElement.appendChild( subElement );
		subElement.appendChild(text);
	}

	QDomElement tagElement = doc.createElement( "DoxygenTags" );
	element.appendChild(tagElement);

	if ( ! m_classNameDocTag.isEmpty() )
	{
		QDomElement	subElement = doc.createElement( "ClassNameTag" );
		QDomText		text = doc.createTextNode(m_classNameDocTag);

		tagElement.appendChild( subElement );
		subElement.appendChild(text);
	}

	if ( ! m_summaryDocTag.isEmpty() )
	{
		QDomElement	subElement = doc.createElement( "SummaryTag" );
		QDomText		text = doc.createTextNode(m_summaryDocTag);

		tagElement.appendChild( subElement );
		subElement.appendChild(text);
	}

	if ( ! m_descriptionDocTag.isEmpty() )
	{
		QDomElement	subElement = doc.createElement( "DescriptionTag" );
		QDomText		text = doc.createTextNode(m_descriptionDocTag);

		tagElement.appendChild( subElement );
		subElement.appendChild(text);
	}

	if ( ! m_functionOwnerDocTag.isEmpty() )
	{
		QDomElement	subElement = doc.createElement( "FunctionOwnerTag" );
		QDomText		text = doc.createTextNode(m_functionOwnerDocTag);

		tagElement.appendChild( subElement );
		subElement.appendChild(text);
	}

	if ( ! m_typeDocTag.isEmpty() )
	{
		QDomElement	subElement = doc.createElement( "TypeTag" );
		QDomText		text = doc.createTextNode(m_typeDocTag);

		tagElement.appendChild( subElement );
		subElement.appendChild(text);
	}

	/// Miscellaneous
	element = doc.createElement( "Miscellaneous" );
	root.appendChild( element );

	attr = doc.createAttribute( "showLineNumbers" );
	attr.setValue(QString("%1").arg( (m_showLineNumbers?1:0) ));
	element.setAttributeNode(attr);

	QDomElement	subElement = doc.createElement( "FileDialogDirectory" );
	QDomText		text = doc.createTextNode(m_fileDialogDirectory);

	element.appendChild( subElement );
	subElement.appendChild(text);

	attr = doc.createAttribute("type");
	attr.setValue(QString::number(m_fileDialogStart));
	subElement.setAttributeNode(attr);
}



void App::parseEditorPrefsNode( const QDomElement& root )
{
	QDomNode	node = root.firstChild();
	while ( ! node.isNull() )
	{
		const QDomElement& element = node.toElement();

		if ( element.tagName() == "Font" )
		{
			QString	family( XMLHelp::getAttribute(element, "family", false, false ) );
			int			pointSize( XMLHelp::getIntAttribute(element, "pointSize", false, false));

			m_font = QFont( family, pointSize);
		}
		else if ( element.tagName() == "BraceMatching" )
		{
			QString	attr( XMLHelp::getAttribute( element, "style", false, false ));

			for ( int i = 0; i < BraceMatchingStyleCOUNT; ++i )
			{
				if ( gBraceMatchingStyleString[i] == attr )
				{
					m_braceMatchingStyle = (BraceMatchingStyle) i;
					break;
				}
			}
		}
		else if ( element.tagName() == "MaxLineLengthIndicator" )
		{
			QString	attr( XMLHelp::getAttribute( element, "maxLength", false, false ) );
			m_maxLineLength = attr.toInt();
			if ( m_maxLineLength < 1 || m_maxLineLength > 200 )
				m_maxLineLength = 80;

			attr = XMLHelp::getAttribute( element, "style", false, false );
			for ( int i = 0; i < MLLIndicatorStyleCOUNT; ++i )
			{
				if ( gMLLIndicatorStyleString[i] == attr )
				{
					m_MLLIndicatorStyle = (MLLIndicatorStyle) i;
					break;
				}
			}

			int	r, g, b;
			attr = XMLHelp::getAttribute( element, "r", false, false );
			r = attr.toInt();
			attr = XMLHelp::getAttribute( element, "g", false, false );
			g = attr.toInt();
			attr = XMLHelp::getAttribute( element, "b", false, false );
			b = attr.toInt();

			if ( r < 0 || r > 255 )
				r = 0;
			if ( g < 0 || g > 255 )
				g = 0;
			if ( b < 0 || b > 255 )
				b = 0;

			m_MLLIndicatorColor.setRgb(r,g,b);
		}
		else if ( element.tagName() == "LineWrapping" )
		{
			QString	attr( XMLHelp::getAttribute( element, "style", false, false ));

			for ( int i = 0; i < LineWrapStyleCOUNT; ++i )
			{
				if ( gLineWrapStyleString[i] == attr )
				{
					m_lineWrapStyle = (LineWrapStyle) i;
					break;
				}
			}

			attr = XMLHelp::getAttribute( element, "indicator", false, false );

			for ( int i = 0; i < LineWrapIndicatorStyleCOUNT; ++i )
			{
				if ( gLineWrapIndicatorStyleString[i] == attr )
				{
					m_lineWrapIndicatorStyle = (LineWrapIndicatorStyle) i;
					break;
				}
			}
			m_lineWrapIndent = XMLHelp::getIntAttribute( element, "indent", false, false);
		}
		else if ( element.tagName() == "Indentation" )
		{
			m_autoIndent = (XMLHelp::getIntAttribute( element, "autoindent", false, false ) != 0);
			m_tabSize = XMLHelp::getIntAttribute(element, "tabSize", false, false);
			m_indentSize = XMLHelp::getIntAttribute(element, "indentSize", false, false);
			m_keepTabs = (XMLHelp::getIntAttribute( element, "keepTabs", false, false ) != 0);
		}
		else if ( element.tagName() == "Whitespace" )
		{
			QString	attr( XMLHelp::getAttribute(element, "style", false, false));

			for ( int i = 0; i < WhitespaceStyleCOUNT; ++i )
			{
				if ( gWhitespaceStyleString[i] == attr )
				{
					m_whitespaceStyle = (WhitespaceStyle) i;
					break;
				}
			}
			m_showEOL = (XMLHelp::getIntAttribute( element, "showEOL" ) != 0);
		}
		else if ( element.tagName() == "Documentation" )
		{
			QDomNode	subNode = element.firstChild();
			while ( ! subNode.isNull() )
			{
				const QDomElement& subElement = subNode.toElement();
				if ( subElement.tagName() == "Templates" )
				{
					QDomNode	tempNode = subElement.firstChild();
					while ( ! tempNode.isNull() )
					{
						const QDomElement& tempElement = tempNode.toElement();
						if ( tempElement.tagName() == "JavaScriptPreamble" )
							m_JavaScriptPreamble = tempElement.text();
						else if ( tempElement.tagName() == "ClassDocTemplate" )
							m_classDocTemplate = tempElement.text();
						else if ( tempElement.tagName() == "FunctionDocTemplate" )
							m_functionDocTemplate = tempElement.text();
						else if ( tempElement.tagName() == "MemberVariableDocTemplate" )
							m_memberVariableDocTemplate = tempElement.text();
						else if ( tempElement.tagName() == "ParameterDocTemplate" )
							m_parameterDocTemplate = tempElement.text();

						tempNode = tempNode.nextSibling();
					}
				}
				else if ( subElement.tagName() == "DoxygenTags" )
				{
					QDomNode	doxNode = subElement.firstChild();
					while ( ! doxNode.isNull() )
					{
						const QDomElement& doxElement = doxNode.toElement();
						if ( doxElement.tagName() == "ClassNameTag" )
							m_classNameDocTag = doxElement.text();
						else if ( doxElement.tagName() == "SummaryTag" )
							m_summaryDocTag = doxElement.text();
						else if ( doxElement.tagName() == "DescriptionTag" )
							m_descriptionDocTag = doxElement.text();
						else if ( doxElement.tagName() == "FunctionOwnerTag" )
							m_functionOwnerDocTag = doxElement.text();
						else if ( doxElement.tagName() == "TypeTag" )
							m_typeDocTag = doxElement.text();

						doxNode = doxNode.nextSibling();
					}
				}
				subNode = subNode.nextSibling();
			}
		}
		else if ( element.tagName() == "Miscellaneous" )
		{
			m_showLineNumbers = (XMLHelp::getIntAttribute( element, "showLineNumbers", false, false ) != 0);

			QDomNode	subNode = element.firstChild();
			while ( ! subNode.isNull() )
			{
				const QDomElement& subElement = subNode.toElement();
				if ( subElement.tagName() == "FileDialogDirectory" )
				{
					m_fileDialogStart = (FileDialogStart) XMLHelp::getIntAttribute( subElement, "type", false, false );
					m_fileDialogDirectory = subElement.text();
				}
				subNode = subNode.nextSibling();
			}
		}
		else
		{
			for ( int i = 0; i < SyntaxItemCOUNT; ++i )
			{
				if ( element.tagName() == gSyntaxItemString[i] )
				{
					QString	attr;
					int	r, g, b;

					attr = XMLHelp::getAttribute( element, "fgRed", false, false );
					r = attr.toInt();
					attr = XMLHelp::getAttribute( element, "fgGreen", false, false );
					g = attr.toInt();
					attr = XMLHelp::getAttribute( element, "fgBlue", false, false );
					b = attr.toInt();

					if ( r < 0 || r > 255 )
						r = 0;
					if ( g < 0 || g > 255 )
						g = 0;
					if ( b < 0 || b > 255 )
						b = 0;

					m_itemForegroundColor[i].setRgb(r,g,b);

					attr = XMLHelp::getAttribute( element, "bgRed", false, false );
					r = attr.toInt();
					attr = XMLHelp::getAttribute( element, "bgGreen", false, false );
					g = attr.toInt();
					attr = XMLHelp::getAttribute( element, "bgBlue", false, false );
					b = attr.toInt();

					if ( r < 0 || r > 255 )
						r = 0;
					if ( g < 0 || g > 255 )
						g = 0;
					if ( b < 0 || b > 255 )
						b = 0;

					m_itemBackgroundColor[i].setRgb(r,g,b);

					break;
				}
			}
		}
		node = node.nextSibling();
	}
}


void	App::getSettingsDirectory(QString& home, QString& subPath) const
{
	QDir			dir;

#if defined(Q_OS_WIN)
  TCHAR szPath[MAX_PATH];

	// The prefs file should go in the user's application data folder.
  if ( SUCCEEDED( ::SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szPath ) ) )
		home = QString::fromStdWString(szPath);
	else	// Better than nothing
		home = QDir::homePath();
#elif defined(Q_OS_UNIX)
	home = QDir::homePath();
#else
#error Need to define settings file path for OS/Platform.
#endif
	subPath = COMPANY_DIR_NAME;
	subPath += "/";
	subPath += PRODUCT_DIR_NAME;
}

} // namespace MD


