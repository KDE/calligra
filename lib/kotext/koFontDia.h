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
#include <qtabwidget.h>
#include <qrichtext_p.h>
class QComboBox;
// The embeddable font chooser widget
class KoFontChooser : public QTabWidget
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param fontListCriteria should contain all the restrictions for font selection as OR-ed values
     *        @see KFontChooser::FontListCriteria for the individual values
     */
    KoFontChooser( QWidget * parent, const char* name = 0L,
            bool _withSubSuperScript = true, uint fontListCriteria=0);
    virtual ~KoFontChooser() {}

    void setFont( const QFont &_font, bool _subscript, bool _superscript );
    void setColor( const QColor & col );
    void setBackGroundColor( const QColor & col );

    bool getSuperScript() const { return m_superScript->isChecked(); }
    bool getSubScript() const { return m_subScript->isChecked(); }


    QFont getNewFont() const { return m_newFont; }
    QColor color() const { return m_chooseFont->color(); }
    QColor backGroundColor() const { return m_backGroundColor;}
    QColor underlineColor() const { return m_underlineColor ; }

    void setUnderlineColor( const QColor & col );


    KoTextFormat::NbLine getNblineType();
    KoTextFormat::LineType getUnderlineType();
    KoTextFormat::LineType getStrikeOutType();

    void setNblineType(KoTextFormat::NbLine nb);
    void setUnderlineType(KoTextFormat::LineType _t);
    void setStrikeOutType(KoTextFormat::LineType _t);

    int changedFlags() const { return m_changedFlags; }
    void setupTab1(bool _withSubSuperScript, uint fontListCriteria );
    void setupTab2();
protected:
    KoTextFormat::LineType getTypeOfLine( int val);

protected slots:
    void slotSuperScriptClicked();
    void slotSubScriptClicked();
    void slotStrikeOutClicked();
    void slotFontChanged(const QFont &);
    void slotChangeColor();
    void slotChangeBackGroundColor();
    void slotUnderlineColor();
    void slotChangeUnderlineType( int );
    void slotChangeStrikeOutType( int );
private:
    KFontChooser *m_chooseFont;
    QCheckBox *m_superScript;
    QCheckBox *m_subScript;
    QCheckBox *m_strikeOut;

    QComboBox *m_underlining;
    QComboBox *m_underlineType;

    QComboBox *m_strikeOutType;
    QPushButton *m_underlineColorButton;

    QPushButton *m_colorButton;
    QPushButton *m_backGroundColorButton;


    QFont m_newFont;
    QColor m_backGroundColor;
    QColor m_underlineColor;

    int m_changedFlags;
};

class KoFontDia : public KDialogBase
{
    Q_OBJECT
public:
    KoFontDia( QWidget* parent, const char* name, const QFont &_font,
               bool _subscript, bool _superscript, const QColor & color,
	       const QColor & backGroundColor,
               const QColor & underlineColor,
               KoTextFormat::NbLine _nbLine,
               KoTextFormat::LineType _underlineType,
               KoTextFormat::LineType _strikeOutType,
               bool _withSubSuperScript=true );

    bool getSuperScript() const { return m_chooser->getSuperScript(); }
    bool getSubScript() const { return m_chooser->getSubScript(); }
    QFont getNewFont() const { return m_chooser->getNewFont(); }
    QColor color() const { return m_chooser->color(); }
    QColor backGroundColor() const {return m_chooser->backGroundColor();}
    QColor underlineColor() const { return m_chooser->underlineColor() ; }
    KoTextFormat::NbLine getNblineType() const { return m_chooser->getNblineType();}
    KoTextFormat::LineType getUnderlineType() const { return m_chooser->getUnderlineType();}
    KoTextFormat::LineType getStrikeOutType() const { return m_chooser->getStrikeOutType();}

    int changedFlags() const { return m_chooser->changedFlags(); }

protected slots:
    void slotReset();
    virtual void slotApply();
    virtual void slotOk();
signals:
     void apply();

private:
    KoFontChooser * m_chooser;
    QFont m_font;
    bool m_subscript;
    bool m_superscript;
    bool m_strikeOut;
    QColor m_color;
    QColor m_backGroundColor;
    QColor m_underlineColor;
    KoTextFormat::NbLine m_nbLine;
    KoTextFormat::LineType m_underlineType;
    KoTextFormat::LineType m_strikeOutType;
};

#endif
