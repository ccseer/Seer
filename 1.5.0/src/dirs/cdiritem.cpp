#include "cdiritem.h"

CDirItem::CDirItem(const QString &text) : QStandardItem(text), m_state(Unload)
{
}

CDirItem::CDirItem(const QString &text, const QString &path, ItemStatus st)
    : CDirItem(text)
{
    SetPath(path);
    m_state = st;
}

CDirItem::ItemStatus CDirItem::GetStatue() const
{
    return m_state;
}

void CDirItem::SetStatus(ItemStatus s)
{
    m_state = s;
}

const QString &CDirItem::GetPath() const
{
    return m_path;
}

void CDirItem::SetPath(const QString &p)
{
    m_path = p;
}
