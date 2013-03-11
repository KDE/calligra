#ifndef FAKE_KFILE_H
#define FAKE_KFILE_H

#include <QObject>

class KFile
{
    Q_GADGET
    Q_FLAGS(Modes)
public:
    enum Mode {
        File         = 1,
        Directory    = 2,
        Files        = 4,
        ExistingOnly = 8,
        LocalOnly    = 16,
        ModeMax      = 65536
    };
    Q_DECLARE_FLAGS(Modes, Mode)

    enum FileView {
        Default         = 0,
        Simple          = 1,
        Detail          = 2,
        SeparateDirs    = 4,
        PreviewContents = 8,
        PreviewInfo     = 16,
        Tree            = 32,
        DetailTree      = 64,
        FileViewMax     = 65536
    };

    enum SelectionMode {
        Single      = 1,
        Multi       = 2,
        Extended    = 4,
        NoSelection = 8
    };

#if 0
    static bool isSortByName( const QDir::SortFlags& sort );
    static bool isSortBySize( const QDir::SortFlags& sort );
    static bool isSortByDate( const QDir::SortFlags& sort ); 
    static bool isSortByType( const QDir::SortFlags& sort );
    static bool isSortDirsFirst( const QDir::SortFlags& sort );
    static bool isSortCaseInsensitive( const QDir::SortFlags& sort );

    static bool isDefaultView( const FileView& view );
    static bool isSimpleView( const FileView& view );
    static bool isDetailView( const FileView& view ); 
    static bool isSeparateDirs( const FileView& view );
    static bool isPreviewContents( const FileView& view ); 
    static bool isPreviewInfo( const FileView& view );
    static bool isTreeView( const FileView& view );
    static bool isDetailTreeView( const FileView& view );
#endif

private:
    KFile() {}
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KFile::Modes)

#endif
 
