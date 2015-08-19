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
    
    d->colorSpaceSelector->bnDownloadProfile->setIcon(themedIcon("download"));
    d->colorSpaceSelector->bnDownloadProfile->setToolTip( i18n("Download Color Profile") );
    d->colorSpaceSelector->bnDownloadProfile->setEnabled( true );
    d->colorSpaceSelector->bnDownloadProfile->hide();

    d->colorSpaceSelector->bnUploadProfile->setIcon(themedIcon("arrow-up"));
    d->colorSpaceSelector->bnUploadProfile->setToolTip( i18n("Share Color Profile") );
    d->colorSpaceSelector->bnUploadProfile->setEnabled( false );
    d->colorSpaceSelector->bnUploadProfile->hide();

#ifdef GHNS
    d->colorSpaceSelector->bnUploadProfile->show();
    d->colorSpaceSelector->bnDownloadProfile->show();
#endif

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
    connect(d->colorSpaceSelector->lstProfile, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(buttonUpdate()));
    connect(this, SIGNAL(selectionChanged(bool)),
            this, SLOT(fillDescription()));
    connect(this, SIGNAL(selectionChanged(bool)), d->colorSpaceSelector->TongueWidget, SLOT(repaint()));


    connect(d->colorSpaceSelector->bnInstallProfile, SIGNAL(clicked()), this, SLOT(installProfile()));
    connect(d->colorSpaceSelector->bnDownloadProfile, SIGNAL(clicked()), this, SLOT(downloadProfile()));
    connect(d->colorSpaceSelector->bnUploadProfile, SIGNAL(clicked()), this, SLOT(uploadProfile()));
    
    connect(d->colorSpaceSelector->bnOK, SIGNAL(accepted()), this, SLOT(accept()));
    connect(d->colorSpaceSelector->bnOK, SIGNAL(rejected()), this, SLOT(reject()));
    
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
    QStringList profileNames;
    foreach(const KoColorProfile *profile, profileList) {
        profileNames.append(profile->name());
    }
    qSort(profileNames);
    QListWidgetItem *defaultProfile = new QListWidgetItem;
    defaultProfile->setText(csf->defaultProfile()+" "+i18n("(Default)"));
    foreach(QString stringName, profileNames) {
        if (stringName==csf->defaultProfile()) {
            d->colorSpaceSelector->lstProfile->addItem(defaultProfile);
        } else {
            d->colorSpaceSelector->lstProfile->addItem(stringName);
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
    QString profileName = i18n("No Profile Found");
    QString whatIsColorant = i18n("Colorant in d50-adapted xyY.");
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
            d->colorSpaceSelector->lblXYZ_R->setToolTip(whatIsColorant);
            d->colorSpaceSelector->lblXYZ_G->setToolTip(whatIsColorant);
            d->colorSpaceSelector->lblXYZ_B->setToolTip(whatIsColorant);
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
    QString estimatedGamma = i18n("Estimated Gamma: ");
    QString estimatedsRGB = i18n("Estimated Gamma: sRGB, L* or rec709 trc");
    QString whatissRGB = i18n("The Tone Response Curve of this color space is either sRGB, L* or rec709 trc.");
    QString currentModelStr = d->colorSpaceSelector->cmbColorModels->currentItem().id();
    if (currentModelStr == "RGBA") {
        estimatedTRC = currentColorSpace()->profile()->getEstimatedTRC();
        if (estimatedTRC[0] == -1) {
            d->colorSpaceSelector->lbltrc->setToolTip(whatissRGB);
            d->colorSpaceSelector->lbltrc->setText(estimatedsRGB);
        } else {
        d->colorSpaceSelector->lbltrc->setToolTip(estimatedGamma+QString::number(estimatedTRC[0])+","+QString::number(estimatedTRC[1])+","+QString::number(estimatedTRC[2]));
        d->colorSpaceSelector->lbltrc->setText(estimatedGamma+QString::number((estimatedTRC[0]+estimatedTRC[1]+estimatedTRC[2])/3));
        }
    } else if (currentModelStr == "GRAYA") {
            estimatedTRC = currentColorSpace()->profile()->getEstimatedTRC();
        if (estimatedTRC[0] == -1) {
            d->colorSpaceSelector->lbltrc->setToolTip(whatissRGB);
            d->colorSpaceSelector->lbltrc->setText(estimatedsRGB);
        } else {
            d->colorSpaceSelector->lbltrc->setToolTip(estimatedGamma+QString::number(estimatedTRC[0]));
            d->colorSpaceSelector->lbltrc->setText(estimatedGamma+QString::number(estimatedTRC[0]));
        }
    } else if (currentModelStr == "CMYKA") {
        d->colorSpaceSelector->lbltrc->setToolTip(i18n("Estimated Gamma can't be retrieved for CMYK"));
        d->colorSpaceSelector->lbltrc->setText(estimatedGamma+notApplicable);
    } else if (currentModelStr == "XYZA") {
        d->colorSpaceSelector->lbltrc->setToolTip(i18n("We asume that XYZ is linear"));
        d->colorSpaceSelector->lbltrc->setText(estimatedGamma+"1.0");
    } else if (currentModelStr == "LABA") {
        d->colorSpaceSelector->lbltrc->setToolTip(i18n("We asume this is the L * TRC"));
        d->colorSpaceSelector->lbltrc->setText(estimatedGamma+"L*");
    } else if (currentModelStr == "YCbCrA") {
        d->colorSpaceSelector->lbltrc->setToolTip(i18n("Estimated Gamma can't be retrieved for YCRCB"));
        d->colorSpaceSelector->lbltrc->setText(estimatedGamma+notApplicable);
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
    
    if (profileName.contains("-elle-") ) {
        d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
        d->colorSpaceSelector->textProfileDescription->appendHtml("<b>"+i18n("Elle's Notes:")+"</b>");
        
        if(profileName.contains("ACES-")) {
            d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
            d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("Quoting Wikipedia, 'Academy Color Encoding System (ACES) is a color image encoding system proposed by the Academy of Motion Picture Arts and Sciences that will allow for a fully encompassing color accurate workflow, with 'seamless interchange of high quality motion picture images regardless of source''."));
        }
        if(profileName.contains("ACEScg-")) {
            d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
            d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("The ACEScg color space is smaller than the ACES color space, but large enough to contain the 'Rec-2020 gamut and the DCI-P3 gamut', and has chromaticities that fall just barely outside the horseshoe-shaped locus of real colors on the xy chromaticity diagram."));
        }
        if(profileName.contains("ClayRGB-")) {
            d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
            d->colorSpaceSelector->textProfileDescription->appendHtml(i18n("To avoid possible copyright infringement issues, I used 'ClayRGB' (following ArgyllCMS) as the base name for these profiles. As used below, 'Compatible with Adobe RGB 1998' is terminology suggested in the preamble to the AdobeRGB 1998 color space specifications.<br/><br/>The Adobe RGB 1998 color gamut covers a higher percentage of real-world cyans, greens, and yellow-greens than sRGB, but still doesn't include all printable cyans, greens, yellow-greens, especially when printing using today's high-end, wider gamut, ink jet printers. BetaRGB (not included in the profile pack) and Rec.2020 are better matches for the color gamuts of today's wide gamut printers.<br/><br/>The Adobe RGB 1998 color gamut is a reasonable approximation to some of today's high-end wide gamut monitors."));
        }
        if(profileName.contains("AllColorsRGB-")) {
            d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
            d->colorSpaceSelector->textProfileDescription->appendHtml(i18n("This profile's color gamut is roughly the same size and shape as the ACES color space gamut, and like the ACES color space, AllColorsRGB holds all possible real colors. But AllColorsRGB actually has a slightly larger color gamut (to capture some fringe colors that barely qualify as real when viewed by the standard observer) and uses the D50 white point.<br/><br/>Just like the ACES color space, AllColorsRGB holds a high percentage of imaginary colors. See the Completely Painless Programmer's Guide to XYZ, RGB, ICC, xyY, and TRCs for more information about imaginary colors.<br/><br/>I can't think of any particular reason why anyone would want to use this profile for editing, unless you have a burning need to make sure your color space really does hold all possible real colors."));
        }
        if(profileName.contains("CIERGB-")) {
            d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
            d->colorSpaceSelector->textProfileDescription->appendHtml(i18n("This profile is included mostly for its historical significance. It's the color space that was used in the original color matching experiments that led to the creation of the XYZ reference color space.<br/><br/>The ASTM E white point is probably the right E white point to use when making the CIERGB color space profile. It's not clear to me what the correct CIERGB primaries really are. Lindbloom gives one set. The LCMS version 1 tutorial gives a different set. I asked a friend to ask a bonified expert in the field, who said the real primaries should be calculated from the spectral wavelengths, so I did."));
        }
        if(profileName.contains("IdentityRGB-")) {
            d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
            d->colorSpaceSelector->textProfileDescription->appendHtml(i18n("The IdentityRGB working space is included in the profile pack because it's a mathematically obvious way to include all possible visible colors, though it has a higher percentage of imaginary colors than the ACES and AllColorsRGB color spaces. I can't think of any reason why you'd ever want to actually edit images in the IdentityRGB working space."));
        }
        if(profileName.contains("LargeRGB-")) {
            d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
            d->colorSpaceSelector->textProfileDescription->appendHtml(i18n("To avoid possible copyright infringement issues, I used 'LargeRGB' (following RawTherapee) as the base name for these profiles.<br/><br/>Kodak designed the RIMM/ROMM (ProPhotoRGB) color gamut to include all printable and most real world colors. It includes some imaginary colors and excludes some of the real world blues and violet blues that can be captured by digital cameras. It also excludes some very saturated 'camera-captured' yellows as interpreted by some (and probably many) camera matrix input profiles.<br/><br/>The ProPhotoRGB primaries are hard-coded into Adobe products such as Lightroom and the Dng-DCP camera 'profiles'. However, other than being large enough to hold a lot of colors, ProPhotoRGB has no particular merit as an RGB working space. Personally and for most editing purposes, I recommend BetaRGB, Rec2020, or the ACEScg profiles ProPhotoRGB."));
        }
        if(profileName.contains("Rec2020-")) {
            d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
            d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("Rec.2020 is the up-and-coming replacement for the thoroughly outdated sRGB color space. As of June 2015, very few (if any) display devices (and certainly no affordable display devices) can display all of Rec.2020. However, display technology is closing in on Rec.2020, movies are already being made for Rec.2020, and various cameras offer support for Rec.2020. And in the digital darkroom Rec.2020 is much more suitable as a general RGB working space than the exceedingly small sRGB color space."));
        }
        if(profileName.contains("sRGB-")) {
            d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
            d->colorSpaceSelector->textProfileDescription->appendHtml(i18n("Hewlett-Packard and Microsoft designed sRGB to match the color gamut of consumer-grade CRTs from the 1990s. sRGB is the standard color space for the world wide web and is still the best choice for exporting images to the internet.<br/><br/>The sRGB color gamut was a good match to calibrated decent quality CRTs. But sRGB is not a good match to many consumer-grade LCD monitors, which often can't display the more saturated sRGB blues and magentas (the good news: as technology progresses, wider gamuts are trickling down to consumer grade monitors).<br/><br/>Printer color gamuts can easily exceed the sRGB color gamut in cyans, greens, and yellow-greens. Colors from interpolated camera raw files also often exceed the sRGB color gamut.<br/><br/>As a very relevant aside, using perceptual intent when converting to sRGB does not magically makes otherwise out of gamut colors fit inside the sRGB color gamut! The standard sRGB color space (along with all the other the RGB profiles provided in my profile pack) is a matrix profile, and matrix profiles don't have perceptual intent tables."));
        }
        if(profileName.contains("WideRGB-")) {
            d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
            d->colorSpaceSelector->textProfileDescription->appendHtml(i18n("To avoid possible copyright infringement issues, I used 'WideRGB' as the base name for these profiles.<br/><br/>WideGamutRGB was designed by Adobe to be a wide gamut color space that uses spectral colors as its primaries. Pascale's primary values produce a profile that matches old V2 Widegamut profiles from Adobe and Canon. It's an interesting color space, but shortly after its introduction, Adobe switched their emphasis to the ProPhotoRGB color space."));
        }
        if(profileName.contains("Gray-")) {
            d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
            d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("These profiles are for use with RGB images that have been converted to monotone gray (black and white). The main reason to convert from RGB to Gray is to save the file space needed to encode the image. Google places a premium on fast-loading web pages, and images are one of the slower-loading elements of a web page. So converting black and white images to Grayscale images does save some kilobytes. For grayscale images uploaded to the internet, convert the image to the V2 Gray profile with the sRGB TRC."));
        }
        if(profileName.contains("-g10")) {
        d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
        d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("The profiles that end in '-g10.icc' are linear gamma (gamma=1.0, 'linear light', etc) profiles and should only be used when editing at high bit depths (16-bit floating point, 16-bit integer, 32-bit floating point, 32-bit integer). Many editing operations produce better results in linear gamma color spaces."));
        }
        if(profileName.contains("-labl")) {
        d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
        d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("The profiles that end in '-labl.icc' have perceptually uniform TRCs. A few editing operations really should be done on perceptually uniform RGB. Make sure you use the V4 versions for editing high bit depth images."));
        }
        if(profileName.contains("-srgbtrc") || profileName.contains("-g22") || profileName.contains("-g18") || profileName.contains("-bt709")) {
        d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
        d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("The profiles that end in '-srgbtrc.icc', '-g22.icc', and '-bt709.icc' have approximately but not exactly perceptually uniform TRCs. ProPhotoRGB's gamma=1.8 TRC is not quite as close to being perceptually uniform."));
        }
        if(d->colorSpaceSelector->cmbColorDepth->currentItem().id()=="U8") {
        d->colorSpaceSelector->textProfileDescription->appendHtml("<br/>");
        d->colorSpaceSelector->textProfileDescription->appendPlainText(i18n("When editing 8-bit images, you should use a profile with a small color gamut and an approximately or exactly uniform TRC. Of the profiles supplied in my profile pack, only the sRGB and AdobeRGB1998 (ClayRGB) color spaces are small enough for 8-bit editing. Even with the AdobeRGB1998 color space you need to be careful to not cause posterization. And of course you can't use the linear gamma versions of these profiles for 8-bit editing."));
        }
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

void KisAdvancedColorSpaceSelector::downloadProfile()
{
#ifdef GHNS
    KNS3::DownloadDialog dialog( "kritaiccprofiles.knsrc", this);
    dialog.exec();
    KoColorSpaceEngine *iccEngine = KoColorSpaceEngineRegistry::instance()->get("icc");
    Q_ASSERT(iccEngine);
    foreach (const KNS3::Entry& e, dialog.changedEntries()) {
        foreach(const QString &file, e.installedFiles()) {
            QFileInfo fi(file);
            iccEngine->addProfile( fi.absolutePath()+'/'+fi.fileName());
        }
        foreach(const QString &file, e.uninstalledFiles()) {
            QFileInfo fi(file);
            iccEngine->removeProfile( fi.absolutePath()+'/'+fi.fileName());
        }
    }
    fillCmbProfiles();
#endif
}

void KisAdvancedColorSpaceSelector::uploadProfile()
{
#ifdef GHNS
    KNS3::UploadDialog dialog("kritaiccprofiles.knsrc", this);
    const KoColorProfile *  profile = KoColorSpaceRegistry::instance()->profileByName(d->colorSpaceSelector->cmbProfile->currentText());
    if(!profile)  return;
    dialog.setUploadFile(KUrl::fromLocalFile(profile->fileName()));
    dialog.setUploadName(profile->name());
    dialog.exec();
#endif
}

void KisAdvancedColorSpaceSelector::buttonUpdate()
{
    QString check = "";
   if (d->colorSpaceSelector->lstProfile->currentItem()) {
        check = d->colorSpaceSelector->lstProfile->currentItem()->text();
    } else {
        check = d->colorSpaceSelector->lstProfile->item(0)->text();
    }
   const KoColorProfile *  profile = KoColorSpaceRegistry::instance()->profileByName(check);
   if(!profile)  return;

   QFileInfo fileInfo(profile->fileName());
   if(fileInfo.isWritable()) {
       d->colorSpaceSelector->bnUploadProfile->setEnabled( true );
       return;
   }
   d->colorSpaceSelector->bnUploadProfile->setEnabled( false );
}

#include "kis_advanced_color_space_selector.moc"
