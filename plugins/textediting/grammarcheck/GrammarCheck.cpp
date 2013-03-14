#include "GrammarCheck.h"
#include "BgGrammarCheck.h"
#include "GrammarCheckMenu.h"
#include <KoCharacterStyle.h>
#include <KoTextBlockData.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootAreaProvider.h>
#include <KLocale>
#include <KDebug>
#include <KAction>
#include <KToggleAction>
#include <KConfigGroup>

#include <QTextBlock>
#include <QThread>
#include <QTimer>
#include <QApplication>
#include <QTextCharFormat>
#include <QTextBoundaryFinder>

#define LOGIC_DEBUG 1

GrammarCheck::GrammarCheck()
: m_document(0)
, m_bgGrammarCheck(0)
, m_enableGrammarCheck(true)
, m_isChecking(false)
, m_grammarCheckMenu(0)
, m_activeSection(0, 0, 0)
, m_simpleEdit(false)
{
    /* setup actions for this plugin */
    KAction *configureAction = new KAction(i18n("Configure &Grammar Checking..."), this);
	connect(configureAction, SIGNAL(triggered()), this, SLOT(configureGrammarCheck()));
	addAction("tool_configure_grammarcheck", configureAction);
	KToggleAction *grammarCheck = new KToggleAction(i18n("Auto Grammar Check"), this);
	addAction("tool_auto_grammarcheck", grammarCheck);
	KConfigGroup grammarConfig = KGlobal::config()->group("Grammar");
	m_enableGrammarCheck = grammarConfig.readEntry("autoGrammarCheck", m_enableGrammarCheck);
	grammarCheck->setChecked(m_enableGrammarCheck);
	m_grammarChecker = LinkGrammar();
	m_bgGrammarCheck = new BgGrammarCheck(m_grammarChecker, this);
	m_grammarCheckMenu = new GrammarCheckMenu(m_grammarChecker, this);
	QPair<QString, KAction*> pair = m_grammarCheckMenu->menuAction();
	addAction(pair.first, pair.second);
	connect(m_bgGrammarCheck, SIGNAL(grammaticallyIncorrectSegment(const QString &, int, int, bool)),
			this, SLOT(highlightGrammaticallyIncorrect(const QString &, int, int, bool)));
	connect(m_bgGrammarCheck, SIGNAL(done()), this, SLOT(finishedRun()));
	connect(grammarCheck, SIGNAL(toggled(bool)), this, SLOT(setBackgroundGrammarChecking(bool)));
}

void GrammarCheck::finishedWord(QTextDocument *document, int cursorPosition)
{
	setDocument(document);
	if (!m_enableGrammarCheck)
		return;
	QTextBlock block = document->findBlock(cursorPosition);
	if (!block.isValid())
		return;
	KoTextBlockData blockData(block);
	blockData.setMarkupsLayoutValidity(KoTextBlockData::Grammar, false);
    checkSection(QTextDocument *document, block.position(), (block.position() + block.length() - 1));
}

void GrammarCheck::findSentencesInBlock(const QTextBlock &block, QVector<QPair<int,int> > &sentencesInCurrentBlock)
{
	//function divides block into a array of sentences
	sentencesInCurrentBlock.clear();
	QString textSegment = block.text();
	QTextBoundaryFinder sentenceFinder(QTextBoundaryFinder::Sentence, textSegment);
	sentenceFinder.toStart();
	QTextBoundaryFinder::BoundaryReasons boundary = sentenceFinder.boundaryReasons();
	int numSentences = 0, startPos = 0, endPos;
	if(LOGIC_DEBUG)
	{
		kDebug(31000) << "BlockTextForSplitting:" << textSegment;
	}
	while(sentenceFinder.toNextBoundary() > 0)
	{
		boundary = sentenceFinder.boundaryReasons();
		if((boundary & QTextBoundaryFinder::StartWord))
		{
			endPos = sentenceFinder.position() - 1;
			if(startPos < endPos)
			{
				if(LOGIC_DEBUG)
				{
					kDebug(31000) << "SentenceFound(Complete):" << textSegment.mid(startPos, (endPos - startPos + 1));
				}
				sentencesInCurrentBlock.append(qMakePair(startPos, endPos));
				numSentences++;
				startPos = endPos + 1;
			}
		}
	}
	//capture potentially incomplete last sentence
	endPos = textSegment.length() - 1;
	if(startPos < endPos)
	{
		if(LOGIC_DEBUG)
		{
			kDebug(31000) << "SentenceFound(Potentially-InComplete):" << textSegment.mid(startPos, (endPos - startPos + 1));
		}
        if(isSentenceComplete(block.document(), block.position() + startPos, block.position() + endPos))
        {
    	    sentencesInCurrentBlock.append(qMakePair(startPos, endPos));
		    numSentences++;
            if(LOGIC_DEBUG)
    	    {
			    kDebug(31000) << "Sentencedetected(Complete):" << textSegment.mid(startPos, (endPos - startPos + 1));
		    }
        }
	}
}

bool GrammarCheck::isSentenceComplete(QTextDocument *document, int startPosition, int endPosition)
{
	kDebug(31000) << startPosition << ", " << endPosition;
	//returns if this segment is a sentence or not syntactically
	QTextCursor cursor(document);
	cursor.setPosition(startPosition);
	cursor.setPosition(endPosition+1, QTextCursor::KeepAnchor);
	QString textSegment = cursor.selectedText();
	kDebug(31000) << textSegment;
	static QString pre_punct = "[^A-Z][0-9a-z\\(\\)]{2}";
	static QString post_punct = QRegExp::escape("\"[]()") + "\\x201C\\x201D\\x201E\\x201F";
	static QString end_of_sentence = pre_punct + "[\\.\\?\\!][" + post_punct + "]?$";
	//static QString end_of_para = ".$";
	
	//QRegExp boundryRX("("+end_of_sentence+")|("+end_of_para+")");
	QRegExp boundryRX(end_of_sentence);
	boundryRX.setPatternSyntax(QRegExp::RegExp2);
	if(textSegment.contains(boundryRX))
	{
		kDebug(31000) << "sentence is complete";
		return true;
	}
	kDebug(31000) << "sentence not complete";
	return false;
}

int GrammarCheck::numberOfWords(QTextDocument *document, int startPosition, int endPosition)
{
	//returns number of finished words in this segment
	if(startPosition >= endPosition || startPosition < 0 || endPosition > document->characterCount() - 1)
	{
		return 0;
	}
	QTextCursor cursor(document);
	cursor.setPosition(startPosition);
	cursor.setPosition(endPosition+1, QTextCursor::KeepAnchor);
	QString textSegment = cursor.selectedText();

	//should we trim starting and trailing whiteSpaces from this text??

	QTextBoundaryFinder wordFinder(QTextBoundaryFinder::Word, textSegment);
	QTextBoundaryFinder::BoundaryReasons boundary = wordFinder.boundaryReasons();
	wordFinder.toStart();
	bool inWord = ((boundary & QTextBoundaryFinder::StartWord) != 0);
	int numWords = 0;
	while(wordFinder.toNextBoundary() > 0)
	{
		boundary = wordFinder.boundaryReasons();
		if((boundary & QTextBoundaryFinder::EndWord) && inWord)
		{
			numWords++;
			inWord = false;
		}
		else if((boundary & QTextBoundaryFinder::StartWord))
		{
			inWord = true;
		}
	}
	return numWords;
}

void GrammarCheck::checkSentence(QTextDocument *document, int startPosition, int endPosition)
{
	if (startPosition >= endPosition)
	{
		return;
	}
	GrammarSentence gs(document, startPosition, endPosition);
	m_documentsQueue.enqueue(gs);
	runQueue();
	m_grammarCheckMenu->setVisible(true);
}

void GrammarCheck::finishedParagraph(QTextDocument *document, int cursorPosition)
{
	setDocument(document);
	Q_UNUSED(document);
	Q_UNUSED(cursorPosition);
}

void GrammarCheck::startingSimpleEdit(QTextDocument *document, int cursorPosition)
{
	m_simpleEdit = true;
	setDocument(document);
	Q_UNUSED(document);
	Q_UNUSED(cursorPosition);
}

void GrammarCheck::checkSection(QTextDocument *document, int startPosition, int endPosition)
{
	//***note: this function clears markups of the blocks involved
	if(startPosition >= endPosition || startPosition < 0 || endPosition > document -> characterCount())
	{
		return;
	}
	QTextBlock block = document->findBlock(startPosition);
	bool sectionFinished = false;
	while(block.isValid() && !sectionFinished)
	{
		QVector<QPair<int, int> > sentenceVector;
		bool isMarkUpClearedForCurrentBlock = false;
		findSentencesInBlock(block, sentenceVector);
		for(int i = 0; i < sentenceVector.size(); i++)
		{
			if(sentenceVector[i].first + block.position() > endPosition)
			{
				sectionFinished = true;
				break;
			}
			if(sentenceVector[i].second + block.position() < startPosition)
			{
				continue;
			}
			if(!isMarkUpClearedForCurrentBlock)
			{
				isMarkUpClearedForCurrentBlock = true;
				KoTextBlockData blockData(block);
				blockData.clearMarkups(KoTextBlockData::Grammar);
			}
			checkSentence(document, sentenceVector[i].first + block.position(), sentenceVector[i].second + block.position());
		}
		block.next();
	}
}

void GrammarCheck::setDocument(QTextDocument *document)
{
	if (m_document == document)
		return;
	if (m_document)
		disconnect (document, SIGNAL(contentsChange(int, int, int)), this, SLOT(documentChanged(int, int, int)));
	m_document = document;
	connect (document, SIGNAL(contentsChange(int, int, int)), this, SLOT(documentChanged(int, int, int)));
}


QStringList GrammarCheck::availableBackends() const
{
	return QStringList();
	//TODO: define this function properly
	//return m_grammarChecker.availableBackends();
}

QStringList GrammarCheck::availableLanguages() const
{
	return QStringList();
	//TODO: define this function properly
	//return m_grammarChecker.availableLanguages();
}

void GrammarCheck::setDefaultLanguage(const QString &language)
{
	//m_grammarChecker.setDefaultLanguage(language);
	m_grammarChecker.setLanguage(language);
	m_bgGrammarCheck->setDefaultLanguage(language);
	if (m_enableGrammarCheck && m_document)
	{
		checkSection(m_document, 0, m_document->characterCount() - 1);
	}
}

void GrammarCheck::setBackgroundGrammarChecking(bool on)
{
	if (m_enableGrammarCheck == on)
		return;
	KConfigGroup grammarConfig = KGlobal::config()->group("Grammar");
	m_enableGrammarCheck = on;
	grammarConfig.writeEntry("autoGrammarCheck", m_enableGrammarCheck);
	if (m_document) {
		if (!m_enableGrammarCheck) {
			for (QTextBlock block = m_document->begin(); block != m_document->end(); block = block.next()) {
				KoTextBlockData blockData(block);
				blockData.clearMarkups(KoTextBlockData::Grammar);
			}
			m_grammarCheckMenu->setEnabled(false);
			m_grammarCheckMenu->setVisible(false);
		} else {
			checkSection(m_document, 0, m_document->characterCount() - 1);
			m_grammarCheckMenu->setVisible(true);
		}
	}
}

QString GrammarCheck::defaultLanguage() const
{
	return m_linkGrammarWrapper.getLanguage();
	//return m_linkGrammarWrapper.defaultLanguage();
}

bool GrammarCheck::backgroundGrammarChecking()
{
	return m_enableGrammarCheck;
}

void GrammarCheck::highlightGrammaticallyIncorrect(const QString &segment, int startPosition, int endPosition, bool grammaticallyIncorrect)
{
	kDebug(31000) << "sent for highlighting" << startPosition << ", " << endPosition << ", " << segment;
	if (!grammaticallyIncorrect)
		return;
	QTextBlock block = m_activeSection.document->findBlock(startPosition);
	KoTextBlockData blockData(block);
	blockData.appendMarkup(KoTextBlockData::Grammar, startPosition - block.position(), startPosition - block.position() + segment.trimmed().length());

	//blockData.appendMarkup(KoTextBlockData::Grammar, startPosition - block.position(), endPosition - block.position());
	Q_UNUSED(segment);
  
}

void GrammarCheck::documentChanged(int from, int minus, int plus)
{
	kDebug(31000)<<"document changed" << from<<", "<<minus<<", "<<plus;
	QTextDocument *document = qobject_cast<QTextDocument*>(sender());
	if (document == 0)
		return;
	QTextBlock block = document->findBlock(from);
	if (!block.isValid())
		return;
	do {
		KoTextBlockData blockData(block);
		if (m_enableGrammarCheck) {
			blockData.setMarkupsLayoutValidity(KoTextBlockData::Grammar, false);
			if (m_simpleEdit) {
				// if it's a simple edit we will wait until finishedWord
				blockData.rebaseMarkups(KoTextBlockData::Grammar, from, plus - minus);
			} else {
				checkSection(document, block.position(), block.position() + block.length() - 1);
			}
		} else {
			blockData.clearMarkups(KoTextBlockData::Grammar);
		}
		block = block.next();
	} while(block.isValid() && block.position() <= from + plus);
	m_simpleEdit = false;
}

void GrammarCheck::runQueue()
{
	//***this function assumes that markups are already cleared for the involved blocks
	Q_ASSERT(QThread::currentThread() == QApplication::instance()->thread());
	if (m_isChecking)
		return;
	while (!m_documentsQueue.isEmpty()) {
		m_activeSection = m_documentsQueue.dequeue();
			continue;
		if (m_activeSection.document.isNull())
		QTextBlock block = m_activeSection.document->findBlock(m_activeSection.from);
		if (!block.isValid())
			continue;
		m_isChecking = true;
		m_bgGrammarCheck->startRun(m_activeSection.document, m_activeSection.from, m_activeSection.to);
		break;
	}
}

void GrammarCheck::configureGrammarCheck()
{
	//TODO: implement this function
}

void GrammarCheck::finishedRun()
{
	Q_ASSERT(QThread::currentThread() == QApplication::instance()->thread());
	m_isChecking = false;
	KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout*>(m_activeSection.document->documentLayout());
	lay->provider()->updateAll();
	QTimer::singleShot(0, this, SLOT(runQueue()));
}

void GrammarCheck::setCurrentCursorPosition(QTextDocument *document, int cursorPosition)
{
	setDocument(document);
	if (m_enableGrammarCheck) {
		//check if sentence at cursor is wrongly incorrect
		QTextBlock block = m_document->findBlock(cursorPosition);
		if (block.isValid()) {
			KoTextBlockData blockData(block);
			KoTextBlockData::MarkupRange range = blockData.findMarkup(KoTextBlockData::Grammar, cursorPosition - block.position());
			if (int length = range.lastChar - range.firstChar) {
				QString segment = block.text().mid(range.firstChar, length);
				m_grammarCheckMenu->setGrammaticallyIncorrect(segment, block.position() + range.firstChar, length);
				m_grammarCheckMenu->setCurrentLanguage(m_bgGrammarCheck->currentLanguage());
				m_grammarCheckMenu->setVisible(true);
				m_grammarCheckMenu->setEnabled(true);
				return;
			}
			m_grammarCheckMenu->setEnabled(false);
		} else {
			m_grammarCheckMenu->setEnabled(false);
		}
	}
}

#include <GrammarCheck.moc>
