/*
 *  Copyright (c) 2012 Joseph Simon <jsimon383@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KISPRINTERCOLORMANAGER_H
#define KISPRINTERCOLORMANAGER_H

#include <QWidget>
#include <QMap>
#include <QPrinterInfo>
#include <QFutureWatcher>

#include "kis_config.h"
#include "ui_wdgprintcolorsettings.h"
#include "ui_wdgprintcolorspaceselector.h"
#include <kis_image.h>

class KisCmpx;
class KoColorSpace;
class KoColorProfile;
class KisPrinterProfileChooser;
class KisConfig;

class KisPrinterColorManager : public QWidget, Ui::WdgPrintColorSettings
{
    Q_OBJECT
public:
    KisPrinterColorManager(QWidget *parent = 0);
    virtual ~KisPrinterColorManager();
    
    virtual void populatePrinterList();
    virtual void simulatePrintJob();
    virtual void registerCurrentProfile();
    virtual void setCurrentProfile(const KoColorProfile *);
    
private slots:
    void slotChangePrinterSelection(int);
    void slotSetAutoCheckBox(bool);
    void slotFinishedAutoProfile(void);
    void slotUserProfileChanged(const KoColorSpace*);
    //void slotPrint(void);
    
private:
    KisCmpx *m_colormanager;
    KisConfig m_cfg;
    QList< QMap<QString, QString> > m_profilemap;     
    QList<QPrinterInfo> m_printerlist;
    QFutureWatcher<void> m_watcher;
    const KoColorProfile * m_profile;
    KisPrinterProfileChooser * m_selectorui;
    QPrinter *m_currentprinter;
    KisImageWSP m_image;
};

#endif