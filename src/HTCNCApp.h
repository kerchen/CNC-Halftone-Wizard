//
// Written by Paul Kerchen
// Released under GNU GPL.  Enjoy.
//


#ifndef MDAPP_H
#define MDAPP_H

#include <QWidget>
#include <QLocale>
#include <QString>
#include <QColor>
#include <QFont>

#include "../inc/JavaScriptInterpreter.h"
#include "MDClassAPI.h"
#include "MDAPIParser.h"

#include <map>

// Forward declarations
class QLocale;
class QDomElement;
class QDomDocument;
class QsciLexerJavaScript;

namespace TEC
{
	class BaseExperiment;
}

namespace MD
{
/**@brief A singleton for holding application-wide information and functionality. */
class App : public QWidget
{
	Q_OBJECT

public:
	typedef enum
	{
		No_BraceMatchingStyle,
		Strict_BraceMatchingStyle,
		Sloppy_BraceMatchingStyle,
		BraceMatchingStyleCOUNT
	} BraceMatchingStyle;

	typedef enum
	{
		No_MLLIndicatorStyle,
		VerticalLine_MLLIndicatorStyle,
		BackgroundColor_MLLIndicatorStyle,
		MLLIndicatorStyleCOUNT
	} MLLIndicatorStyle;

	typedef enum
	{
		Default_SyntaxItem,
		Comment_SyntaxItem,
		Number_SyntaxItem,
		Keyword_SyntaxItem,
		DoubleQuotedString_SyntaxItem,
		SingleQuotedString_SyntaxItem,
		Operator_SyntaxItem,
		Identifier_SyntaxItem,
		UnclosedString_SyntaxItem,
		SyntaxItemCOUNT
	} SyntaxItem;

	typedef enum
	{
		No_LineWrapStyle,
		Word_LineWrapStyle,
		Character_LineWrapStyle,
		LineWrapStyleCOUNT
	} LineWrapStyle;

	typedef enum
	{
		IndentOnly_LineWrapIndicatorStyle,
		Text_LineWrapIndicatorStyle,
		Border_LineWrapIndicatorStyle,
		LineWrapIndicatorStyleCOUNT
	} LineWrapIndicatorStyle;

	typedef enum
	{
		Invisible_WhitespaceStyle,
		Visible_WhitespaceStyle,
		VisibleAfterIndent_WhitespaceStyle,
		WhitespaceStyleCOUNT
	} WhitespaceStyle;

	typedef enum
	{
		/// Dialog should start in last-used directory
		LastUsed_FileDialogStart,
		/// Dialog should start in directory relative to app binary.
		AppRelative_FileDialogStart,
		/// Dialog should always start from same absolute directory.
		Absolute_FileDialogStart,
		FileDialogStart_COUNT
	} FileDialogStart;

	/// Returns a reference to the application singleton.
	static App& Instance()
	{
		if ( ! s_instance )
		{
			s_instance = new App();
		}
		return *s_instance;
	}

	/// Yer typical dtor.
	~App();

	/**@brief Performs application initialization.
	 * Call it once at program start.
	 */
	bool init( QString& failureMsg );

	/**@brief Performs application cleanup and shutdown.
	 * The app singleton object is deleted.
	 */
	static void shutdown()
	{
		if ( s_instance )
		{
			delete s_instance;
			s_instance = NULL;
		}
	}

	/**@brief Returns the full path to the application installation directory.
	 * If the RTI_MDESIGNER_INSTALL_DIR environment variable is set, it is
	 * used instead of the default.
	 * @return The full path to the application installation directory, with 
	 * a trailing slash. */
	QString getInstallDir() const;

	/**@brief Returns the absolute path to the application settings file.
	 */
	QString getSettingsFilePath() const;

	/**@brief Saves the current application settings to the settings file. */
	void saveSettings();

	/**@brief Returns the application version string. */
	const QString& getVersionString() { return s_versionString; }

	/**@brief Returns the locale that should be used for numeric data displayed to user. 
	*/
	const QLocale& getDisplayLocale() const { return *m_displayLocale; }
	
	/**@brief Sets the language and country that should be used for future runs of the app. */
	void setDisplayLocale( QLocale::Language, QLocale::Country );

	/**@brief Returns the two-digit ISO 639 language code for the given language ID. */
	QString getLocaleLanguageCode( QLocale::Language );

	/**@brief Returns the two-digit ISO 3166 country code for the given country ID. */
	QString getLocaleCountryCode( QLocale::Country );

	/**@brief Indicates how the starting directory of file dialogs should be determined. */
	FileDialogStart getFileDialogStart() const { return m_fileDialogStart; }

	void setFileDialogStart( FileDialogStart start ) { m_fileDialogStart = start; }

	/**@brief Returns the current starting directory for file dialogs.
	 * The returned value will either be an absolute path to a directory name
	 * or a path to a directory name relative to the location of the application
	 * binary.  Use getFileDialogStart() to determine the meaning of
	 * the value returned by this function.
	 * @sa getFileDialogStart().
	 */
	const QString& getFileDialogDirectory() const { return m_fileDialogDirectory; }

	/**@brief Sets the current starting directory for file dialogs.
	 * This value should be consistent with the value returned by getFileDialogStart().
	 * @sa getFileDialogStart().
	 */
	void setFileDialogDirectory( const QString& startDir ) { m_fileDialogDirectory = startDir; }

	/**@brief Returns the absolute directory that should be used as the starting directory for file dialogs.
	 * Takes the values of getFileDialogStart() and getFileDialogDirectory()
	 * to determine the appropriate return value.
	 */
	QString getFileDialogStartDirectory() const;

	/// Triggers the app to emit a "prefs changed" signal.
	void emitPreferencesChangedSignal() { emit preferencesChanged(); }

	const QFont& getFont() { return m_font; }
	void setFont( const QFont& font ) { m_font = font; }

	const QColor& getItemForegroundColor( SyntaxItem item ) { return m_itemForegroundColor[ item ]; }
	void setItemForegroundColor( SyntaxItem item, const QColor& c ) { m_itemForegroundColor[ item ] = c; }

	const QColor& getItemBackgroundColor( SyntaxItem item ) { return m_itemBackgroundColor[item]; }
	void setItemBackgroundColor( SyntaxItem item, const QColor& c ) { m_itemBackgroundColor[item] = c; }

	/// Returns the user's preference for brace matching.
	BraceMatchingStyle getBraceMatchingStyle() { return m_braceMatchingStyle; }
	/// Sets the user's brace matching preference
	void setBraceMatchingStyle( BraceMatchingStyle s ) { m_braceMatchingStyle = s; }

	/// Returns the user's max line length indicator style preference.
	MLLIndicatorStyle getMLLIndicatorStyle() { return m_MLLIndicatorStyle; }
	/// Sets the user's max line length indicator style preference.
	void setMLLIndicatorStyle( MLLIndicatorStyle s ) { m_MLLIndicatorStyle = s; }

	/// Returns the user's max line length preference.
	int getMaxLineLength() { return m_maxLineLength; }
	/// Sets the user's max line length preference.
	void setMaxLineLength( int ll ) { m_maxLineLength = ll; }

	/// Returns the user's max line length indicator color preference.
	const QColor& getMLLIndicatorColor() { return m_MLLIndicatorColor; }
	/// Sets the user's max line length indicator color preference.
	void setMLLIndicatorColor( const QColor& c ) { m_MLLIndicatorColor = c; }

	/// Returns the user's line wrapping style preference.
	LineWrapStyle getLineWrapStyle() { return m_lineWrapStyle; }
	/// Sets the user's line wrapping style preference.
	void setLineWrapStyle( LineWrapStyle s ) { m_lineWrapStyle = s; }

	/// Returns the user's line wrap indicator style preference.
	LineWrapIndicatorStyle getLineWrapIndicatorStyle() { return m_lineWrapIndicatorStyle; }
	/// Sets the user's line wrap indicator style preference.
	void setLineWrapIndicatorStyle( LineWrapIndicatorStyle s ) { m_lineWrapIndicatorStyle = s; }

	/// Returns the user's line wrap indent preference.
	int getLineWrapIndent() { return m_lineWrapIndent; }
	/// Sets the user's line wrap indent preference.
	void setLineWrapIndent( int amt ) { m_lineWrapIndent = amt; }

	/// Returns the user's auto-indent preference.
	bool getAutoIndent() { return m_autoIndent; }
	/// Sets the user's auto-indent preference.
	void setAutoIndent( bool ai ) { m_autoIndent = ai; }

	/// Returns the user's tab size preference.
	int getTabSize() { return m_tabSize; }
	/// Sets the user's tab size preference.
	void setTabSize( int ts ) { m_tabSize = ts; }

	/// Returns the user's indent size preference.
	int getIndentSize() { return m_indentSize; }
	/// Sets the user's indent size preference.
	void setIndentSize( int is ) { m_indentSize = is; }

	/// Returns the user's preference of whether or not tab characters should be preserved.
	bool getKeepTabs() { return m_keepTabs; }
	/// Sets the user's preference of whether or not tab characters should be preserved.
	void setKeepTabs( bool kt ) { m_keepTabs = kt; }

	/// Returns the user's whitespace style preference.
	WhitespaceStyle getWhitespaceStyle() { return m_whitespaceStyle; }
	/// Sets the user's whitespace style preference.
	void setWhitespaceStyle( WhitespaceStyle s ) { m_whitespaceStyle = s; }

	/// Returns the user's preference regarding whether or not to show the end-of-line.
	bool getShowEOL() { return m_showEOL; }
	/// Sets the user's preference regarding whether or not to show the end-of-line.
	void setShowEOL( bool show ) { m_showEOL = show; }

	/// Returns the user's preference regarding whether or not line numbers should be shown in the editor's margin.
	bool getShowLineNumbers() { return m_showLineNumbers; }
	/// Sets the user's preference regarding whether or not line numbers should be shown in the editor's margin.
	void setShowLineNumbers( bool show ) { m_showLineNumbers = show; }

	/**@brief Sets the text that should appear at the top of every JavaScript file written by the editor. */
	void setJavaScriptPreamble( const QString& text ) { m_JavaScriptPreamble = text; }

	/**@brief Returns the text that should appear at the top of every JavaScript file written by the editor. */
	const QString& getJavaScriptPreamble();

	/**@brief Sets the text that defines how class documentation should be written by the editor. */
	void setClassDocTemplate( const QString& text ) { m_classDocTemplate = text; }
	/**@brief Returns the text that defines how class documentation should be written by the editor. */
	const QString& getClassDocTemplate();

	/**@brief Sets the text that defines how function documentation should be written by the editor. */
	void setFunctionDocTemplate( const QString& text ) { m_functionDocTemplate = text; }
	/**@brief Returns the text that defines how function documentation should be written by the editor. */
	const QString& getFunctionDocTemplate();

	/**@brief Sets the text that defines how member variable documentation should be written by the editor. */
	void setMemberVariableDocTemplate( const QString& text ) { m_memberVariableDocTemplate = text; }
	/**@brief Returns the text that defines how member variable documentation should be written by the editor. */
	const QString& getMemberVariableDocTemplate();

	/**@brief Sets the text that defines how GUI parameter documentation should be written by the editor. */
	void setParameterDocTemplate( const QString& text ) { m_parameterDocTemplate = text; }
	/**@brief Returns the text that defines how GUI parameter documentation should be written by the editor. */
	const QString& getParameterDocTemplate();

	/// Returns the doxygen tag that tags class name fields.
	const QString& getClassNameDocTag() { return m_classNameDocTag; }
	void setClassNameDocTag( const QString& tag ) { m_classNameDocTag = tag; }

	/// Returns the doxygen tag that tags summary fields.
	const QString& getSummaryDocTag() { return m_summaryDocTag; }
	void setSummaryDocTag( const QString& tag ) { m_summaryDocTag = tag; }

	/// Returns the doxygen tag that tags description fields.
	const QString& getDescriptionDocTag() { return m_descriptionDocTag; }
	void setDescriptionDocTag( const QString& tag ) { m_descriptionDocTag = tag; }

	/// Returns the doxygen tag that tags member function class name fields.
	const QString& getFunctionOwnerDocTag() { return m_functionOwnerDocTag; }
	void setFunctionOwnerDocTag( const QString& tag ) { m_functionOwnerDocTag = tag; }

	/// Returns the doxygen tag that tags type fields.
	const QString& getTypeDocTag() { return m_typeDocTag; }
	void setTypeDocTag( const QString& tag) { m_typeDocTag = tag; }

	/**@brief Evaluates JavaScript code. 
	 * @param script The JavaScript code to be evaluated.
	 * @param result The string that receives the result returned by the interpreter.
	 * @return Returns true if the code did not have any errors.
	 */
	bool evaluateJavaScript( const QString& script, std::string& result );

	/**@brief Returns a list of the names of the JavaScript classes known to the app. */
	QStringList getClassNames() const;

	/**@brief Returns true if the given name is the name of a JavaScript class known to the app. */
	bool isValidClassName( const QString& name ) const;

	/**@brief Returns the API associated with the global context. */
	const MD::ClassAPI* getGlobalAPI() const;

	/**@brief Returns the API associated with a particular JavaScript class. */
	const MD::ClassAPI* getClassAPI( const QString& className ) const;
	
	/**@brief Sets the active experiment using the given experiment filename.
	 * @param filename The absolute path to a TEC experiment file.
	 * @param errorMsg If the experiment could not be loaded, this parameter
	 * will hold diagnostic messages for the user.
	 * If an experiment cannot be created from the file specified by filename,
	 * the active experiment will remain unchanged.
	 */
	bool setActiveExperiment( const QString& filename, QString& errorMsg );

	/// Returns the current active experiment.
	TEC::BaseExperiment*	getActiveExperiment() const { return m_activeExperiment; }

signals:
	void preferencesChanged();

private slots:

private:
	/// Private constructor.
	App();
	/// Not implemented.
	App(const App&);
	/// Not implemented.
	void operator=(const App&);

	/**@brief Parses the App node of the settings file. */
	bool parseAppNode( const QDomElement&, QString& errorMsg );

	/**@brief Creates the application settings file.
	 * @param overwriteExisting If true, existing settings file will be overwritten with 
	 *	current settings.
	 * @return Returns true if the file already exists or if it was successfully created.
	 */
	bool createSettingsFile( bool overwriteExisting );

	/// Writes editor-related preferences.
	void writeEditorPrefsNode( QDomDocument& doc, QDomElement& root );

	void parseEditorPrefsNode( const QDomElement& root );

	/**@brief Returns the absolute path to the settings directory, broken
	 * into two parts: the existing root and the application-created subdirectory.
	 * Usually, the root will be the user's home directory and subDir will
	 * be something like "Realtime Technologies/Maneuver Designer".
	 */
	void	getSettingsDirectory(QString& root, QString& subDir) const;

	/// The app singleton object.
	static MD::App*	s_instance;

	/// The application version string.
	const QString	s_versionString;

	mutable QString		m_installDir;

	/// The locale to use for formatting numeric information displayed to the user.
	QLocale*					m_displayLocale;
	/// The Qt language ID of the language that will be saved the next time the app settings are saved.
	QLocale::Language	m_localeLanguage;
	/// The Qt country ID of the country that will be saved the next time the app settings are saved.
	QLocale::Country	m_localeCountry;

	tec::JavaScriptInterpreter*	m_jsInterp;
	tec::detail::Context*				m_jsContext;

	/// The class API objects need a reference lexer that owns those objects; this is it.
	QsciLexerJavaScript*	m_referenceLexer;

	/// Holds the text that appears at the top of each JavaScript file written by the editor.
	QString			m_JavaScriptPreamble;
	/// Holds the template for the documentation block that appears above each maneuver class definition.
	QString			m_classDocTemplate;
	/// Holds the template for the documentation block that appears above each member function definition.
	QString			m_functionDocTemplate;
	/// Holds the template for the documentation block that appears above each member variable definition.
	QString			m_memberVariableDocTemplate;
	/// Holds the template for the documentation block that appears above each gui parameter definition.
	QString			m_parameterDocTemplate;

	/// The doxygen tag that tags class name fields.
	QString			m_classNameDocTag;
	/// The doxygen tag that tags summary fields.
	QString			m_summaryDocTag;
	/// The doxygen tag that tags description fields.
	QString			m_descriptionDocTag;
	/// The doxygen tag that tags member function class name fields.
	QString			m_functionOwnerDocTag;
	/// The doxygen tag that tags type fields.
	QString			m_typeDocTag;

	/// The font used in all JavaScript editors.
	QFont				m_font;

	/// The foreground color for various items in all JavaScript editors.
	QColor			m_itemForegroundColor[ SyntaxItemCOUNT ];
	/// The background color for various items in all JavaScript editors.
	QColor			m_itemBackgroundColor[ SyntaxItemCOUNT ];

	/// The brace matching style for all JavaScript editors.
	BraceMatchingStyle	m_braceMatchingStyle;

	/// The maximum line length indicator style for all JavaScript editors.
	MLLIndicatorStyle	m_MLLIndicatorStyle;
	/// The maximum line length for all JavaScript editors.
	int					m_maxLineLength;
	/// The indicator color for all JavaScript editors.
	QColor			m_MLLIndicatorColor;

	/// The line wrapping style used by all JavaScript editors.
	LineWrapStyle	m_lineWrapStyle;
	/// The line wrapping indicator style used by all JavaScript editors.
	LineWrapIndicatorStyle	m_lineWrapIndicatorStyle;
	/// The amount wrapped lines should be indented.
	int					m_lineWrapIndent;

	/// True if auto-indentation should be used in JavaScript editors.
	bool				m_autoIndent;
	/// The size, in spaces, of tabs.
	int					m_tabSize;
	/// The size, in spaces, of indentations.
	int					m_indentSize;
	/// If true, tabs should not be converted to m_tabSize spaces.
	bool				m_keepTabs;

	/// The style of whitespace display used by JavaScript editors.
	WhitespaceStyle	m_whitespaceStyle;
	/// True if JavaScript editors should show end-of-line characters.
	bool				m_showEOL;

	/// True if line numbers should be shown in the margins of text editors.
	bool				m_showLineNumbers;

	/// Indicates what the starting directory should be for file dialogs.
	FileDialogStart m_fileDialogStart;

	/// Holds the directory name that should be used as the starting point for file dialogs
	QString		m_fileDialogDirectory;

	/// Holds the APIs that are common to all maneuvers.
	APIParser::ClassAPIMap	m_classAPIMap;

	/// If true, the app should use m_experimentFilename for the app's active experiment.
	bool			m_useExperimentFile;

	/// The absolute path and filename to the active experiment.
	QString		m_experimentFilename;

	/// The experiment that should be used for populating database controls in the form preview.
	TEC::BaseExperiment*	m_activeExperiment;

	/// Maps Qt language codes to ISO 639 code
	std::map< QLocale::Language, QString >	s_languageCodeMap;
	/// Maps Qt country code to ISO 3166 code
	std::map< QLocale::Country, QString > s_countryCodeMap;
};

}	// namespace MD

#endif


