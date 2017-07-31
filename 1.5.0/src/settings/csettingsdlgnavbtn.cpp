#include "csettingsdlgnavbtn.h"
#include "ccomponent.h"
#include "cglobaldata.h"
#include "cthememanager.h"
#include "stable.h"
#include "ui_csettingsdlgnavbtn.h"

#include <qevent.h>

CSettingsDlgNavBtn::CSettingsDlgNavBtn(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::CSettingsDlgNavBtn),
      m_checked(false),
      m_is_title(false),
      m_wnd_pressed(false)
{
    ui->setupUi(this);
}

CSettingsDlgNavBtn::~CSettingsDlgNavBtn()
{
    delete ui;
}

void CSettingsDlgNavBtn::SetTitle(bool a)
{
    m_is_title = a;
    ui->label_text->setObjectName("CSettingsDlgNavBtn_TitleText");
    ui->label_icon->setObjectName("CSettingsDlgNavBtn_TitleIcon");
    this->setObjectName("CSettingsDlgNavBtn_Title");
}

void CSettingsDlgNavBtn::Init(const QVariantList &res)
{
    ui->Layout->setContentsMargins(0, 0, 0, 0);
    ui->horizontalLayout->setSpacing(0);

    if (!m_is_title) {
        ui->label_icon->setText(
            fa::c(static_cast<fa::icon>(res.last().toInt())));
        ui->label_text->setText(res.at(0).toString());

        SetChecked(m_checked);
    }
    else {
        ui->label_text->setText(res.at(0).toString());
    }
}

void CSettingsDlgNavBtn::SetChecked(bool checked)
{
    m_checked = checked;

    ui->widget->setProperty("checked", m_checked);
    ui->widget->setStyleSheet(ui->widget->styleSheet());
}

void CSettingsDlgNavBtn::mousePressEvent(QMouseEvent *e)
{
    if (!m_checked && !m_is_title) {
        SetChecked(true);
        emit Clicked();
    }

    if (e->button() == Qt::LeftButton && contentsRect().contains(e->pos())) {
        m_wnd_move    = e->globalPos();
        m_wnd_pressed = true;
    }

    QWidget::mousePressEvent(e);
}

void CSettingsDlgNavBtn::mouseReleaseEvent(QMouseEvent *e)
{
    m_wnd_pressed = false;
    QWidget::mouseReleaseEvent(e);
}

void CSettingsDlgNavBtn::mouseMoveEvent(QMouseEvent *e)
{
    if (m_wnd_pressed) {
        emit Move(e->globalPos() - m_wnd_move);
        m_wnd_move = e->globalPos();
    }
    QWidget::mouseMoveEvent(e);
}
