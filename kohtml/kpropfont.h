// kpropfont.h

#if !defined KPROPFONT_H
#define KPROPFONT_H

#include <qlabel.h>
#include <qpushbutton.h>
#include <kfontdialog.h>
#include <kconfig.h>

/** A font display with a font dialog popup button.
  * The widget is resized when the button font changes.
  * 
  */
class KPropFont : public QLabel
{
	Q_OBJECT
	
public:
	/** Constructor. 'text' is the text of the label.
	  * 'perc' is the percentage of space reserved for the button,
	  * 'testtext' is the text in the label,
	  * 'buttontext' is the text on the button,
	  */
	KPropFont( QWidget *parent, const char *testtext, const char *buttontext, int perc,
			QFont mydefault, const char *key=0, const char *group=0, KConfig *config=0, const char *name=0 );

	/** Destructor. */
	virtual ~KPropFont();
	
	/** Set the font of the test label text. */
	void setLabelFont( QFont font );

	/** Returns the font of the test label text. */
	QFont getLabelFont();

public slots:
	/** Write the config key value. . */
	virtual void setConfig();

	/** Read the config key value into the widget. 
	  * . 
	  */
	virtual void getConfig();

	/** set the group to modify */
	void setGroup( const char *group );

	/** Returns the group to be modified */
	const char *getGroup();

	/** set the key to modify */
	void setKey( const char * );

	/** returns the Key to be modified */
	const char *getKey();

	/** Set the KConfig object to operate upon. */
	void setKConfig( KConfig * );

	/** Returns the current KConfig object */
	KConfig *getKConfig();

protected:
	/** Handles resize events for the line edit widget. */
	virtual void resizeEvent( QResizeEvent *rev );

	/** This method is called after a font change.
	  * It will call sizeUpdate() to resize the widget to contain
	  * the new font.
	  */
	virtual void fontChange( const QFont &oldfont );

	/** Sets the minimum size to fit the font. */
	void sizeUpdate();

	QLabel *label;
	QPushButton *button;
	int percentage;
	KConfig *ConfigObject;
	QString Group, Key;
	QFont defaultFont;

protected slots:
	/** popup the font dialog and put the font in the Label */
	void getFontDialog();
	
};

#endif //KPROPFONT_H

