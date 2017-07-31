#include "csettingswnd4general.h"
#include "cflagdelegate.h"
#include "cglobaldata.h"
#include "cthememanager.h"
#include "ctranslationini.h"
#include "settingshelper.h"
#include "ui_csettingswnd4general.h"

#include <QGraphicsOpacityEffect>
#include <QMouseEvent>
#include <QSettings>
#include "qdir.h"
#include "qlistview.h"
#include "qscrollbar.h"
#include "qtimer.h"

CSettingsWnd4General::CSettingsWnd4General(QWidget *parent)
    : QWidget(parent), ui(new Ui::CSettingsWnd4General)
{
    ui->setupUi(this);

    ui->scrollArea->verticalScrollBar()->setObjectName("sub");

    foreach (auto i, findChildren<QWidget *>()) {
        if (i->objectName().startsWith("line") && !i->inherits("QLineEdit")) {
            i->setObjectName("line");
        }
        else if (i->objectName().startsWith("label_group")) {
            i->setObjectName("label_group");
        }
    }

    // theme btn
    QButtonGroup *grp = new QButtonGroup(this);
    {
        foreach (auto i, GetThemeBtnMap().values()) {
            i->setCheckable(true);
            i->setChecked(false);
            grp->addButton(i);
            i->setCursor(Qt::PointingHandCursor);
        }
        grp->setExclusive(true);
        connect(grp,
                (void (QButtonGroup::*)(QAbstractButton *))
                    & QButtonGroup::buttonClicked,
                this, &CSettingsWnd4General::SlotThemeBtnClicked);
    }

    InitLanView();
    UpdateSettings();

    QTimer::singleShot(0, this,
                       [=]() { SlotThemeBtnClicked(grp->checkedButton()); });
}

void CSettingsWnd4General::UpdateUI(int title_height, CScreenAdapter *dpi)
{
    const auto x = dpi->x();
    {
        // list view 上面会 空一点出来.在这里补回来
        ui->Layout->setVerticalSpacing(x * 8);
        ui->Layout_app->setSpacing(x * 5);
        ui->Layout_lan->setSpacing(x * 5);
        ui->Layout_wnd_app_behavior->setContentsMargins(0, 0, 0, 3 * x);
    }

    auto height = 0;
    foreach (auto i, findChildren<QLabel *>()) {
        if (i->objectName().startsWith("label_group")) {
            auto f = i->font();
            f.setPixelSize(15 * dpi->ratio());
            i->setFont(f);
            i->setFixedWidth(i->fontMetrics().width(i->text())
                             + i->fontMetrics().xHeight() * 2);
            height = i->height();
        }
    }
    ui->Layout->setContentsMargins(9 * x, title_height - height / 2, 9 * x, 0);

    // theme
    const auto h = ui->checkBox_autorun->height();
    foreach (auto i, GetThemeBtnMap().values()) {
        i->setFixedSize(h, h);
    }

    if (auto cfd = this->findChild<CFlagDelegate *>()) {
        cfd->UpdateDpi(dpi->ratio());
    }
}

void CSettingsWnd4General::InitLanView()
{
    ui->listView->setModel(&m_model);
    ui->listView->setViewMode(QListView::IconMode);
    ui->listView->setEditTriggers(QListView::NoEditTriggers);
    ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listView->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->listView->setDragEnabled(false);
    ui->listView->setResizeMode(QListView::Adjust);
    ui->listView->setTextElideMode(Qt::ElideMiddle);
    ui->listView->setUniformItemSizes(true);
    auto cfd = new CFlagDelegate(this);
    ui->listView->setItemDelegate(cfd);

    ui->listView->setMouseTracking(true);
    ui->listView->viewport()->installEventFilter(this);

    const QString def_name = "English";
    const QMap<QString, QString> lan{
        {def_name, "://settings/english.png"},
        {"zh_CN", "://settings/zh_CN.png"},
        {"Japanese", "://settings/japan.png"},
        {"Italian", "://settings/italian.png"},
        {"Russian", "://settings/russian.png"},
        {"Spanish", "://settings/spanish.png"},
        {"Swedish", "://settings/swedish.png"},
        {"German", "://settings/Germany.png"},
        {"zh_TW", "://settings/taiwan.png"},
        {"French", "://settings/french.png"},
    };

    auto item = new QStandardItem;
    item->setText(def_name);
    item->setIcon(QIcon(lan.value(def_name)));
    item->setData(QVariant::fromValue(item->icon()), g_orig_icon_role);
    m_model.appendRow(item);

    foreach (
        const auto &i,
        QDir(g_i18n_path)
            .entryInfoList(QStringList() << "*.qm", QDir::Files, QDir::Name)) {
        auto item          = new QStandardItem;
        const QString name = i.baseName();
        item->setText(name);
        if (lan.contains(name)) {
            item->setIcon(QIcon(lan.value(name)));
            item->setData(QVariant::fromValue(item->icon()), g_orig_icon_role);
        }
        m_model.appendRow(item);
    }

    m_cur_tr = Translation_Ini::GetCurTranslation();
    if (m_cur_tr.isEmpty()) {
        m_cur_tr = g_i18n_eng;
    }
    for (int i = 0; i < m_model.rowCount(); ++i) {
        if (m_cur_tr == m_model.index(i, 0).data()) {
            m_model.itemFromIndex(m_model.index(i, 0))
                ->setData(true, g_cur_role);
            break;
        }
    }
}

bool CSettingsWnd4General::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->listView->viewport()) {
        const auto t = event->type();
        if (t == QEvent::Leave) {
            setCursor(Qt::ArrowCursor);
        }
        else if (t == QEvent::MouseMove) {
            auto e                    = (QMouseEvent *)event;
            const auto index          = ui->listView->indexAt(e->pos());
            Qt::CursorShape res_shape = Qt::ArrowCursor;
            if (index.isValid()) {
                // text, no flag
                if (index.data(g_orig_icon_role).value<QIcon>().isNull()) {
                    res_shape = Qt::PointingHandCursor;
                }
                // has flag
                else {
                    // width of flag == g_flag_len
                    if (auto cfd = this->findChild<CFlagDelegate *>()) {
                        auto flag_l = cfd->GetFlagBaseSz();
                        if (QRect(ui->listView->visualRect(index).topLeft(),
                                  QSize(flag_l, flag_l))
                                .contains(e->pos())) {
                            res_shape = Qt::PointingHandCursor;
                        }
                    }
                }
            }
            if (cursor().shape() != res_shape) {
                setCursor(res_shape);
            }
            return true;
        }
    }
    return false;
}

void CSettingsWnd4General::on_listView_clicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    //取消上次的选中
    for (int i = 0; i < m_model.rowCount(); ++i) {
        auto row = m_model.index(i, 0);
        if (true == row.data(g_cur_role)) {
            if (index == row) {
                return;
            }
            m_model.itemFromIndex(m_model.index(i, 0))
                ->setData(false, g_cur_role);
            break;
        }
    }

    m_model.itemFromIndex(index)->setData(true, g_cur_role);
    ui->listView->selectionModel()->select(index,
                                           QItemSelectionModel::ToggleCurrent);

    emit SigRequiredRestart(qHash(Q_FUNC_INFO),
                            index.data().toString() != m_cur_tr);
}

void CSettingsWnd4General::SlotThemeBtnClicked(QAbstractButton *btn)
{
    // btn 被选中后 再去点击仍会响应
    const auto map = GetThemeBtnMap();
    emit SigRequiredRestart(
        qHash(Q_FUNC_INFO),
        map.key((QPushButton *)btn) != g_pstyle->GetCurStyle());

    foreach (auto i, map.values()) {
        QGraphicsDropShadowEffect *eff = NULL;
        if (i->isChecked()) {
            eff = new QGraphicsDropShadowEffect(this);
            eff->setColor(g_pstyle->GetSettingsThemeRectColor());
            eff->setBlurRadius(10);
            eff->setXOffset(0);
            eff->setYOffset(0);
        }
        i->setGraphicsEffect(eff);
    }
}

CSettingsWnd4General::~CSettingsWnd4General()
{
    delete ui;
}

void CSettingsWnd4General::WriteAutorun()
{
    const QString name = QApplication::applicationName();
    QSettings settings(REG_AUTO_RUN, QSettings::NativeFormat);
    if (ui->checkBox_autorun->isChecked()) {
        QString path = QApplication::applicationFilePath();
        settings.setValue(name, path.replace("/", "\\"));
    }
    else {
        settings.remove(name);
    }
}

void CSettingsWnd4General::WriteSettings()
{
    g_pini->SetIni(Ini::autorun_b, ui->checkBox_autorun->isChecked());
    g_pini->SetIni(Ini::auto_update_b, ui->checkBox_auto_update->isChecked());
    g_pini->SetIni(Ini::openfile_by_dclick_b,
                   ui->checkBox_dclicked_by_default->isChecked());
    g_pini->SetIni(Ini::use_hook_key_close_b,
                   ui->checkBox_close_wnd_by_trigger_key->isChecked());
    g_pini->SetIni(Ini::tracking_file_b, ui->checkBox_tracking->isChecked());
    const auto map = GetThemeBtnMap();
    foreach (auto i, map.values()) {
        if (i->isChecked()) {
            g_pstyle->SetCurStyle(map.key(i));
            break;
        }
    }

    WriteAutorun();

    for (int i = 0; i < m_model.rowCount(); ++i) {
        auto row = m_model.index(i, 0);
        if (true == row.data(g_cur_role) && row.data() != m_cur_tr) {
            Translation_Ini::SetCurTranslation(row.data().toString());
            break;
        }
    }
}

const QMap<QssStyle, QPushButton *> CSettingsWnd4General::GetThemeBtnMap()
{
    const QMap<QssStyle, QPushButton *> map{
        {Qss_Default, ui->pushButton_style_default},
        {Qss_Dark, ui->pushButton_style_dark},
    };
    return map;
}

void CSettingsWnd4General::UpdateSettings()
{
    ui->checkBox_autorun->setChecked(g_pini->GetIni(Ini::autorun_b).toBool());
    ui->checkBox_dclicked_by_default->setChecked(
        g_pini->GetIni(Ini::openfile_by_dclick_b).toBool());
    ui->checkBox_close_wnd_by_trigger_key->setChecked(
        g_pini->GetIni(Ini::use_hook_key_close_b).toBool());
    ui->checkBox_tracking->setChecked(
        g_pini->GetIni(Ini::tracking_file_b).toBool());
    ui->checkBox_auto_update->setChecked(
        g_pini->GetIni(Ini::auto_update_b).toBool());

    GetThemeBtnMap().value(g_pstyle->GetCurStyle())->setChecked(true);
}
