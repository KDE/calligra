#include "BgGrammarCheck.h"
#include "GrammarCheck.h"

#include <KoCharacterStyle.h>
#include <QTextDocument>
#include <QCoreApplication>
#include <QTextBlock>
#include <KDebug>
#include <QTimer>

BgGrammarCheck::BgGrammarCheck(const LinkGrammar &grammarChecker, QObject *parent)
:QObject(parent)
{
    m_grammarChecker = grammarChecker;
    QString lang = m_grammarChecker.getLanguage();
    if (lang.isEmpty())
        lang = "en_US";
    setDefaultLanguage(lang);
}

BgGrammarCheck::BgGrammarCheck(QObject *parent)
:QObject(parent)
{
    m_grammarChecker = LinkGrammar();
}

BgGrammarCheck::~BgGrammarCheck()
{
    m_grammarChecker.cleanUp();
}

void BgGrammarCheck::setDefaultLanguage(const QString &language)
{
    m_defaultLanguage = language;
    int index = m_defaultLanguage.indexOf('_');
    if (index > 0) {
        m_defaultCountry = m_defaultLanguage.mid(index+1);
        m_defaultLanguage = m_defaultLanguage.left(index);
    }
}

void BgGrammarCheck::changeLanguage(const QString &lang)
{
    m_grammarChecker.setLanguage(lang);
}    

void BgGrammarCheck::startRun(QTextDocument *document, int startPosition, int endPosition)
{
    if(!m_document)
    {
        emit done();
        return;
    }
    m_document = document;
    if (m_currentLanguage != m_defaultLanguage || m_currentCountry != m_defaultCountry) {
        m_currentCountry = m_defaultCountry;
        m_currentLanguage = m_defaultLanguage;
        if (m_currentCountry.isEmpty()) {
            changeLanguage(m_currentLanguage);
        } else {
            changeLanguage(m_currentLanguage+'_'+m_currentCountry);
        }
    }
    if (startPosition <= endPosition) {
        kDebug(31000) << "Starting:" << startPosition << endPosition;
        //TODO: put the core logic here.. may be a helper function
        m_startPosition = startPosition;
        m_endPosition = endPosition;
        QTextCursor cursor(m_document);
        cursor.setPosition(m_startPosition);
        cursor.setPosition(m_endPosition, QTextCursor::KeepAnchor);
        m_currentSentence = cursor.selectedText();
        QTimer::singleShot(0, this, SLOT(checkCurrentSentence()));
    } 
    else {
        emit done();
    }
}

void BgGrammarCheck::checkCurrentSentence()
{
    kDebug(31000) << "sentence sent to link-grammar" << m_currentSentence;
    bool isCorrect = m_grammarChecker.parseSentence(m_currentSentence);
    kDebug(31000) << isCorrect;
    if(!isCorrect)
        emit grammaticallyIncorrectSegment(m_currentSentence, m_startPosition, m_endPosition, true);
    emit done();
}

QString BgGrammarCheck::currentLanguage()
{
    return m_currentLanguage;
}

//void BgGrammmarCheck::continueChecking()
//TODO: implement remaining functions

#include <BgGrammarCheck.moc>
