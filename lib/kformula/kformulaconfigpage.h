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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef kfconfigpages_h
#define kfconfigpages_h

#include <qfont.h>

#include <kdialogbase.h>

#include "kformuladefs.h"

class QCheckBox;
class QColor;
class QGridLayout;
class QLabel;
class QPushButton;
class QSpinBox;
class QWidget;

class KColorButton;
class KConfig;



KFORMULA_NAMESPACE_BEGIN


class Document;

class ConfigurePage : public QObject
{
    Q_OBJECT
public:

    ConfigurePage( Document* document, QWidget* view, KConfig* config, QVBox* box, char* name = 0 );
    void apply();
    void slotDefault();

protected slots:

    void syntaxHighlightingClicked();
    void selectNewDefaultFont();
    void selectNewNameFont();
    void selectNewNumberFont();
    void selectNewOperatorFont();

    void baseSizeChanged( int value );

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
    QSpinBox* sizeSpin;

    QLabel* defaultFontName;
    QLabel* nameFontName;
    QLabel* numberFontName;
    QLabel* operatorFontName;

    KColorButton* defaultColorBtn;
    KColorButton* numberColorBtn;
    KColorButton* operatorColorBtn;
    KColorButton* emptyColorBtn;
    KColorButton* errorColorBtn;
    QCheckBox* syntaxHighlighting;
};


KFORMULA_NAMESPACE_END

#endif // kfconfigpages_h
