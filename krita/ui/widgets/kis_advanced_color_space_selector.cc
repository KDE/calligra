/*
 *  Copyright (C) 2007 Cyrille Berger <cberger@cberger.net>
 *  Copyright (C) 2011 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (C) 2011 Srikanth Tiyyagura <srikanth.tulasiram@gmail.com>
 *  Copyright (C) 2015 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
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
 
#include "kis_advanced_color_space_selector.h"

#include <KoFileDialog.h>
#include <KoColorProfile.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <KoColorSpaceEngine.h>
#include <KoID.h>

#include <KoConfig.h>
#include <KoIcon.h>

#ifdef GHNS
#include <knewstuff3/downloaddialog.h>
#include <knewstuff3/uploaddialog.h>
#endif

#include <QDesktopServices>

#include <kcomponentdata.h>
#include <kstandarddirs.h>
#include <kglobal.h>

#include "kis_factory2.h"

#include "ui_wdgcolorspaceselectoradvanced.h"

struct KisAdvancedColorSpaceSelector::Private {
    Ui_WdgColorSpaceSelectorAdvanced* colorSpaceSelector;
    QString knsrcFile;
};

KisAdvancedColorSpaceSelector::KisAdvancedColorSpaceSelector(QWidget* parent, const QString &caption) : QDialog(parent), d(new Private)
{
    
    setObjectName("KisAdvancedColorSpaceSelector");
    setWindowTitle(caption);
    d->colorSpaceSelector = new Ui_WdgColorSpaceSelectorAdvanced;
    d->colorSpaceSelector->setupUi(this);
    d->colorSpaceSelector->cmbColorModels->setIDList(KoColorSpaceRegistry::instance()->colorModelsList(KoColorSpaceRegistry::OnlyUserVisible));
    fillCmbDepths(d->colorSpaceSelector->cmbColorModels->currentItem());

    d->colorSpaceSelector->bnInstallProfile->setIcon(themedIcon("document-open"));
    d->colorSpaceSelector->bnInstallProfile->setToolTip( i18n("Open Color Profile") );
    connect(d->colorSpaceSelector->cmbColorModels, SIGNAL(activated(const KoID &)),
            this, SLOT(fillCmbDepths(const KoID &)));
    connect(d->colorSpaceSelector->cmbColorDepth, SIGNAL(activated(const KoID &)),
            this, SLOT(fillLstProfiles()));
    connect(d->colorSpaceSelector->cmbColorModels, SIGNAL(activated(const KoID &)),
            this, SLOT(fillLstProfiles()));
    connect(d->colorSpaceSelector->lstProfile, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(colorSpaceChanged()));
    //connect(d->colorSpaceSelector->lstProfile, SIGNAL(currentItemChanged(int)),
    //        this, SLOT(buttonUpdate()));
    connect(this, SIGNAL(selectionChanged(bool)),
            this, SLOT(fillDescription()));
    connect(this, SIGNAL(selectionChanged(bool)), d->colorSpaceSelector->TongueWidget, SLOT(repaint()));


    connect(d->colorSpaceSelector->bnInstallProfile, SIGNAL(clicked()), this, SLOT(installProfile()));
    
    //connect(d->colorSpaceSelector->bnOK, SIGNAL(accepted()), this, (accept()));
    //connect(d->colorSpaceSelector->bnOK, SIGNAL(rejected()), this, (reject()));
    
    d->knsrcFile = "kritaiccprofiles.knsrc";
    fillLstProfiles();
}

KisAdvancedColorSpaceSelector::~KisAdvancedColorSpaceSelector()
{
    delete d->colorSpaceSelector;
    delete d;
}

void KisAdvancedColorSpaceSelector::fillLstProfiles()
{
    QString s = KoColorSpaceRegistry::instance()->colorSpaceId(d->colorSpaceSelector->cmbColorModels->currentItem(), d->colorSpaceSelector->cmbColorDepth->currentItem());
    d->colorSpaceSelector->lstProfile->clear();

    const KoColorSpaceFactory * csf = KoColorSpaceRegistry::instance()->colorSpaceFactory(s);
    if (csf == 0) return;//TODO: make this give better feedback.

    QList<const KoColorProfile *>  profileList = KoColorSpaceRegistry::instance()->profilesFor(csf);
    QListWidgetItem *defaultProfile = new QListWidgetItem;
    defaultProfile->setText(csf->defaultProfile()+" "+i18n("(Default)"));
    foreach(const KoColorProfile *profile, profileList) {
        if (profile->name()==csf->defaultProfile()) {
            d->colorSpaceSelector->lstProfile->addItem(defaultProfile);
        } else {
            d->colorSpaceSelector->lstProfile->addItem(profile->name());
        }
    }
    d->colorSpaceSelector->lstProfile->setCurrentItem(defaultProfile);
    colorSpaceChanged();
}

void KisAdvancedColorSpaceSelector::fillCmbDepths(const KoID& id)
{
    KoID activeDepth = d->colorSpaceSelector->cmbColorDepth->currentItem();
    d->colorSpaceSelector->cmbColorDepth->clear();
    QList<KoID> depths = KoColorSpaceRegistry::instance()->colorDepthList(id, KoColorSpaceRegistry::OnlyUserVisible);
    d->colorSpaceSelector->cmbColorDepth->setIDList(depths);
    if (depths.contains(activeDepth)) {
        d->colorSpaceSelector->cmbColorDepth->setCurrent(activeDepth);
    }
}

void KisAdvancedColorSpaceSelector::fillDescription()
{
    QString notApplicable = i18n("N/A");
    QString notApplicableTooltip = i18n("This profile has no colorants.");
    QString profileName = "No Profile Found";
    //set colorants
    QString s = KoColorSpaceRegistry::instance()->colorSpaceId(d->colorSpaceSelector->cmbColorModels->currentItem(), d->colorSpaceSelector->cmbColorDepth->currentItem());
    const KoColorSpaceFactory * csf = KoColorSpaceRegistry::instance()->colorSpaceFactory(s);
    if (csf == 0) return;
    QList<const KoColorProfile *>  profileList = KoColorSpaceRegistry::instance()->profilesFor(csf);
    if (profileList.isEmpty()==false) {
    profileName = currentColorSpace()->profile()->name();
        if (currentColorSpace()->profile()->hasColorants()){
            QVector <double> colorants = currentColorSpace()->profile()->getColorantsxyY();
            QVector <double> whitepoint = currentColorSpace()->profile()->getWhitePointxyY();
            //QString text = currentColorSpace()->profile()->info()+" ="+
            d->colorSpaceSelector->TongueWidget->setEnabled(true);
            d->colorSpaceSelector->TongueWidget->setProfileData(colorants, whitepoint, true);
            d->colorSpaceSelector->lblXYZ_W->setText(QString::number(whitepoint[0])+", "+QString::number(whitepoint[1])+", "+QString::number(whitepoint[2]));
            d->colorSpaceSelector->lblXYZ_R->setText(QString::number(colorants[0])+", "+QString::number(colorants[1])+", "+QString::number(colorants[2]));
            d->colorSpaceSelector->lblXYZ_G->setText(QString::number(colorants[3])+", "+QString::number(colorants[4])+", "+QString::number(colorants[5]));
            d->colorSpaceSelector->lblXYZ_B->setText(QString::number(colorants[6])+", "+QString::number(colorants[7])+", "+QString::number(colorants[8]));
            d->colorSpaceSelector->lblXYZ_R->setToolTip("Colorant in d50-adapted xyY.");
            d->colorSpaceSelector->lblXYZ_G->setToolTip("Colorant in d50-adapted xyY.");
            d->colorSpaceSelector->lblXYZ_B->setToolTip("Colorant in d50-adapted xyY.");
        } else {
            QVector <double> whitepoint2 = currentColorSpace()->profile()->getWhitePointxyY();
            d->colorSpaceSelector->TongueWidget->setEnabled(false);
            d->colorSpaceSelector->lblXYZ_W->setText(QString::number(whitepoint2[0])+", "+QString::number(whitepoint2[1])+", "+QString::number(whitepoint2[2]));
            
            d->colorSpaceSelector->lblXYZ_R->setText(notApplicable);
            d->colorSpaceSelector->lblXYZ_R->setToolTip(notApplicableTooltip);
            d->colorSpaceSelector->lblXYZ_G->setText(notApplicable);
            d->colorSpaceSelector->lblXYZ_G->setToolTip(notApplicableTooltip);
            d->colorSpaceSelector->lblXYZ_B->setText(notApplicable);
            d->colorSpaceSelector->lblXYZ_B->setToolTip(notApplicableTooltip);
        }
    } else {
        d->colorSpaceSelector->TongueWidget->setEnabled(false);
        d->colorSpaceSelector->lblXYZ_W->setText(notApplicable);
        d->colorSpaceSelector->lblXYZ_W->setToolTip(notApplicableTooltip);
        d->colorSpaceSelector->lblXYZ_R->setText(notApplicable);
        d->colorSpaceSelector->lblXYZ_R->setToolTip(notApplicableTooltip);
        d->colorSpaceSelector->lblXYZ_G->setText(notApplicable);
        d->colorSpaceSelector->lblXYZ_G->setToolTip(notApplicableTooltip);
        d->colorSpaceSelector->lblXYZ_B->setText(notApplicable);
        d->colorSpaceSelector->lblXYZ_B->setToolTip(notApplicableTooltip);
    }
    
    //set TRC
    QVector <double> estimatedTRC(3);
    QString currentModelStr = d->colorSpaceSelector->cmbColorModels->currentItem().id();
    if (currentModelStr == "RGBA") {
        estimatedTRC = currentColorSpace()->profile()->getEstimatedTRC();
        if (estimatedTRC[0] == -1) {
            d->colorSpaceSelector->lbltrc->setToolTip("The Tone Response curve of this color space is the sRGB trc, which is approcimate 2.2");
            d->colorSpaceSelector->lbltrc->setText("Estimated Gamma: sRGB trc");
        } else {
        d->colorSpaceSelector->lbltrc->setToolTip("Estimated Gamma: "+QString::number(estimatedTRC[0])+","+QString::number(estimatedTRC[1])+","+QString::number(estimatedTRC[2]));
        d->colorSpaceSelector->lbltrc->setText("Estimated Gamma: "+QString::number((estimatedTRC[0]+estimatedTRC[1]+estimatedTRC[2])/3));
        }
    } else if (currentModelStr == "GRAYA") {
            estimatedTRC = currentColorSpace()->profile()->getEstimatedTRC();
        if (estimatedTRC[0] == -1) {
            d->colorSpaceSelector->lbltrc->setToolTip("The Tone Response curve of this color space is the sRGB trc, which is approcimate 2.2");
            d->colorSpaceSelector->lbltrc->setText("Estimated Gamma: sRGB trc");
        } else {
            d->colorSpaceSelector->lbltrc->setToolTip("Estimated Gamma: "+QString::number(estimatedTRC[0]));
            d->colorSpaceSelector->lbltrc->setText("Estimated Gamma: "+QString::number(estimatedTRC[0]));
        }
    } else if (currentModelStr == "CMYKA") {
        d->colorSpaceSelector->lbltrc->setToolTip("Estimated Gamma can't be retrieved for CMYK");
        d->colorSpaceSelector->lbltrc->setText("Estimated Gamma: N/A");
    } else if (currentModelStr == "XYZA") {
        d->colorSpaceSelector->lbltrc->setToolTip("We asume that XYZ is linear");
        d->colorSpaceSelector->lbltrc->setText("Estimated Gamma: 1.0");
    } else if (currentModelStr == "LABA") {
        d->colorSpaceSelector->lbltrc->setToolTip("We asume this is the L *star TRC");
        d->colorSpaceSelector->lbltrc->setText("Estimated Gamma: L*");
    } else if (currentModelStr == "YCbCrA") {
        d->colorSpaceSelector->lbltrc->setToolTip("Estimated Gamma can't be retrieved for YCRCB");
        d->colorSpaceSelector->lbltrc->setText("Estimated Gamma: N/A");
    }

    d->colorSpaceSelector->textProfileDescription->clear();
    d->colorSpaceSelector->textProfileDescription->setPlainText(profileName);
    d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
    d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("The following information about this colorprofile and depth is availeble:"));
    d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");

    if (currentModelStr == "RGBA") {
        d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("RGB, standing for Red, Green, Blue, is the colour model used by screens and other light-based media. It is an additive colour model, meaning that adding colours together makes them brighter. This colour model is the most extensive of all colour models, and is recommended as a model for working files, that you can later convert to other spaces. RGB is also the recommended space for HDR editing."));
    } else if (currentModelStr == "CMYKA") {
            d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("CMYK, standing for Cyan, Magenta, Yellow, Key, is the model used by printers and other ink-based media. It is a subtractive model, meaning that adding colors together will turn them darker. Because of CMYK profiles being very specific per printer, it is recommended to work in RGB space, and then later convert to a CMYK profile, preferably one delivered by you printer. Unfortunatly, Krita can't retrieve colorants or the TRC for this space."));
    } else if (currentModelStr == "XYZA") {
            d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("XYZ is the space determined by the CIE as the space that encompasses all other colors, and used to convert colors between profiles. It is an additive colour model, meaning that adding colours together makes them brighter. It isn't recommended for editing, but can be useful to encode in. TRC is assumed to be linear."));
    } else if (currentModelStr == "GRAYA") {
            d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("Grayscale, as it states, allows only for gray values and transparent values. The benefit of this is that it will take up twice as little RAM and harddrive space as any given RGB image of the same bit-depth. Useful for inking and greyscale images."));
    } else if (currentModelStr == "LABA") {
            d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("LAB, standing for Lightness, A component, B component, is a special model for color correction. It is based on human perception, meaning that it tries to encode the difference in lightness, red-green balance and yellow-blue balance. This makes it useful for color correction, but the vast majority of color maths in the blending modes don't work as expected here. Similarly, we do not support HDR in LAB, meaning that HDR images converted to LAB lose color information. This space is not recommended for painting, nor for export, but best as a space to do post-processing in. TRC is assumed to be the L* TRC."));
    } else if (currentModelStr == "YCbCrA") {
            d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("YCrCb, standing for Luma, Red Chroma, Blue Chroma, is a model designed for video encoding. It is based on human perception, meaning that it tries to encode the difference in lightness, red-green balance and yellow-blue balance. It is there to open up certain images correctly, but Krita does not currently ship a profile for this due to lack of open source ICC profiles for YCrCb."));
    }
    /*
    if (currentColorSpace()->colorDepthId().id()=="U8" && //profile larger than sRGB//){
    d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("This space is larger than generic screen RGB(sRGB), which means that it can show visible banding in 8bit. We recommend using a higher bit-depth for this image to ensure smoothness of values."));
    }*/
    QString currentDepthStr = d->colorSpaceSelector->cmbColorDepth->currentItem().id();
    d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
    if (currentDepthStr == "U8") {
            d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("8bit: The default amount of colours per channel. Each channel will have 256 values available, leading to a total amount of 256*amount of channels. Recommended to use for images intended for the web, or otherwise simple images."));
    } else if (currentDepthStr == "U16") {
            d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("16bit: Also known as 'deep colour'. 16bit is ideal for editing images with a linear TRC, large colour space, or just when you need more precise colour blending. This does take twice as much space on the RAM and hard-drive than any given 8bit image of the same properties, and for some devices it takes much more processing power. We recommend watching the RAM usage of the file carefully, or otherwise use 8bit if your computer slows down."));
    } else if (currentDepthStr == "F16") {
            d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("16bit float: Also known as 'Half Floating Point', and the standard in VFX industry images. 16bit float is ideal for editing images with a linear TRC, large colour space, or just when you need more precise colour blending. It being floating point is an absolute requirement for Scene Reffered (HDR) images. This does take twice as much space on the RAM and hard-drive than any given 8bit image of the same properties, and for some devices it takes much more processing power. We recommend watching the RAM usage of the file carefully, or otherwise use 8bit if your computer slows down."));
    } else if (currentDepthStr == "F32") {
            d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("32bit float: Also known as 'Full Floating Point'. 32bit float is ideal for editing images with a linear TRC, large colour space, or just when you need more precise colour blending. It being floating point is an absolute requirement for Scene Reffered (HDR) images. This does take four times as much space on the RAM and hard-drive than any given 8bit image of the same properties, and for some devices it takes much more processing power. We recommend watching the RAM usage of the file carefully, or otherwise use 8bit if your computer slows down."));
    } else if (currentDepthStr == "F64") {
            d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("64bit float: 64bit float is as precise as it gets in current technology, and this depth is used most of the time for images that are generated or used as an input for software. It being floating point is an absolute requirement for Scene Reffered (HDR) images. This does take eight times as much space on the RAM and hard-drive than any given 8bit image of the same properties, and for some devices it takes much more processing power. We recommend watching the RAM usage of the file carefully, or otherwise use 8bit if your computer slows down."));
    }
            
}

const KoColorSpace* KisAdvancedColorSpaceSelector::currentColorSpace()
{
    QString check = "";
    if (d->colorSpaceSelector->lstProfile->currentItem()) {
        check = d->colorSpaceSelector->lstProfile->currentItem()->text();
    } else {
        check = d->colorSpaceSelector->lstProfile->item(0)->text();
    }
    //qDebug()<<check;
    return KoColorSpaceRegistry::instance()->colorSpace(
               d->colorSpaceSelector->cmbColorModels->currentItem().id(), d->colorSpaceSelector->cmbColorDepth->currentItem().id()
               , check);
}

void KisAdvancedColorSpaceSelector::setCurrentColorModel(const KoID& id)
{
    d->colorSpaceSelector->cmbColorModels->setCurrent(id);
    fillCmbDepths(id);
}

void KisAdvancedColorSpaceSelector::setCurrentColorDepth(const KoID& id)
{
    d->colorSpaceSelector->cmbColorDepth->setCurrent(id);
    fillLstProfiles();
}

void KisAdvancedColorSpaceSelector::setCurrentProfile(const QString& name)
{
    QList<QListWidgetItem *> Items= d->colorSpaceSelector->lstProfile->findItems(name, Qt::MatchStartsWith);
    d->colorSpaceSelector->lstProfile->setCurrentItem(Items.at(0));
}

void KisAdvancedColorSpaceSelector::setCurrentColorSpace(const KoColorSpace* colorSpace)
{
  setCurrentColorModel(colorSpace->colorModelId());
  setCurrentColorDepth(colorSpace->colorDepthId());
  setCurrentProfile(colorSpace->profile()->name());
}

void KisAdvancedColorSpaceSelector::colorSpaceChanged()
{
    bool valid = d->colorSpaceSelector->lstProfile->count() != 0;
    emit(selectionChanged(valid));
    if(valid) {
        emit colorSpaceChanged(currentColorSpace());
    }
}

void KisAdvancedColorSpaceSelector::installProfile()
{
    QStringList mime;
    mime << "*.icm" <<  "*.icc";
    KoFileDialog dialog(this, KoFileDialog::OpenFiles, "OpenDocumentICC");
    dialog.setCaption(i18n("Install Color Profiles"));
    dialog.setDefaultDir(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
    dialog.setNameFilters(mime);
    QStringList profileNames = dialog.urls();

    KoColorSpaceEngine *iccEngine = KoColorSpaceEngineRegistry::instance()->get("icc");
    Q_ASSERT(iccEngine);

    QString saveLocation = KGlobal::mainComponent().dirs()->saveLocation("icc_profiles");

    foreach (const QString &profileName, profileNames) {
        KUrl file(profileName);
        if (!QFile::copy(profileName, saveLocation + file.fileName())) {
            kWarning() << "Could not install profile!";
            return;
        }
        iccEngine->addProfile(saveLocation + file.fileName());

    }

    fillLstProfiles();
}

void KisAdvancedColorSpaceSelector::buttonUpdate()
{
   const KoColorProfile *  profile = KoColorSpaceRegistry::instance()->profileByName(d->colorSpaceSelector->lstProfile->currentItem()->text());
   if(!profile)  return;

   QFileInfo fileInfo(profile->fileName());
   /*if(fileInfo.isWritable()) {
       d->colorSpaceSelector->bnUploadProfile->setEnabled( true );
       return;
   }
   d->colorSpaceSelector->bnUploadProfile->setEnabled( false );*/
}

#include "kis_advanced_color_space_selector.moc"
