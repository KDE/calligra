#ifndef FAKE_KURLREQUESTER_H
#define FAKE_KURLREQUESTER_H

#include <QWidget>

#include <kfile.h>
#include <kpushbutton.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kurlcompletion.h>
//#include <keditlistwidget.h>

class KUrlRequester : public QWidget
{
public:
    KUrlRequester( QWidget *parent=0 ) : QWidget(parent) {}
    KUrlRequester( const KUrl& url, QWidget *parent=0) : QWidget(parent) {}
    KUrlRequester( QWidget *editWidget, QWidget *parent) : QWidget(parent) {}
    virtual ~KUrlRequester() {}
    KUrl url() const { return KUrl(); }
    KUrl startDir() const { return KUrl(); }
    QString text() const { return QString(); }
    void setMode( KFile::Modes m ) {}
    KFile::Modes mode() const  { return KFile::File; }
    void setFilter( const QString& filter ) {}
    QString filter() const { return QString(); }
    virtual KFileDialog * fileDialog() const { return 0; }
    KLineEdit * lineEdit() const { return 0; }
    KComboBox * comboBox() const { return 0; }
    KPushButton * button() const { return 0; }
    KUrlCompletion *completionObject() const { return 0; }
    //const KEditListWidget::CustomEditor &customEditor();
    QString clickMessage() const { return QString(); }
    void setClickMessage(const QString& msg) {}
    //Qt::WindowModality fileDialogModality() const;
    //void setFileDialogModality(Qt::WindowModality modality);
//public Q_SLOTS:
    void setUrl( const KUrl& url ) {}
    void setStartDir( const KUrl& startDir ) {}
    void setText(const QString& text) {}
    void clear() {}
#if 0
Q_SIGNALS:
    void textChanged( const QString& );
    void returnPressed();
    void returnPressed( const QString& );
    void openFileDialog( KUrlRequester * );
    void urlSelected( const KUrl& );
protected:
    virtual void changeEvent (QEvent *e);
    bool eventFilter( QObject *obj, QEvent *ev );
#endif
};

class KUrlComboRequester : public KUrlRequester
{
public:
    KUrlComboRequester(QWidget *parent = 0) : KUrlRequester(parent) {}
};

#endif
