
#include "settingsdlg.h"
#include "settingsdlg.moc"

#include <kapp.h>

#include <qlayout.h>

#include "kproptext.h"
#include "kpropradio.h"
#include "kpropgroup.h"
#include "kpropfont.h"

SettingsDlg::SettingsDlg()
:KPropDlg(KPropDlg::TREE, KPropDlg::OK | KPropDlg::CANCEL | KPropDlg::APPLY,
          i18n("KoHTML: Preferences"), 0, "settingsDlg", true)
{
  miscFrame = addPage(-1, i18n("General"));
  setupMiscTab();
  
  fontsFrame = addPage(-1, i18n("Fonts"));
  setupFontsTab();

  emit getConfig();
  
  showPage(miscFrame);
}

SettingsDlg::~SettingsDlg()
{
}

void SettingsDlg::setupMiscTab()
{
  QVBoxLayout *layout = new QVBoxLayout(miscFrame, 4);
  
  KPropRadio *browserStart = new KPropRadio(miscFrame, i18n("Browser starts with"), "BrowserStart", "Personal Settings");
  browserStart->addRadio(i18n("Blank page"), TRUE);
  browserStart->addRadio(i18n("Home page"), FALSE);
  connectConfig(browserStart);
  layout->addWidget(browserStart);  
  
  KPropGroup *groupHomePage = new KPropGroup(miscFrame, i18n("Home Page"));
  
  layout->addWidget(groupHomePage);
  
  QLabel *l = new QLabel(i18n("Clicking the Home button brings you to this page."), groupHomePage);
  l->adjustSize();
  l->setMinimumSize(l->sizeHint());
  groupHomePage->addWidget(l);
  
  KPropText *homePage = new KPropText(groupHomePage, i18n("Location:"), 70, "HomePage", "Personal Settings");
  homePage->adjustSize();
  homePage->setMinimumSize(homePage->sizeHint());  
  connectConfig(homePage);
  groupHomePage->addWidget(homePage);
  
}

void SettingsDlg::setupFontsTab()
{
  QVBoxLayout *layout = new QVBoxLayout(fontsFrame, 4);
  
  KPropRadio *fontSize = new KPropRadio(fontsFrame, i18n("Font Size"), "FontSize", "Fonts");
  fontSize->addRadio(i18n("Small"), FALSE);
  fontSize->addRadio(i18n("Medium"), TRUE);
  fontSize->addRadio(i18n("Large"), FALSE);
  connectConfig(fontSize);
  layout->addWidget(fontSize);  
  
  KPropFont *standardFont = new KPropFont(fontsFrame, "KOffice", i18n("Standard Font"), 50, QFont("helvetica"), "StandardFont", "Fonts");
  connectConfig(standardFont);
  layout->addWidget(standardFont);  
  
  KPropFont *fixedFont = new KPropFont(fontsFrame, "KoHTML", i18n("Fixed Font"), 50, QFont("courier"), "FixedFont", "Fonts");
  connectConfig(fixedFont);
  layout->addWidget(fixedFont);  
}