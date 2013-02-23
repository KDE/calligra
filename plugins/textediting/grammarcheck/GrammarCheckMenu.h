#ifndef GRAMMARCHECKMENU_H
#define GRAMMARCHECKMENU_H

#include <QObject>
#include <QPair>

#include "LinkGrammar.h"

class GrammarCheck;
class KActionMenu;
class KAction;
class KMenu;
class QSignalMapper;

class GrammarCheckMenu : public QObject
{
  Q_OBJECT
public:
	explicit GrammarCheckMenu(const LinkGrammar &grammarChecker, GrammarCheck *grammarCheck);
	~GrammarCheckMenu();
	QPair<QString, KAction*> menuAction();
	void setGrammaticallyIncorrect(const QString &segment, int position,int length);
	void setEnabled(bool b);
	void setVisible(bool b);
	void setCurrentLanguage(const QString &language);
signals:
	void clearHighlightingForSegment(int startPosition, int length);
private slots:
	void createSuggestionsMenu();
	void ignoreSegment();
private:
	GrammarCheck *m_grammarCheck;
	LinkGrammar m_grammarChecker;
	KActionMenu *m_suggestionsMenuAction;
	KAction *m_ignoreSegmentAction;
	KMenu *m_suggestionsMenu;
	int m_lengthGrammaticallyIncorrect;
	int m_currentGrammaticallyIncorrectStartPosition;
	QString m_currentGrammaticallyIncorrectSegment;
};
#endif // GRAMMARCHECKMENU_H
