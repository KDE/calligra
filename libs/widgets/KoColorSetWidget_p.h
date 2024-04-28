/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007, 2012 C. Boemann <cbo@boemann.dk>
   SPDX-FileCopyrightText: 2007-2008 Fredy Yanardi <fyanardi@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KoColorSetWidget_p_h
#define KoColorSetWidget_p_h

#include "KoColorSetWidget.h"

#include <QApplication>
#include <QCheckBox>
#include <QDir>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QPointer>
#include <QScrollArea>
#include <QSize>
#include <QTimer>
#include <QToolButton>
#include <QWidgetAction>

#include <KLocalizedString>
#include <WidgetsDebug.h>

#include <KoColorSet.h>

class KoColorPatch;

class Q_DECL_HIDDEN KoColorSetWidget::KoColorSetWidgetPrivate
{
public:
    KoColorSetWidget *thePublic;
    QPointer<KoColorSet> colorSet;
    QTimer m_timer;
    QVBoxLayout *mainLayout;
    bool firstShowOfContainer;
    QWidget *colorSetContainer;
    QScrollArea *scrollArea;
    QGridLayout *colorSetLayout;
    QHBoxLayout *recentsLayout;
    KoColorPatch *recentPatches[6];
    QToolButton *addRemoveButton;
    int numRecents;

    void colorTriggered(KoColorPatch *patch);
    void addRecent(const KoColor &);
    void activateRecent(int i);
    void fillColors();
    void addRemoveColors();
};

#endif
