#include "csettingswnd4plugins.h"
#include "ccomponent.h"
#include "cconfirmdlg.h"
#include "cfiledialog.h"
#include "cthememanager.h"
#include "extensions.h"
#include "settingshelper.h"
#include "stable.h"
#include "ui_csettingswnd4plugins.h"
#include "utilities.h"

#include <qfile.h>
#include <qsettings.h>
#include <qstandarditemmodel.h>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>
#include <QUrl>
#include <QScrollBar>
#include "qjsondocument.h"

//TODO: tab 里两个 输入框高度不一致
CSettingsWnd4Plugins::CSettingsWnd4Plugins(QWidget *parent)
    : QWidget(parent),
      m_pmodel(new QStandardItemModel(this)),
      ui(new Ui::CSettingsWnd4Plugins)
{
    ui->setupUi(this);

    // table
    {
        ui->tableView->setObjectName("local_plugin_view");
        ui->tableView->verticalScrollBar()->setObjectName("sub");
        ui->tableView->setModel(m_pmodel);
        m_pmodel->setColumnCount(3);
        m_pmodel->setHorizontalHeaderLabels(
            QStringList() << tr("Name") << tr("Extension") << tr("Command"));

        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->setHorizontalScrollMode(
            QAbstractItemView::ScrollPerPixel);
        auto hheader = ui->tableView->horizontalHeader();
        hheader->setSectionResizeMode(QHeaderView::ResizeToContents);
        hheader->setStretchLastSection(true);
        hheader->setSectionsClickable(false);
        ui->tableView->setColumnHidden(2, true);
        //改变顺序
        // ui->tableView->verticalHeader()->setVisible(true);
        ui->tableView->verticalHeader()->setSectionsMovable(true);
        ui->tableView->verticalHeader()->setSelectionBehavior(
            QAbstractItemView::SelectRows);
        ui->tableView->setFocusPolicy(Qt::ClickFocus);

        connect(ui->tableView->selectionModel(),
                &QItemSelectionModel::selectionChanged,
                [=](const QItemSelection &selected) {
                    ui->pushButton_del->setEnabled(!selected.isEmpty());
                    ui->pushButton_edit->setEnabled(!selected.isEmpty());
                });
    }

    // modify plugin content
    {
        ui->Layout_local_content->addWidget(&m_wnd_add);
        connect(&m_wnd_add, &CSettingsWnd4PluginsAdd::SigCancel, this,
                [=]() { SetAddPluginVisibel(false); });
        connect(&m_wnd_add, &CSettingsWnd4PluginsAdd::SigSubmit, this,
                &CSettingsWnd4Plugins::SlotAddPluginSubmit);
    }
    {
        ui->pushButton_add->setText(fa::c(fa::ini_add));

        ui->pushButton_edit->setText(fa::c(fa::ini_edit));
        ui->pushButton_edit->setEnabled(false);

        ui->pushButton_del->setText(fa::c(fa::ini_trash));
        ui->pushButton_del->setEnabled(false);

        ui->pushButton_about_plugin->setText(fa::c(fa::question));
        ui->pushButton_about_plugin->setToolTip(web_plugin);

        ui->lineEdit_search->setPlaceholderText(tr("Search plugin"));
        ui->lineEdit_search->setClearButtonEnabled(true);

        SetAddPluginVisibel(false);
    }

    foreach (auto i, this->findChildren<QWidget *>()) {
        i->setFocusPolicy(Qt::NoFocus);
        if (i->inherits("QLineEdit")) {
            i->setFocusPolicy(Qt::ClickFocus);
        }
        else if (i->inherits("QPushButton")) {
            i->setCursor(Qt::PointingHandCursor);
        }
    }

    InitData();
    ui->gridLayout_available->addWidget(&m_wnd_available);
    ui->tabWidget->setCurrentIndex(!m_pmodel->rowCount());
}

void CSettingsWnd4Plugins::UpdateUI(int title_height, CScreenAdapter *dpi)
{
    auto item_height
        = ui->tableView->fontMetrics().height() + 14 * dpi->ratio();
    QHeaderView *verticalHeader = ui->tableView->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(item_height);
    auto hheader = ui->tableView->horizontalHeader();
    hheader->setFixedHeight(item_height);
    hheader->setMinimumSectionSize(hheader->fontMetrics().width("Name123456"));

    const auto x = dpi->x();
    ui->pushButton_add->setFont(fa::font(21 * dpi->ratio()));
    ui->pushButton_edit->setFont(fa::font(21 * dpi->ratio()));
    ui->pushButton_del->setFont(fa::font(21 * dpi->ratio()));
    ui->pushButton_about_plugin->setFont(fa::font(21 * dpi->ratio()));

    // ui display
    ui->Layout_local->setContentsMargins(x * 4, x * 4, x * 4, x * 4);
    ui->Layout_local_content->setSpacing(5 * x);
    const auto top = title_height - ui->tabWidget->tabBar()->height() / 2;
    ui->Layout_container->setContentsMargins(9 * x, top, 9 * x, 0);

    m_wnd_add.UpdateUI(dpi);
    m_wnd_available.UpdateUI(dpi);
}

void CSettingsWnd4Plugins::on_pushButton_about_plugin_clicked()
{
    OpenByDefaultApp(
        QUrl(ui->pushButton_about_plugin->toolTip(), QUrl::TolerantMode));
}

CSettingsWnd4Plugins::~CSettingsWnd4Plugins()
{
    delete ui;
}

void CSettingsWnd4Plugins::InitData()
{
    QList<QStandardItem *> row;
    foreach (const PluginData &i, g_data->plugins) {
        row.clear();
        row << new QStandardItem(i.name)
            << new QStandardItem(CExtensionList(i.suffix).forDisplay())
            << new QStandardItem(i.para);
        m_pmodel->appendRow(row);
    }
}

void CSettingsWnd4Plugins::WriteSettings()
{
    PluginsList new_data;
    QJsonArray json_arr;
    PluginData p_data;

    auto header      = ui->tableView->verticalHeader();
    const auto row_t = m_pmodel->rowCount();
    for (int i = 0; i < row_t; ++i) {
        int logical_row = header->logicalIndex(i);
        p_data.name     = m_pmodel->item(logical_row, PC_Name)->text();
        p_data.suffix   = m_pmodel->item(logical_row, PC_Suffix)
                            ->text()
                            .split(' ', QString::SkipEmptyParts);
        p_data.para = m_pmodel->item(logical_row, PC_Para)->text();
        new_data.append(p_data);

        QJsonArray arr;
        arr.append(p_data.name);
        arr.append(m_pmodel->item(logical_row, PC_Suffix)->text());
        arr.append(p_data.para);
        json_arr.append(arr);
    }

    g_pini->SetIni(Ini::plugins_data_bytes, QJsonDocument(json_arr).toJson());
    g_data->plugins = new_data;
}

void CSettingsWnd4Plugins::SlotAddPluginSubmit(const PluginData &new_data)
{
    int row_same_plugin = -1;
    for (int i = 0; i < m_pmodel->rowCount(); ++i) {
        if (m_pmodel->item(i, 1)->text().split(" ", QString::SkipEmptyParts)
                == new_data.suffix
            && m_pmodel->item(i, 2)->text().trimmed()
                   == new_data.para.trimmed()) {
            row_same_plugin = i;
            break;
        }
    }

    if (row_same_plugin != -1) {
        const auto index = m_pmodel->index(row_same_plugin, 0);
        ui->tableView->scrollTo(index);
        ui->tableView->setCurrentIndex(index);
        emit SigShowInfoText(
            tr("Plugin already exists") + " - " + index.data().toString(),
            13000);
    }
    else {
        const auto suf = CExtensionList(new_data.suffix).forDisplay();
        if (m_wnd_add.row_editing != -1) {
            m_pmodel->setData(m_pmodel->index(m_wnd_add.row_editing, PC_Name),
                              new_data.name);
            m_pmodel->setData(m_pmodel->index(m_wnd_add.row_editing, PC_Suffix),
                              suf);
            m_pmodel->setData(m_pmodel->index(m_wnd_add.row_editing, PC_Para),
                              new_data.para);
        }
        else {
            QList<QStandardItem *> row;
            row << new QStandardItem(new_data.name) << new QStandardItem(suf)
                << new QStandardItem(new_data.para);
            m_pmodel->appendRow(row);
            auto index = m_pmodel->index(m_pmodel->rowCount() - 1, 0);
            ui->tableView->scrollTo(index);
            ui->tableView->setCurrentIndex(index);
        }
    }
    m_wnd_add.on_pushButton_cancel_clicked();
}

void CSettingsWnd4Plugins::SetAddPluginVisibel(bool arg)
{
    m_wnd_add.setVisible(arg);
    ui->wnd_add_main->setVisible(!arg);
}

void CSettingsWnd4Plugins::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    QTimer::singleShot(0, ui->lineEdit_search, SLOT(setFocus()));
}

void CSettingsWnd4Plugins::on_pushButton_add_clicked()
{
#define PARSE_ERR(p)                                                \
    emit SigShowInfoText("Parse " + QString(p) + " failed.", 9000); \
    return;

    //    SetAddPluginVisibel(true);
    const auto path = CFileDialog::GetOpenFilePath(
        this, tr("Choose Seer plugin"), "Seer plugin (*.json)");
    if (path.isEmpty()) {
        return;
    }
    QFile f(path);
    if (!f.open(f.ReadOnly)) {
        emit SigShowInfoText("Open failed.", 9000);
        return;
    }
    const auto json = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (json.isNull()) {
        PARSE_ERR("")
    }
    const auto obj = json.object();
    if (obj.isEmpty()) {
        PARSE_ERR("")
    }
    if (obj.value("name").isUndefined() || obj.value("suffix").isUndefined()
        || obj.value("exec").isUndefined()) {
        PARSE_ERR("")
    }
    PluginData res;
    // name
    res.name = obj.value("name").toString();
    if (res.name.isEmpty()) {
        PARSE_ERR("name")
    }
    // suffix
    {
        auto arr = obj.value("suffix").toArray();
        if (arr.isEmpty()) {
            PARSE_ERR("suffix")
        }
        for (int i = 0; i < arr.count(); ++i) {
            auto t = arr.at(i);
            if (t.isUndefined() || t.toString().isEmpty()) {
                PARSE_ERR("suffix")
            }
            res.suffix.append(t.toString());
        }
    }
    // exec
    res.para = obj.value("exec").toString();
    if (res.para.isEmpty()) {
        PARSE_ERR("exec")
    }
    auto args = res.para.split(" ", QString::SkipEmptyParts);
    if (args.isEmpty()) {
        PARSE_ERR("exec")
    }
    QFileInfo info(QFileInfo(path).absoluteDir().absolutePath() + "/"
                   + args.first());
    if (!info.exists()) {
        emit SigShowInfoText(info.fileName() + " not found.", 9000);
        return;
    }
    args[0]  = "\"" + info.absoluteFilePath() + "\"";
    res.para = args.join(" ");
    SlotAddPluginSubmit(res);
}

void CSettingsWnd4Plugins::on_pushButton_del_clicked()
{
    auto i = ui->tableView->currentIndex();
    if (!i.isValid()) {
        return;
    }
    m_pmodel->removeRow(i.row());
}

void CSettingsWnd4Plugins::on_tableView_doubleClicked(const QModelIndex &index)
{
    on_pushButton_edit_clicked();
    m_wnd_add.SetFocus((CSettingsWnd4PluginsAdd::FocusControl)index.column());
}

void CSettingsWnd4Plugins::on_lineEdit_search_textChanged(const QString &arg)
{
    const QString text = arg.trimmed();
    const auto row_t   = m_pmodel->rowCount();
    for (int i = 0; i < row_t; ++i) {
        ui->tableView->setRowHidden(
            i,
            !(m_pmodel->item(i, 0)->text().contains(text, Qt::CaseInsensitive)
              || m_pmodel->item(i, 1)->text().contains(text,
                                                       Qt::CaseInsensitive)
              /*|| m_pmodel->item(i, 2)->text().contains(text, Qt::CaseInsensitive)*/));
    }
}

void CSettingsWnd4Plugins::on_pushButton_edit_clicked()
{
    SetAddPluginVisibel(true);
    m_wnd_add.SetFocus((CSettingsWnd4PluginsAdd::FocusControl)0);

    const auto row = ui->tableView->currentIndex().row();
    QStringList data;
    data << m_pmodel->item(row, PC_Name)->text();
    data << m_pmodel->item(row, PC_Para)->text();
    data << m_pmodel->item(row, PC_Suffix)->text();
    m_wnd_add.SetData(data);
    m_wnd_add.row_editing = row;
}
