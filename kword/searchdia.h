/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2001, S.R.Haque <srhaque@iee.org>
   Copyright (C) 2001, David Faure <david@mandrakesoft.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef searchdia_h
#define searchdia_h

#include <koFind.h>
#include <koReplace.h>
#include <qcolor.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qrichtext_p.h>
using namespace Qt3;

class QPushButton;
class QGridLayout;
class QCheckBox;
class QComboBox;
class QSpinBox;
class KColorButton;
class KWCanvas;
class KWTextFrameSet;

//
// This class represents the KWord-specific search extension items, and also the
// corresponding replace items.
//
class KWSearchContext
{
public:

    // Options.

    typedef enum
    {
        Family = 1 * KoFindDialog::MinimumUserOption,
        Color = 2 * KoFindDialog::MinimumUserOption,
        Size = 4 * KoFindDialog::MinimumUserOption,
        Bold = 8 * KoFindDialog::MinimumUserOption,
        Italic = 16 * KoFindDialog::MinimumUserOption,
        Underline = 32 * KoFindDialog::MinimumUserOption,
        VertAlign = 64 * KoFindDialog::MinimumUserOption
    } Options;

    KWSearchContext();

    QString m_family;
    QColor m_color;
    int m_size;
    QTextFormat::VerticalAlignment m_vertAlign;

    QStringList m_strings; // history
    long m_optionsMask;
    long m_options;
};

//
// This class represents the GUI elements that correspond to KWSearchContext.
//
class KWSearchContextUI : public QObject
{
    Q_OBJECT
public:
    KWSearchContextUI( KWSearchContext *ctx, QWidget *parent );
    void setCtxOptions( long options );
    void setCtxHistory( const QStringList & history );

private slots:
    void slotShowOptions();

private:
    KWSearchContext *m_ctx;
    QWidget *m_parent;

    bool m_bOptionsShown;
    QPushButton *m_btnShowOptions;
    QGridLayout *m_grid;
    QCheckBox *m_checkFamily;
    QCheckBox *m_checkSize;
    QCheckBox *m_checkColor;
    QCheckBox *m_checkBold;
    QCheckBox *m_checkItalic;
    QCheckBox *m_checkUnderline;
    QCheckBox *m_checkVertAlign;
    QComboBox *m_familyItem;
    QSpinBox *m_sizeItem;
    KColorButton *m_colorItem;
    QCheckBox *m_boldItem;
    QCheckBox *m_italicItem;
    QCheckBox *m_underlineItem;
    QComboBox *m_vertAlignItem;
};

//
// This class is the KWord search dialog.
//
class KWSearchDia:
    public KoFindDialog
{
    Q_OBJECT

public:
    KWSearchDia( KWCanvas *parent, const char *name, KWSearchContext *find );

protected slots:
    void slotOk();

private:
    KWSearchContextUI *m_findUI;
};

//
// This class is the KWord replace dialog.
//
class KWReplaceDia:
    public KoReplaceDialog
{
    Q_OBJECT

public:

    KWReplaceDia( KWCanvas *parent, const char *name, KWSearchContext *find, KWSearchContext *replace );

protected slots:
    void slotOk();

private:

    KWSearchContextUI *m_findUI;
    KWSearchContextUI *m_replaceUI;
};

//
// This class implements the 'find' functionality ( the "search next, prompt" loop )
// and the 'replace' functionality. Same class, to allow centralizing the code that
// finds the framesets and paragraphs to look into.
//
class KWFindReplace : public QObject
{
    Q_OBJECT
public:
    KWFindReplace( KWCanvas * canvas, KWSearchDia * dialog );
    KWFindReplace( KWCanvas * canvas, KWReplaceDia * dialog );
    ~KWFindReplace();
    void proceed();

protected:
    bool findInFrameSet( KWTextFrameSet * fs, QTextParag * firstParag, int firstIndex,
                         QTextParag * lastParag, int lastIndex );
    bool process( const QString &_text, const QRect &expose);
    void selectMatch( int index, int length );

protected slots:
    void highlight( const QString &text, int matchingIndex, int matchingLength, const QRect &expose );
    void replace( const QString &text, int replacementIndex, int replacedLength, const QRect &expose );

private:
    // Only one of those two will be set
    KoFind * m_find;
    KoReplace * m_replace;

    // Only one of those two will be set
    KWSearchDia * m_findDlg;
    KWReplaceDia * m_replaceDlg;

    int m_options;
    KWCanvas *m_canvas;
    KWTextFrameSet *m_currentFrameSet;
    QTextParag *m_currentParag;
    int m_offset;
};

#endif
