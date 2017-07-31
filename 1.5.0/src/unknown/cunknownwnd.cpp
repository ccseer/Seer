#include "cunknownwnd.h"
#include "cfileproperty.h"
#include "ctableviewwithcopykey.h"

#include <QClipboard>
#include "qcoreapplication.h"
#include "qevent.h"

CUnknownWnd::CUnknownWnd(QWidget *parent)
    : CAbstractWnd(ManagerPtr(new CAbstractManager), parent),
      m_pview(new CTableViewWithCopyKey)
{
    SetWnd(m_pview);

    m_pview->setModel(g_pfile_property->GetModel());
}

//同步
void CUnknownWnd::LoadHelper()
{
    emit SigLoaded();
}

QSize CUnknownWnd::GetSize()
{
    const auto fm = m_pview->fontMetrics();
    return QSize(fm.width("Last modified time")
                     // sha1
                     + fm.width("3dcee692ffd3d23ceaf104e3468e7dfb45e6eb8fXXXXX")
                     // scrollbar    width: 4.5ex
                     + fm.xHeight() * 4.5,
                 //
                 fm.height() * 12 + fm.xHeight() * 12);
}
