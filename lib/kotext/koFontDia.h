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
     */
    KoFontChooser( QWidget * parent, const char* name = 0L,
            bool _withSubSuperScript = true, uint fontListCriteria=0);
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

    /*
    bool getHyphenation() const { return m_chooser->getHyphenation(); }
    bool getSuperScript() const { return m_chooser->getSuperScript(); }
    bool getSubScript() const { return m_chooser->getSubScript(); }
    QFont getNewFont() const { return m_chooser->getNewFont(); }
    QColor color() const { return m_chooser->color(); }
    QColor backGroundColor() const {return m_chooser->backGroundColor();}
    QColor underlineColor() const { return m_chooser->underlineColor() ; }
    KoTextFormat::UnderlineType getUnderlineType() const { return m_chooser->getUnderlineType();}
    KoTextFormat::StrikeOutType getStrikeOutType() const { return m_chooser->getStrikeOutType();}

    KoTextFormat::UnderlineStyle getUnderlineStyle() const { return m_chooser->getUnderlineStyle();}
    KoTextFormat::StrikeOutStyle getStrikeOutStyle() const { return m_chooser->getStrikeOutStyle();}

    double shadowDistanceX() const { return m_chooser->shadowDistanceX(); }
    double shadowDistanceY() const { return m_chooser->shadowDistanceY(); }
    QColor shadowColor() const { return m_chooser->shadowColor(); }

    double getRelativeTextSize()const{ return m_chooser->getRelativeTextSize();}

    int getOffsetFromBaseLine() const {return m_chooser->getOffsetFromBaseLine();}
    bool getWordByWord()const{ return m_chooser->getWordByWord();}

    QString getLanguage() const { return m_chooser->getLanguage();}

    KoTextFormat::AttributeStyle getFontAttribute()const { return m_chooser->getFontAttribute();}
    */

    int changedFlags() const { return m_chooser->changedFlags(); }

    KoTextFormat newFormat() const { return m_chooser->newFormat(); }

protected slots:
    void slotReset();
    virtual void slotApply();
    virtual void slotOk();
signals:
    void applyFont();

private:
    KoFontChooser * m_chooser;
    /*
    QFont m_font;
    bool m_bSubscript;
    bool m_bSuperscript;
    bool m_bStrikeOut;
    QColor m_color;
    QColor m_backGroundColor;
    QColor m_underlineColor;
    KoTextFormat::UnderlineType m_underlineType;
    KoTextFormat::UnderlineStyle m_underlineStyle;
    KoTextFormat::StrikeOutStyle m_strikeOutStyle;
    KoTextFormat::StrikeOutType m_strikeOutType;
    double m_relativeSize;
    int m_offsetBaseLine;
    bool m_bWordByWord;
    bool m_bHyphenation;
    KoTextFormat::AttributeStyle m_fontAttribute;
    QString m_language;
    */
    KoTextFormat m_initialFormat;
};

#endif
