/* This file is part of the KDE project
   Copyright (C)  2001 Montel Laurent <lmontel@mandrakesoft.com>

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

#ifndef __kofontdia_h__
#define __kofontdia_h__

#include <qcheckbox.h>
#include <kfontdialog.h>

// The embeddable font chooser widget
class KoFontChooser : public QWidget
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param fontListCriteria should contain all the restrictions for font selection as OR-ed values
     *        @see KFontChooser::FontListCriteria for the individual values
     */
    KoFontChooser( QWidget * parent, const char* name = 0L, 
            bool _withSubSuperScript = true,
            uint fontListCriteria=0);
    virtual ~KoFontChooser() {}

    void setFont( const QFont &_font, bool _subscript, bool _superscript );
    void setColor( const QColor & col );
    void setBackGroundColor( const QColor & col );

    bool getSuperScript() const { return m_superScript->isChecked(); }
    bool getSubScript() const { return m_subScript->isChecked(); }
    QFont getNewFont() const { return m_newFont; }
    QColor color() const { return m_chooseFont->color(); }
    QColor backGroundColor() const { return m_backGroundColor;}
    int changedFlags() const { return m_changedFlags; }

protected slots:
    void slotSuperScriptClicked();
    void slotSubScriptClicked();
    void slotUnderlineClicked();
    void slotStrikeOutClicked();
    void slotFontChanged(const QFont &);
    void slotChangeColor();
    void slotChangeBackGroundColor();
private:
    KFontChooser *m_chooseFont;
    QCheckBox *m_underline;
    QCheckBox *m_superScript;
    QCheckBox *m_subScript;
    QCheckBox *m_strikeOut;
    QPushButton *m_colorButton;
    QPushButton *m_backGroundColorButton;
    QFont m_newFont;
    QColor m_backGroundColor;
    int m_changedFlags;
};

class KoFontDia : public KDialogBase
{
    Q_OBJECT
public:
    KoFontDia( QWidget* parent, const char* name, const QFont &_font,
               bool _subscript, bool _superscript, const QColor & color,
	       const QColor & backGroundColor,
               bool _withSubSuperScript=true );

    bool getSuperScript() const { return m_chooser->getSuperScript(); }
    bool getSubScript() const { return m_chooser->getSubScript(); }
    QFont getNewFont() const { return m_chooser->getNewFont(); }
    QColor color() const { return m_chooser->color(); }
    QColor backGroundColor() const {return m_chooser->backGroundColor();}
    int changedFlags() const { return m_chooser->changedFlags(); }

private:
    KoFontChooser * m_chooser;
};

#endif
