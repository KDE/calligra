/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KARBONSMALLSTYLEPREVIEW_H
#define KARBONSMALLSTYLEPREVIEW_H

#include <QWidget>

class KarbonFillStyleWidget;
class KarbonStrokeStyleWidget;
class KoCanvasBase;

/// This is a small widget used on the statusbar, to display fill/stroke colors etc.
class KarbonSmallStylePreview : public QWidget
{
    Q_OBJECT
public:
    explicit KarbonSmallStylePreview(QWidget *parent = nullptr);
    ~KarbonSmallStylePreview() override;

Q_SIGNALS:
    void fillApplied();
    void strokeApplied();

public Q_SLOTS:
    void selectionChanged();
    void canvasChanged(const KoCanvasBase *canvas);

private:
    KarbonFillStyleWidget *m_fillFrame;
    KarbonStrokeStyleWidget *m_strokeFrame;
};

#endif // KARBONSMALLSTYLEPREVIEW_H
