#ifndef GRAMMARCHECK_H
#define GRAMMARCHECK_H

#include <KoTextEditingPlugin.h>
#include "LinkGrammar.h"
#include <QTextCharFormat>
#include <QTextDocument>
#include <QPointer>
#include <QQueue>
#include <QTextLayout>
#include <QTextStream>

#include <GrammarCheckMenu.h>

class QTextDocument;
class QTextStream;
class BgGrammarCheck;

class GrammarCheck : public KoTextEditingPlugin
{
    Q_OBJECT
public:
    GrammarCheck();
	/// reimplemented from superclass
	void finishedWord(QTextDocument *document, int cursorPosition);
	/// reimplemented from superclass
	void finishedParagraph(QTextDocument *document, int cursorPosition);
	/// reimplemented from superclass
	void startingSimpleEdit(QTextDocument *document, int cursorPosition);
	/// reimplemented from superclass
	void checkSection(QTextDocument *document, int startPosition, int endPosition);
	///reimplemented from superclass
	void setCurrentCursorPosition(QTextDocument *document, int cursorPosition);
	void checkSentence(QTextDocument *document, int startPosition, int endPosition);
	void findSentencesInBlock(const QTextBlock &block, QVector<QPair<int,int> > &sentencesInCurrentBlock);
	bool isSentenceComplete(QString textSegment);
	int numberOfWords(QTextDocument *document, int startPosition, int endPosition);
	QStringList availableBackends() const;
	QStringList availableLanguages() const;
	QString defaultLanguage() const;
	bool backgroundGrammarChecking();
	void setDocument(QTextDocument *document);
public slots:
	void setDefaultLanguage(const QString &lang);
private slots:
	void highlightGrammaticallyIncorrect(const QString &segment, int startPosition, int endPosition, bool grammaticallyIncorrect = true);
	void finishedRun();
	void configureGrammarCheck();
	void runQueue();
	void setBackgroundGrammarChecking(bool b);
	void documentChanged(int from, int minus, int plus);
private:
	LinkGrammar m_linkGrammarWrapper;
	QPointer<QTextDocument> m_document;
	BgGrammarCheck *m_bgGrammarCheck;
	struct GrammarSentence {
		GrammarSentence(QTextDocument *doc, int start, int end)
		: document(doc)
		{
			from = start;
			to = end;
		}
		QPointer<QTextDocument> document;
		int from;
		int to;
	};
	QQueue<GrammarSentence> m_documentsQueue;
	bool m_enableGrammarCheck;
	bool m_isChecking;
	LinkGrammar m_grammarChecker;
	QTextStream stream;
	GrammarCheckMenu *m_grammarCheckMenu;
	GrammarSentence m_activeSection; // the section we are currently doing a run on;
	bool m_simpleEdit; //set when user is doing a simple edit, meaning we should ignore documentCanged
};
#endif
