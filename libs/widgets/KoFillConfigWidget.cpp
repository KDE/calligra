/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * Copyright (C) 2012 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoFillConfigWidget.h"

#include <QToolButton>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QSizePolicy>

#include <klocale.h>

#include <KoGroupButton.h>
#include <KoIcon.h>
#include <KoColor.h>
#include <KoColorPopupAction.h>
#include <KoSliderCombo.h>
#include <KoCanvasController.h>
#include <KoSelection.h>
#include <KoToolManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoCanvasResourceManager.h>
#include <KoDocumentResourceManager.h>
#include <KoShapeManager.h>
#include <KoShapeBackground.h>

static const char* const buttonnone[]={
    "16 16 3 1",
    "# c #333333",
    "e c #ff0000",
    "- c #ffffff",
    "################",
    "#------------ee#",
    "#-----------eee#",
    "#----------eee-#",
    "#---------eee--#",
    "#--------eee---#",
    "#-------eee----#",
    "#------eee-----#",
    "#-----eee------#",
    "#----eee-------#",
    "#---eee--------#",
    "#--eee---------#",
    "#-eee----------#",
    "#eee-----------#",
    "#ee------------#",
    "################"};

static const char* const buttonsolid[]={
    "16 16 2 1",
    "# c #000000",
    ". c #969696",
    "################",
    "#..............#",
    "#..............#",
    "#..............#",
    "#..............#",
    "#..............#",
    "#..............#",
    "#..............#",
    "#..............#",
    "#..............#",
    "#..............#",
    "#..............#",
    "#..............#",
    "#..............#",
    "#..............#",
    "################"};


// FIXME: Smoother gradient button.

static const char* const buttongradient[]={
    "16 16 15 1",
    "# c #000000",
    "n c #101010",
    "m c #202020",
    "l c #303030",
    "k c #404040",
    "j c #505050",
    "i c #606060",
    "h c #707070",
    "g c #808080",
    "f c #909090",
    "e c #a0a0a0",
    "d c #b0b0b0",
    "c c #c0c0c0",
    "b c #d0d0d0",
    "a c #e0e0e0",
    "################",
    "#abcdefghijklmn#",
    "#abcdefghijklmn#",
    "#abcdefghijklmn#",
    "#abcdefghijklmn#",
    "#abcdefghijklmn#",
    "#abcdefghijklmn#",
    "#abcdefghijklmn#",
    "#abcdefghijklmn#",
    "#abcdefghijklmn#",
    "#abcdefghijklmn#",
    "#abcdefghijklmn#",
    "#abcdefghijklmn#",
    "#abcdefghijklmn#",
    "#abcdefghijklmn#",
    "################"};

static const char* const buttonpattern[]={
    "16 16 4 1",
    ". c #0a0a0a",
    "# c #333333",
    "a c #a0a0a0",
    "b c #ffffffff",
    "################",
    "#aaaaabbbbaaaaa#",
    "#aaaaabbbbaaaaa#",
    "#aaaaabbbbaaaaa#",
    "#aaaaabbbbaaaaa#",
    "#aaaaabbbbaaaaa#",
    "#bbbbbaaaabbbbb#",
    "#bbbbbaaaabbbbb#",
    "#bbbbbaaaabbbbb#",
    "#bbbbbaaaabbbbb#",
    "#aaaaabbbbaaaaa#",
    "#aaaaabbbbaaaaa#",
    "#aaaaabbbbaaaaa#",
    "#aaaaabbbbaaaaa#",
    "#aaaaabbbbaaaaa#",
    "################"};

class KoFillConfigWidget::Private
{
public:
    Private()
    : canvas(0)
    {
    }

    QToolButton *colorButton;
    KoColorPopupAction *colorAction;
    QButtonGroup *group;

    KoSliderCombo *opacity;

    QWidget *spacer;
    KoCanvasBase *canvas;
};

KoFillConfigWidget::KoFillConfigWidget(QWidget * parent)
:  QWidget(parent)
, d(new Private())
{
    setObjectName("Fill widget");
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    d->colorButton = new QToolButton(this);
    layout->addWidget(d->colorButton);
    d->colorAction = new KoColorPopupAction(this);
    d->colorAction->setIcon(koIcon("format-stroke-color"));
    d->colorAction->setToolTip(i18n("Change the filling color"));
    connect(d->colorAction, SIGNAL(colorChanged(const KoColor &)), this, SIGNAL(colorChanged(const KoColor &)));
    d->colorButton->setDefaultAction(d->colorAction);

    d->group = new QButtonGroup(this);
    d->group->setExclusive(true);

    // The button for no fill
    KoGroupButton *button = new KoGroupButton(KoGroupButton::GroupLeft, this);
    button->setIcon(koIcon("edit-delete"));
    button->setToolTip(i18nc("No stroke or fill", "None"));
    d->group->addButton(button, None);
    layout->addWidget(button);

    // The button for solid fill
    button = new KoGroupButton(KoGroupButton::GroupCenter, this);
    button->setIcon(QPixmap((const char **) buttonsolid));
    button->setToolTip(i18nc("Solid color stroke or fill", "Solid"));
    d->group->addButton(button, Solid);
    layout->addWidget(button);

    // The button for gradient fill
    button = new KoGroupButton(KoGroupButton::GroupCenter, this);
    button->setIcon(QPixmap((const char **) buttongradient));
    button->setToolTip(i18n("Gradient"));
    d->group->addButton(button, Gradient);
    layout->addWidget(button);

    // The button for pattern fill
    button = new KoGroupButton(KoGroupButton::GroupRight, this);
    button->setIcon(QPixmap((const char **) buttonpattern));
    button->setToolTip(i18n("Pattern"));
    d->group->addButton(button, Pattern);
    layout->addWidget(button);

    connect(d->group, SIGNAL(buttonClicked(int)), this, SLOT(styleButtonPressed(int)));

    // Opacity setting
    // FIXME: There is also an opacity setting in the color chooser. How do they interact?
    d->opacity = new KoSliderCombo(this);
    d->opacity->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    d->opacity->setMinimum(0);
    d->opacity->setMaximum(100);
    d->opacity->setValue(100);
    d->opacity->setDecimals(0);
    connect(d->opacity, SIGNAL(valueChanged(qreal, bool)), this, SIGNAL(opacityChanged(qreal)));

    layout->addWidget(new QLabel(i18n("Opacity:")));
    layout->addWidget(d->opacity);

    // Spacer
    d->spacer = new QWidget();
    d->spacer->setObjectName("SpecialSpacer");
    layout->addWidget(d->spacer);
}

KoFillConfigWidget::~KoFillConfigWidget()
{
    delete d;
}

void KoFillConfigWidget::setCanvas( KoCanvasBase *canvas )
{
    d->canvas = canvas;
}

void KoFillConfigWidget::styleButtonPressed(int buttonId)
{
    switch (buttonId) {
        case KoFillConfigWidget::None:
            // Direct manipulation

        case KoFillConfigWidget::Solid:
            // Only select mode in the widget, don't set actual gradient :/ .

            break;
        case KoFillConfigWidget::Gradient:
            // Only select mode in the widget, don't set actual gradient :/ .

            break;
        case KoFillConfigWidget::Pattern:
            // Only select mode in the widget, don't set actual pattern :/ .

            break;
    }
}

void KoFillConfigWidget::applyChanges()
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();

    //FIXME d->canvas->resourceManager()->setActiveStroke( d->stroke );

    if (!selection || !selection->count())
        return;
}


#include <KoFillConfigWidget.moc>
