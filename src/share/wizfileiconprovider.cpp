#include "wizfileiconprovider.h"
#include <QPixmapCache>
#include <QDir>

#if defined(Q_WS_WIN)
#  define _WIN32_IE 0x0500
#  include <qt_windows.h>
#  include <commctrl.h>
#  include <objbase.h>

#ifndef SHGFI_ADDOVERLAYS
#  define SHGFI_ADDOVERLAYS 0x000000020
#  define SHGFI_OVERLAYINDEX 0x000000040
#endif

#endif

CWizFileIconProvider::CWizFileIconProvider()
{
}

QIcon CWizFileIconProvider::icon(const QString& strFileName) const
{
    QIcon retIcon = QFileIconProvider::icon(QFileInfo(strFileName));
    if (!retIcon.isNull())
        return retIcon;
    //
#if defined(Q_WS_WIN)
    for (int i = 0; i < 2; i++)
    {
        long flags = SHGFI_ICON|SHGFI_SYSICONINDEX|SHGFI_ADDOVERLAYS|SHGFI_OVERLAYINDEX | SHGFI_USEFILEATTRIBUTES;
        flags |= (i == 0 ? SHGFI_SMALLICON : SHGFI_LARGEICON);
        //
        SHFILEINFO info;
        memset(&info, 0, sizeof(SHFILEINFO));
        unsigned long val = SHGetFileInfo((const wchar_t *)QDir::toNativeSeparators(strFileName).utf16(), 0, &info,
                            sizeof(SHFILEINFO), flags);
        //
        if (val && info.hIcon)
        {
            QPixmap pixmap = QPixmap::fromWinHICON(info.hIcon);
            if (!pixmap.isNull())
            {
                retIcon.addPixmap(pixmap);
            }
            else
            {
              qWarning("QFileIconProviderPrivate::getWinIcon() no small icon found");
            }
            //
            DestroyIcon(info.hIcon);
        }
    }
#endif
    //
    return retIcon;
}
QString CWizFileIconProvider::type(const QString& strFileName) const
{
#if defined(Q_WS_WIN)
    long flags = SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES;
    //
    SHFILEINFO info;
    memset(&info, 0, sizeof(SHFILEINFO));
    SHGetFileInfo((const wchar_t *)QDir::toNativeSeparators(strFileName).utf16(), 0, &info,
                        sizeof(SHFILEINFO), flags);
    //
    return QString::fromUtf16((const ushort*)info.szTypeName);
#else
    QString retType = QFileIconProvider::type(QFileInfo(strFileName));
    return retType;
#endif
}


