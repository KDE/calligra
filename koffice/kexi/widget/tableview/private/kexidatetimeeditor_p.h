// Private interface from qdatetimeedit.cpp
// Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.

// This is temporary workaround
// Will be implemented outside of Qt class
// TODO check for future Qt versions (3.4?)

#ifndef KEXIDATETIMEEDITOR_P_H
#define KEXIDATETIMEEDITOR_P_H

#include <qglobal.h>

#ifdef Q_WS_WIN
# define QDateTimeEditor_HACK 1
#endif

#if defined(QDateTimeEditor_HACK) && QT_VERSION >= 0x030301 && QT_VERSION < 0x030400

#include <private/qrichtext_p.h>
#include <qwidget.h>

class QTextParagraph;
class QTextCursor;
class QDateTimeEditBase;
class QNumberSection;
class QDateTimeEditorPrivate;

//! @internal
class QDateTimeEditor : public QWidget
{
	Q_OBJECT
public:
	uint sectionCount() const;
	int  focusSection() const;
	bool setFocusSection( int s );

private:
	QDateTimeEditBase* cw;
	QDateTimeEditorPrivate* d;
};


//! @internal
class QNumberSection
{
public:
	int selectionStart() const;
	int selectionEnd() const;
	Q_DUMMY_COMPARISON_OPERATOR( QNumberSection )
private:
	int selstart :12;
	int selend	 :12;
	int act	 :7;
	bool sep	 :1;
};

//! needed for focusSection() and setFocusSection()
//! @internal
class QDateTimeEditorPrivate
{
public:
	uint sectionCount() const;
	int focusSection() const;
	bool setFocusSection( int idx );

protected:
	void applyFocusSelection();

private:
	bool frm;
	QTextParagraph *parag;
	QTextCursor *cursor;
	QSize sz;
	int focusSec;
	QValueList< QNumberSection > sections;
	QString sep;
	int offset;
};

#if defined(KEXIDATETIMEEDITOR_P_IMPL) && defined(QDateTimeEditor_HACK)
int QNumberSection::selectionStart() const { return selstart; }
int QNumberSection::selectionEnd() const { return selend; }

uint QDateTimeEditorPrivate::sectionCount() const { return (uint)sections.count(); }

int QDateTimeEditorPrivate::focusSection() const { return focusSec; }

bool QDateTimeEditorPrivate::setFocusSection( int idx )
{
	if ( idx > (int)sections.count()-1 || idx < 0 )
		return FALSE;
	if ( idx != focusSec ) {
		focusSec = idx;
		applyFocusSelection();
		return TRUE;
	}
	return FALSE;
}

void QDateTimeEditorPrivate::applyFocusSelection()
{
	if ( focusSec > -1 ) {
		int selstart = sections[ focusSec ].selectionStart();
		int selend = sections[ focusSec ].selectionEnd();
		parag->setSelection( QTextDocument::Standard, selstart, selend );
		parag->format();
		if ( parag->at( selstart )->x < offset ||
			parag->at( selend )->x + parag->string()->width( selend ) > offset + sz.width() ) {
			offset = parag->at( selstart )->x;
		}
	}
}

int QDateTimeEditor::focusSection() const
{
	return d->focusSection();
}

bool QDateTimeEditor::setFocusSection( int sec )
{
	return d->setFocusSection( sec );
}

//added
uint QDateTimeEditor::sectionCount() const
{
	return d->sectionCount();
}

#include "kexidatetimeeditor_p.moc"

#endif //KEXIDATATIMEEDITOR_P_IMPL

#endif

#endif //KEXIDATETIMEEDITOR_P_H
