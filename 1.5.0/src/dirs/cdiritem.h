#ifndef CDIRITEM_H
#define CDIRITEM_H

#include <QStandardItem>

class CDirItem : public QStandardItem {
public:
    enum ItemStatus { Unload, Loading, Loaded };

    CDirItem(const QString& text);
    CDirItem(const QString& text, const QString& path, ItemStatus st = Unload);

    void SetPath(const QString& p);
    const QString& GetPath() const;

    void SetStatus(ItemStatus s);
    ItemStatus GetStatue() const;

private:
    QString m_path;
    ItemStatus m_state;
};

#endif  // CDIRITEM_H
