#ifndef LINKGRAMMAR_H
#define LINKGRAMMAR_H

#include<QMap>
#include<QString>

extern "C"
{
#include <link-grammar/link-includes.h>
}
    
	class LinkGrammar
	{
	public:
		LinkGrammar();
		LinkGrammar(Dictionary dict, Parse_Options opts);
		~LinkGrammar();
		//bool parseSentence(const QString text) const;
		bool parseSentence(const QString text);
		void cleanUp();
		bool isLanguageAvailable(QString language);
		int getMaxTimeToParseInNumberOfSeconds() const;
		void setMaxTimeToParseInNumberOfSeconds(int numberOfSeconds);
		int getDisjunctCount() const;
		void setDisjunctCount(int count);
		QString getLanguage() const;
		void setLanguage(const QString &language);
	//	void addLanguageToSetOfSupportedLanguages(QString language, QString dictionaryPath = QString());
		
	private:
		void cleanUpDictionary();
		void cleanUpParseOptions();
		Dictionary m_Dict;
		Parse_Options m_Opts;
		int m_maxTimeToParseInNumberOfSeconds;
		int m_disjunctCount;
		QString m_languagePreference;
		QString m_currentDictionaryLanguage;
		QMap<QString, QString> m_listOfAvailableLanguages;
	};
	
#endif // LINKGRAMMARWRAPPER_H
