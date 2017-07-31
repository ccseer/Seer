#ifndef _EXTENSIONS_H_
#define _EXTENSIONS_H_

#include <QStringList>
#include "qmap.h"
#include "types.h"

class CExtensionList : public QStringList {
public:
    CExtensionList() : QStringList()
    {
    }
    CExtensionList(const QStringList& s)
    {
        foreach (auto i, s) {
            append(i);
        }
    }

    QString forFilter();
    QStringList forDirFilter();
    QString forRegExp();

    QString forDisplay(const QString& space = "   ") const;
};

class CExtensions {
public:
    CExtensions();

    DisplayType GetType(const QString& path);
    FileSubType GetSubType(const QString& path);

    const QMap<DisplayType, CExtensionList>& GetAll() const
    {
        return m_all;
    }
    const QMap<FileSubType, CExtensionList>& GetAllSub() const
    {
        return m_all_sub;
    }

protected:
    QMap<FileSubType, CExtensionList> m_all_sub;
    QMap<DisplayType, CExtensionList> m_all;
};

#endif
