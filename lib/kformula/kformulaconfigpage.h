/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef kfconfigpages_h
#define kfconfigpages_h

#include <qfont.h>
#include <qvaluevector.h>

#include <kdialogbase.h>

#include "kformuladefs.h"

class QButtonGroup;
class QCheckBox;
class QColor;
class QGridLayout;
class QGroupBox;
class QLabel;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QStringList;
class QWidget;

class KColorButton;
class KConfig;
class KListView;
class KPushButton;
class KIntNumInput;

KFORMULA_NAMESPACE_BEGIN


class Document;

class KOFORMULA_EXPORT ConfigurePage : public QObject
{
    Q_OBJECT
public:

    ConfigurePage( Document* document, QWidget* view, KConfig* config, QVBox* box, char* name = 0 );
    void apply();
    void slotDefault();

protected:

    bool selectFont( QFont & );

protected slots:

    void syntaxHighlightingClicked();
    void selectNewDefaultFont();
    void selectNewNameFont();
    void selectNewNumberFont();
    void selectNewOperatorFont();

    void baseSizeChanged( int value );
    void slotChanged();

private:

    QPushButton* buildFontLine( QWidget* fontWidget, QGridLayout* layout, int number,
                                QFont font, QString name, QLabel*& fontName );

    void updateFontLabel( QFont font, QLabel* label );

    Document* m_document;
    QWidget* m_view;
    KConfig* m_config;

    QFont defaultFont;
    QFont nameFont;
    QFont numberFont;
    QFont operatorFont;

    KIntNumInput* sizeSpin;

    QLabel* defaultFontName;
    QLabel* nameFontName;
    QLabel* numberFontName;
    QLabel* operatorFontName;

//     KColorButton* defaultColorBtn;
//     KColorButton* numberColorBtn;
//     KColorButton* operatorColorBtn;
//     KColorButton* emptyColorBtn;
//     KColorButton* errorColorBtn;
    QCheckBox* syntaxHighlighting;
//     QGroupBox* hlBox;

    QButtonGroup* styleBox;
    QRadioButton* symbolStyle;
    QRadioButton* esstixStyle;
    QRadioButton* cmStyle;

    bool m_changed;
};


// class MathFontsConfigurePage : public QObject
// {
//     Q_OBJECT
// public:

//     MathFontsConfigurePage( Document* document, QWidget* view, KConfig* config, QVBox* box, char* name = 0 );
//     void apply();
//     void slotDefault();

//     QValueVector<QString>::iterator findUsedFont( QString name );

// protected slots:

//     void slotAddFont();
//     void slotRemoveFont();
//     void slotMoveUp();
//     void slotMoveDown();

// private:

//     void setupLists( const QStringList& usedFonts );

//     Document* m_document;
//     QWidget* m_view;
//     KConfig* m_config;

//     KListView* availableFonts;
//     KListView* requestedFonts;

//     KPushButton* addFont;
//     KPushButton* removeFont;
//     KPushButton* moveUp;
//     KPushButton* moveDown;

//     QValueVector<QString> usedFontList;
// };


KFORMULA_NAMESPACE_END

#endif // kfconfigpages_h
