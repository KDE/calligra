// kpropspin.h

#if !defined KPROPSPIN_H
#define KPROPSPIN_H

#include <qlabel.h>
#include <qspinbox.h>
#include <kconfig.h>


/** This class provides a label with a spinbox.
  * You can get a pointer to the spinbox to configure it,
  * and the widget wil resize on font change.
  */
class KPropSpin : public QLabel
{
	Q_OBJECT
	
public:
	/** Constructor. 'text' contains the label text, 'perc' sets the 
	  * percentage of space reserved for the spinbox.
	  **/
	KPropSpin( QWidget *parent, const char *text, int perc, const char *key=0, const char *group=0, KConfig *config=0, const char *name=0 );

	/** Destructor. Deletes the spinbox. */
	virtual ~KPropSpin();
	
	/** Returns a pointer to the spinbox so you can configure it. */
	QSpinBox *getSpinBox();
	
	/** Set the value of the spinbox. Reimplement 
	  * if you want to handle other than numeric values.
	  */
	virtual void setContents( int value );

	/** Set the text contents of the spinbox. Used by getConfig()
	  * to set the value from the cofnig file.
	  */
	virtual void setContents( const char *s = 0 );

	/** Returns the current contents of the spinbox. Used
	  * by setConfig() to write to the config file.
	  */
	const char *getContents();

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


public slots:
	/** Write the config key value. . */
	virtual void setConfig();

	/** Read the config key value into the widget. 
	  * . 
	  */
	virtual void getConfig();

protected:
	/** Handles resizes for the spinbox. */
	virtual void resizeEvent( QResizeEvent *rev );

	/** This method is called after a font change.
	  * It will call sizeUpdate() to resize the widget to contain
	  * the new font.
	  */
	virtual void fontChange( const QFont &oldfont );

	/** Sets the minimum size to fit the font. */
	void sizeUpdate();

	QSpinBox *spinBox;
	int percentage;
	KConfig *ConfigObject;
	QString Group, Key;
};

#endif //KPROPSPIN_H

