#include "cclipboardhelper.h"
#include "stable.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include <iostream>

CClipboardHelper::CClipboardHelper(QObject *parent) : QObject(parent)
{
}

void CClipboardHelper::Backup()
{
    m_old_clipboard_data.clear();
    if (auto s = qApp->clipboard()->mimeData()) {
        foreach (auto i, s->formats()) {
            m_old_clipboard_data.append(qMakePair(i, s->data(i)));
            std::cout << "backup" << std::endl;
            elprint;
        }
    }
    qApp->clipboard()->clear();
}

void CClipboardHelper::Restore()
{
    qApp->clipboard()->clear();
    QMimeData *m = new QMimeData;
    foreach (const FmtBytes &i, m_old_clipboard_data) {
        m->setData(i.first, i.second);
        std::cout << "Restore" << std::endl;
        elprint;
    }
    qApp->clipboard()->setMimeData(m);
}
