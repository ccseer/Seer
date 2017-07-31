#ifndef CPDFWND_H
#define CPDFWND_H

#ifdef PDF
#include "cabstractwnd.h"
#include "pdfview.h"

class QLabel;
class QToolButton;

class CPdfWnd : public CAbstractWnd {
    Q_OBJECT
public:
    CPdfWnd(QWidget* parent = 0);
    ~CPdfWnd();

    virtual QSize GetSize();

    virtual DisplayType Type() const
    {
        return Pdf;
    }

protected:
    virtual void InitControlWnd(QHBoxLayout* lay);

    virtual FilePropertyData LoadFileInfoHelper();

private:
    void UpdateRecents();
    void RestoreFromRecents();

    PdfView m_view;

    QLabel* m_plabel_page;
    QToolButton* m_pbtn_width;

    // CAbstractWnd interface
protected slots:
    virtual void LoadHelper();

};
#endif
#endif  // CPDFWND_H
