/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef PropertyEditor_h_
#define PropertyEditor_h_

#include <kdialogbase.h>

class QComboBox;
class GDocument;
class CommandHistory;
class UnitBox;
class Gradient;
class BrushCells;

class QColor;
class QLabel;
class QComboBox;
class QPushButton;
class QSlider;
class QRadioButton;
class QWidgetStack;
class QSpinBox;
class KColorButton;
class KFontChooser;

// This *huge* class is needed to present the preview pixmap.
// It is simply a plain Widget which tries to get all the free
// space it can get (in x and y direction). (Werner)
class PWidget : public QWidget {

public:
    PWidget(QWidget *w) : QWidget(w) {}
    virtual QSize minimumSizeHint() const { return QSize(10, 50); }
    virtual QSizePolicy sizePolicy() const { return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); }
};

class PropertyEditor : public KDialogBase {

    Q_OBJECT

public:
    PropertyEditor (CommandHistory* history, GDocument* doc,
                    QWidget* parent = 0L, const char* name = 0L);

    static int edit (CommandHistory* history, GDocument* doc);

protected:
    void createInfoWidget (QWidget* parent);
    void createOutlineWidget (QWidget* parent);
    void createFillWidget (QWidget* parent);
    void createFontWidget (QWidget* parent);

private slots:
    void applyPressed ();
    void fillStyleChanged ();
    void fillPatternColorChanged (const QColor&);
    void gradientColorChanged (const QColor&);
    void gradientStyleChanged (int);
    void gradientAngleChanged (int);

private:
    void readProperties ();
    void updateGradient ();

    GDocument* document;
    CommandHistory* cmdHistory;
    bool isGlobal:1;
    bool haveTextObjects:1;
    bool haveLineObjects:1;
    bool haveEllipseObjects:1;
    bool haveRectangleObjects:1;
    bool haveFillObjects:1;
    bool haveArrows:1;

    // Info Tab
    QLabel* infoLabel[5];

    // OutlinePen Tab
    UnitBox *widthField;
    KColorButton* penColorBttn;
    QComboBox* penStyleField;
    QComboBox *leftArrows, *rightArrows;
    QPushButton *ellipseKind[3];
    QPushButton *textAlign[3];
    QSlider* roundnessSlider;

    // Fill Tab
    QRadioButton *fillStyleBttn[5];
    KColorButton *fillSolidColor;
    KColorButton *fillPatternColor;
    QComboBox *gradStyleCombo;
    KColorButton *fillColorBtn1, *fillColorBtn2;
    PWidget *gradPreview;
    QWidgetStack *wstack;
    Gradient *gradient;
    BrushCells *brushCells;
    QSpinBox *gradientAngle;

    KFontChooser *fontChooser;
};

#endif
