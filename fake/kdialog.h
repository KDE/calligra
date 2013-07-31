#ifndef FAKE_KDIALOG_H
#define FAKE_KDIALOG_H

#include <QDialog>
#include <QMenu>
#include <kio/kfileitem.h>
#include <kicon.h>
#include <kguiitem.h>
#include <kpushbutton.h>

#include "kofake_export.h"

class KOFAKE_EXPORT KDialog : public QDialog
{
    Q_OBJECT
    Q_ENUMS(ButtonCode)
public:

    enum ButtonCode
    {
      None    = 0x00000000,
      Help    = 0x00000001,
      Default = 0x00000002,
      Ok      = 0x00000004,
      Apply   = 0x00000008,
      Try     = 0x00000010,
      Cancel  = 0x00000020,
      Close   = 0x00000040,
      No      = 0x00000080,
      Yes     = 0x00000100,
      Reset   = 0x00000200,
      Details = 0x00000400,
      User1   = 0x00001000,
      User2   = 0x00002000,
      User3   = 0x00004000,
      NoDefault = 0x00008000
    };
    Q_DECLARE_FLAGS(ButtonCodes, ButtonCode)

    enum ButtonPopupMode { InstantPopup = 0, DelayedPopup = 1 };
    Q_DECLARE_FLAGS(ButtonPopupModes, ButtonPopupMode)

    KDialog(QWidget *p = 0, Qt::WFlags flags = 0) : QDialog(p) {}

    void setButtons(ButtonCodes buttonMask) {}
    void setButtons(int) {}
    void setButtonsOrientation( Qt::Orientation orientation ) {}
    void setEscapeButton( ButtonCode id ) {}
    ButtonCode defaultButton() const { return Ok; }
    void setDefaultButton( ButtonCode id ) {}
    void showButtonSeparator( bool state ) {}
    void showButton( ButtonCode id, bool state ) {}
    void setButtonText( ButtonCode id, const QString &text ) {}
    QString buttonText( ButtonCode id ) const { return QString(); }
    void setButtonIcon( ButtonCode id, const KIcon &icon ) {}
    KIcon buttonIcon( ButtonCode id ) const { return KIcon(); }
    void setButtonToolTip( ButtonCode id, const QString &text ) {}
    QString buttonToolTip( ButtonCode id ) const { return QString(); }
    void setButtonWhatsThis( ButtonCode id, const QString &text ) {}
    QString buttonWhatsThis( ButtonCode id ) const { return QString(); }
    void setButtonGuiItem( ButtonCode id, const KGuiItem &item ) {}
    void setButtonMenu( ButtonCode id, QMenu *menu, ButtonPopupMode popupmode=InstantPopup) {}
    void setButtonFocus( ButtonCode id ) {}
    void setInitialSize( const QSize &size ) { resize(size); }

    void incrementInitialSize( const QSize &size ) {}
    void restoreDialogSize( const KConfigGroup& config ) {}
    void saveDialogSize( KConfigGroup& config, KConfigGroup::WriteConfigFlags options = KConfigGroup::Normal ) const {}
    QString helpLinkText() const { return QString(); }
    bool isButtonEnabled( ButtonCode id ) const;
    KPushButton* button( ButtonCode id ) const { return 0; }
    static int marginHint() { return 2; }
    static int spacingHint() { return 2; }
    static int groupSpacingHint() { return 2; }

#if 0
    enum CaptionFlag
    {
      NoCaptionFlags = 0,
      AppNameCaption = 1,
      ModifiedCaption = 2,
      HIGCompliantCaption = AppNameCaption
    };
    Q_DECLARE_FLAGS(CaptionFlags, CaptionFlag)

    static QString makeStandardCaption( const QString &userCaption,
                                      QWidget* window = 0,
                                      CaptionFlags flags = HIGCompliantCaption );
    static void resizeLayout( QWidget *widget, int margin, int spacing ) {}
    static void resizeLayout( QLayout *lay, int margin, int spacing ) {}
    static void centerOnScreen( QWidget *widget, int screen = -1 ) {}
#endif
    static bool avoidArea( QWidget *widget, const QRect& area, int screen = -1 ) { return true; }

    void setMainWidget( QWidget *widget ) {}
    QWidget *mainWidget() { return 0; }

#if 0
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
    static void setAllowEmbeddingInGraphicsView( bool allowEmbedding );
#endif

public Q_SLOTS:
    virtual void setCaption( const QString &caption ) { setWindowTitle(caption); }
    virtual void setCaption( const QString &caption, bool modified ) { setCaption(caption); }
    virtual void setPlainCaption( const QString &caption ) { setCaption(caption); }

    void enableButton( ButtonCode id, bool state ) {}
    void enableButtonOk( bool state ) {}
    void enableButtonApply( bool state ) {}
    void enableButtonCancel( bool state ) {}
    void enableLinkedHelp( bool state ) {}

    void setHelpLinkText( const QString &text ) {}
    void setHelp( const QString &anchor, const QString &appname = QString() ) {}
    bool isDetailsWidgetVisible() const { return false; }
    void setDetailsWidgetVisible( bool visible ) {}
    void setDetailsWidget( QWidget *detailsWidget ) {}
    //void delayedDestruct() { deleteLater(); }

Q_SIGNALS:
#if 0
    void layoutHintChanged();
    void helpClicked();
    void defaultClicked();
    void resetClicked();
    void user3Clicked();
    void user2Clicked();
    void user1Clicked();
    void applyClicked();
    void tryClicked();
    void okClicked();
    void yesClicked();
    void noClicked();
    void cancelClicked();
    void closeClicked();
#endif

    void buttonClicked( KDialog::ButtonCode button);

#if 0
    void hidden();
    void finished();
    void aboutToShowDetails();
#endif

protected Q_SLOTS:
    virtual void slotButtonClicked(int button) { emit buttonClicked( (KDialog::ButtonCode) button ); }

#if 0
    void updateGeometry();
#endif

};

#endif
