#include "LinkGrammar.h"

#include <QTextDocument>
#include <QString>
#include <QMap>
#include <KDebug>
#include <locale.h>

LinkGrammar::LinkGrammar()
{
    //add the default language to list of supported languages
    //addLanguageToSetOfSupportedLanguagesWithDictionaryPath("en");
    setlocale(LC_ALL, "");
    m_Opts = parse_options_create();
    m_Dict = dictionary_create_lang("en");
    m_languagePreference = "en";
	// default max parsing time = 1
	m_maxTimeToParseInNumberOfSeconds = 1;
	// default disjunct count = 2
	m_disjunctCount = 2;
}

LinkGrammar::LinkGrammar(Dictionary dict, Parse_Options opts)
{
	m_Opts = opts;
	m_Dict = dict;
	// default max parsing time = 1
	m_maxTimeToParseInNumberOfSeconds = 1;
	// default disjunct count = 2
	m_disjunctCount = 2;
}

LinkGrammar::~LinkGrammar()
{
	cleanUp();
	m_listOfAvailableLanguages.clear();
}

void LinkGrammar::cleanUpDictionary()
{
	if(m_Dict)
		dictionary_delete(m_Dict);
	m_Dict = 0;
}

void LinkGrammar::cleanUpParseOptions()
{
	if(m_Opts)
		parse_options_delete(m_Opts);
	m_Opts = 0;
}

void LinkGrammar::cleanUp()
{
	cleanUpParseOptions();
	cleanUpDictionary();
}

int LinkGrammar::getMaxTimeToParseInNumberOfSeconds()
{
	return m_maxTimeToParseInNumberOfSeconds;
}

void LinkGrammar::setMaxTimeToParseInNumberOfSeconds(int numberOfSeconds)
{
	m_maxTimeToParseInNumberOfSeconds = numberOfSeconds;
}

int LinkGrammar::getDisjunctCount()
{
	return m_disjunctCount;
}

void LinkGrammar::setDisjunctCount(int count)
{
	m_disjunctCount = count;
}

QString LinkGrammar::getLanguage() const
{
	return m_languagePreference;
}

void LinkGrammar::setLanguage(const QString &language)
{
	m_languagePreference = language;
}
/*
void LinkGrammarWrapper::addLanguageToSetOfSupportedLanguages(QString language, QString dictionaryPath = QString())
{
	m_listOfAvailableLanguages[languages] = dictionaryPath;
}
*/
bool LinkGrammar::isLanguageAvailable(QString language)
{
	return m_listOfAvailableLanguages.contains(language);
}

bool LinkGrammar::parseSentence(QString givenSentence)
{
    setlocale(LC_ALL, "");
    m_Opts = parse_options_create();
    m_Dict = dictionary_create_lang("en");

    kDebug(31000) << "in parse sentence function";
    kDebug(31000) << givenSentence;
    
	if(!m_Dict)
	{
		kDebug(31000) << "No dictionary";
		return true; //no grammar checking
	}
    
    QByteArray utfByteArray = givenSentence.toUtf8();
    char *textChar = utfByteArray.data();
	
    Sentence sent = sentence_create(textChar, m_Dict);
	if (!sent)
	{
		kDebug(31000) << "Failed to create sentence";
		return true;
	}

	kDebug(31000) << "sentence created";

	parse_options_set_max_parse_time(m_Opts, m_maxTimeToParseInNumberOfSeconds);
	parse_options_set_disjunct_cost(m_Opts, m_disjunctCount);
	
	parse_options_set_min_null_count(m_Opts, 0);
	parse_options_set_max_null_count(m_Opts, 0);
	parse_options_set_islands_ok(m_Opts, 0);
	parse_options_set_panic_mode(m_Opts, true);
	parse_options_reset_resources(m_Opts);
	
	sentence_split(sent, m_Opts);
	quint32 num_linkages = sentence_parse(sent, m_Opts);
	
	bool res =  (num_linkages >= 1);
	
	if(parse_options_timer_expired(m_Opts))
	{
		kDebug() << "Timer expired!";
		res = true;
	}

	/*****
	 * TODO: find out what actually went wrong by allowing null linkages.
	 *******/
	//sentence_delete(sent);
	return res;
}
