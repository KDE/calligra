#ifndef __koffice_filter_h__
#define __koffice_filter_h__

#include <qobject.h>

class QDomDocument;
class QCString;
class KoStore;
class KoDocument;

/**
 * This is an abstract base class for filters. Please overload the
 * method you "like best" and adapt the .desktop-file.
 * See koffice/filters/HOWTO for further information!
 *
 * @short Abstract base class for filters
 */
class KoFilter : public QObject {

    Q_OBJECT

public:
    virtual ~KoFilter() {}

    /**
     * This method takes a file as input and outputs a file. (This is the
     * "traditional way" of filtering. It's not very fast (because of all
     * the tmp files, but it's easy and you don't have to know the internals
     * of the app.
     * @param fileIn 	The name of the file to filter (input file)
     * @param fileOut 	Save the converted stuff to that file (output file)
     * @param from 	Mimetype of the input file
     * @param to 	Mimetype of the output file
     * @param config   	A String which can be used to pass configure information
     * @return 		If the method returns true the filtering was successful
     */
    virtual const bool filter(const QCString &fileIn, const QCString &fileOut,
                              const QCString &from, const QCString &to,
                              const QString &config=QString::null);

    /**
     * This one takes a file as input and writes in a QDomDocument. It's
     * slightly faster then the first one, but you need a part which uses
     * QDom (e.g. KSpread, KIS,...- but NOT KWord, KPresenter!).
     * This one can only be used as import filter!!! (hence the I_ :)
     * @param file 	The filename of the input file
     * @param from 	Mimetype of the input file
     * @param to 	Mimetype of the output file
     * @param config   	A String which can be used to pass configure information
     * @return 		A ptr to the QDomDocument you created.
     */
    virtual const QDomDocument *I_filter(const QCString &file, const QCString &from,
					 const QCString &to, const QString &config=QString::null);
    
    /**
     * This is the most hacky method(tm) available. Here you have direct
     * access to the (empty) KoDocument. Overload this method if you want
     * to IMPORT data.
     * @param file 	The filename of the input file
     * @param document 	The KoDocument of the part
     * @param from 	Mimetype of the input file
     * @param to 	Mimetype of the output file
     * @param config   	A String which can be used to pass configure information
     * @return 		If the method returns true the filtering was successful
     */
    virtual const bool I_filter(const QCString &file, KoDocument *document,
				const QCString &from, const QCString &to,
				const QString &config=QString::null);

    /**
     * This is another very nasty method. Here you have direct access to the
     * "full" KoDocument (read only, if you don't cast away the constness :)
     * Overload this method if you want to EXPORT data.
     * @param file 	The filename of the input file
     * @param document 	The KoDocument of the part
     * @param from 	Mimetype of the input file
     * @param to 	Mimetype of the output file
     * @param config   	A String which can be used to pass configure information
     * @return 		If the method returns true the filtering was successful
     */
    virtual const bool E_filter(const QCString &file, const KoDocument * const document,
				const QCString &from, const QCString &to,
				const QString &config=QString::null);

protected:
    KoFilter(KoFilter *parent, QString name);   // only for classes inheriting this one

private:
    KoFilter();                                 // Meyers says this is nice :)
    KoFilter &operator=(const KoFilter &);
};
#endif
