/* This file is part of the KDE project
   Copyright (C)  2001,2002,2003 Montel Laurent <lmontel@mandrakesoft.com>

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

#include <kfontdialog.h>
#include <qtabwidget.h>
#include <kotextformat.h>
#include <qcheckbox.h>

/// OK including config.h in public headers is bad practice - to be removed once kspell2 is required
#include <config.h>
#ifdef HAVE_LIBKSPELL2
#include <kspell2/broker.h>
#endif
class QComboBox;

/**
 * The embeddable font chooser widget
 */
class KoFontChooser : public QTabWidget
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param fontListCriteria should contain all the restrictions for font selection as OR-ed values
     *        @see KFontChooser::FontListCriteria for the individual values
     * @param broker If your application supports spell-checking, pass here the KSpell2 Broker
     * so that the font dialog can show which languages are supported for spellchecking.
     */
    KoFontChooser( QWidget * parent, const char* name = 0,
                   bool _withSubSuperScript = true, uint fontListCriteria=0
#ifdef HAVE_LIBKSPELL2
                   , KSpell2::Broker::Ptr broker = 0
#endif
                   );

    virtual ~KoFontChooser();

    /// Set the text format to be displayed.
    /// This contains all the attributes - no need to use setFont etc.
    void setFormat( const KoTextFormat& format );

    /// @return the modified text format, once the dialog is closed.
    /// This contains all the attributes - no need to use newFont() etc.
    KoTextFormat newFormat() const;

    /// @return a bitfield (see KoTextFormat) which identifies which
    /// parts of the text format have been modified.
    int changedFlags() const { return m_changedFlags; }

protected:
    void setFont( const QFont &_font, bool _subscript, bool _superscript );
    void setColor( const QColor & col );
    void setBackGroundColor( const QColor & col );

    bool superScript() const { return m_superScript->isChecked(); }
    bool subScript() const { return m_subScript->isChecked(); }
    KoTextFormat::VerticalAlignment vAlign() const {
        return m_subScript->isChecked() ? KoTextFormat::AlignSubScript :
            m_superScript->isChecked() ? KoTextFormat::AlignSuperScript :
            KoTextFormat::AlignNormal; }

    QFont newFont() const { return m_newFont; }
    QColor color() const;
    QColor backGroundColor() const { return m_backGroundColor;}
    QColor underlineColor() const { return m_underlineColor ; }

    void setUnderlineColor( const QColor & col );


    KoTextFormat::UnderlineType underlineType() const;
    KoTextFormat::UnderlineStyle underlineStyle() const;
    KoTextFormat::StrikeOutType strikeOutType() const;
    KoTextFormat::StrikeOutStyle strikeOutStyle() const;

    void setUnderlineType(KoTextFormat::UnderlineType nb);
    void setStrikeOutlineType(KoTextFormat::StrikeOutType nb);
    void setUnderlineStyle(KoTextFormat::UnderlineStyle _t);
    void setStrikeOutStyle(KoTextFormat::StrikeOutStyle _t);

    void setShadow( double shadowDistanceX, double shadowDistanceY, const QColor& shadowColor );
    double shadowDistanceX() const;
    double shadowDistanceY() const;
    QColor shadowColor() const;

    bool wordByWord()const;
    void setWordByWord( bool _b);

    bool hyphenation() const;
    void setHyphenation( bool _b);

    QString language() const;
    void setLanguage( const QString & );

    KoTextFormat::AttributeStyle fontAttribute() const;
    void setFontAttribute( KoTextFormat::AttributeStyle _att);


    double relativeTextSize() const;
    void setRelativeTextSize(double _size);

    int offsetFromBaseLine() const;
    void setOffsetFromBaseLine(int _offset);

    void setupTab1( uint fontListCriteria );
    void setupTab2( bool _withSubSuperScript );
    void updatePositionButton();

protected slots:
    void slotSuperScriptClicked();
    void slotSubScriptClicked();
    void slotStrikeOutTypeChanged( int );
    void slotFontChanged(const QFont &);
    void slotChangeColor();
    void slotChangeBackGroundColor();
    void slotUnderlineColor();
    void slotChangeUnderlineType( int );
    void slotChangeUnderlining( int);

    void slotChangeStrikeOutType( int );
    void slotShadowChanged();
    void slotRelativeSizeChanged( int );
    void slotOffsetFromBaseLineChanged( int );
    void slotChangeAttributeFont( int );
    void slotWordByWordClicked();
    void slotChangeLanguage( int );
    void slotHyphenationClicked();
private:
    KFontChooser *m_chooseFont;
    QCheckBox *m_superScript;
    QCheckBox *m_subScript;

    QComboBox *m_underlining;
    QComboBox *m_underlineType;

    QComboBox *m_strikeOutType;
    QPushButton *m_underlineColorButton;

    QPushButton *m_colorButton;
    QPushButton *m_backGroundColorButton;

    class KoFontChooserPrivate;
    KoFontChooserPrivate* d;

    QFont m_newFont;
    QColor m_backGroundColor;
    QColor m_underlineColor;

    int m_changedFlags;
};

class KoFontDia : public KDialogBase
{
    Q_OBJECT
public:
    KoFontDia( const KoTextFormat& initialFormat,
               QWidget* parent, const char* name );

#ifdef HAVE_LIBKSPELL2
    /// If your application supports spell-checking, pass here the KSpell2 Broker
    /// so that the font dialog can show which languages are supported for spellchecking.
    KoFontDia( const KoTextFormat& initialFormat,
               KSpell2::Broker::Ptr broker,
               QWidget* parent, const char* name );
#endif

    int changedFlags() const { return m_chooser->changedFlags(); }

    KoTextFormat newFormat() const { return m_chooser->newFormat(); }

protected slots:
    void slotReset();
    virtual void slotApply();
    virtual void slotOk();
signals:
    void applyFont();

private:
    void init();

    KoFontChooser * m_chooser;
    KoTextFormat m_initialFormat;
};

#endif
