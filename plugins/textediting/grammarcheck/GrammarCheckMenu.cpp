#include "GrammarCheckMenu.h"
#include "GrammarCheck.h"

#include <KDebug>
#include <KActionMenu>
#include <KMenu>
#include <KLocale>
#include <QSignalMapper>

GrammarCheckMenu::GrammarCheckMenu(const LinkGrammar &grammarChecker, GrammarCheck *grammarCheck)
  : QObject(grammarCheck),
	m_grammarCheck(grammarCheck),
	m_grammarChecker(grammarChecker),
	m_suggestionsMenuAction(0),
	m_ignoreSegmentAction(0),
	m_suggestionsMenu(0),
	m_currentGrammaticallyIncorrectStartPosition(-1)
{
	m_suggestionsMenuAction = new KActionMenu(i18n("Grammar"), this);
	m_suggestionsMenu = m_suggestionsMenuAction->menu();
	connect(m_suggestionsMenu, SIGNAL(aboutToShow()), this, SLOT(createSuggestionsMenu()));

	m_ignoreSegmentAction = new KAction(i18n("Ignore"), this);
	connect(m_ignoreSegmentAction, SIGNAL(triggered()), this, SLOT(ignoreSegment()));

	setEnabled(false);
	setVisible(false);
}

GrammarCheckMenu::~GrammarCheckMenu()
{

}

QPair<QString, KAction*> GrammarCheckMenu::menuAction()
{
	return QPair<QString, KAction*>("grammar_suggestions", m_suggestionsMenuAction);
}

void GrammarCheckMenu::createSuggestionsMenu()
{
	m_suggestionsMenu->clear();
	m_suggestionsMenu->addAction(m_ignoreSegmentAction);
}

void GrammarCheckMenu::ignoreSegment()
{
	if (m_currentGrammaticallyIncorrectSegment.isEmpty() || m_currentGrammaticallyIncorrectStartPosition < 0)
		return;

	//TODO: we have yet to add this functionality, so may be comment it for now
	//m_grammarChecker.addToSession(m_currentGrammaticallyIncorrectSegment);

	emit clearHighlightingForSegment(m_currentGrammaticallyIncorrectStartPosition, m_lengthGrammaticallyIncorrect);

	m_currentGrammaticallyIncorrectSegment.clear();
	m_currentGrammaticallyIncorrectStartPosition = -1;
}

void GrammarCheckMenu::setGrammaticallyIncorrect(const QString &segment, int position, int length)
{
	m_currentGrammaticallyIncorrectSegment = segment;
	m_lengthGrammaticallyIncorrect = length;
	m_currentGrammaticallyIncorrectStartPosition = position;
}

void GrammarCheckMenu::setEnabled(bool b)
{
	if (m_suggestionsMenuAction)
		m_suggestionsMenuAction->setEnabled(b);

	if (m_ignoreSegmentAction)
		m_ignoreSegmentAction->setEnabled(b);
}

void GrammarCheckMenu::setVisible(bool b)
{
	if (m_suggestionsMenuAction)
		m_suggestionsMenuAction->setVisible(b);

	if (m_ignoreSegmentAction)
		m_ignoreSegmentAction->setVisible(b);
}

void GrammarCheckMenu::setCurrentLanguage(const QString &language)
{
	m_grammarChecker.setLanguage(language);
}

#include <GrammarCheckMenu.moc>
