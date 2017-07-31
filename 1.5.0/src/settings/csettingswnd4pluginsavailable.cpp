#include "csettingswnd4pluginsavailable.h"
#include "settingshelper.h"
#include "stable.h"
#include "ui_csettingswnd4pluginsavailable.h"
#include "utilities.h"

#include <QJsonArray>
#include <QScrollBar>
#include "qjsondocument.h"

CSettingsWnd4PluginsAvailable::CSettingsWnd4PluginsAvailable(QWidget *parent)
    : QWidget(parent),
      m_const_text(tr("Download")),
      m_pview(new QListView),
      m_psplitter(new QSplitter(Qt::Horizontal)),
      m_peditor(new QTextBrowser),
      ui(new Ui::CSettingsWnd4PluginsAvailable)
{
    ui->setupUi(this);
    ui->pushButton_dl->setText(m_const_text);
    ui->pushButton_dl->setObjectName("btn_plugin_dl");
    ui->pushButton_dl->setCursor(Qt::PointingHandCursor);
    ui->pushButton_dl->setEnabled(false);
    ui->wnd_btns->setContentsMargins(0, 0, 0, 0);
    ui->Layout_btns->setContentsMargins(0, 0, 0, 0);

    ui->lineEdit->setPlaceholderText(tr("Search available plugin"));
    ui->lineEdit->setClearButtonEnabled(true);

    ui->Layout_splitter->addWidget(m_psplitter);
    m_psplitter->addWidget(m_pview);
    m_psplitter->addWidget(m_peditor);
    m_psplitter->setCollapsible(m_psplitter->indexOf(m_pview), false);
    m_psplitter->setCollapsible(m_psplitter->indexOf(m_peditor), false);

    m_peditor->setOpenExternalLinks(true);
    m_peditor->setPlaceholderText(tr("Select a plugin on the left"));
    m_peditor->verticalScrollBar()->setObjectName("sub");

    InitData();
    m_pview->setModel(&m_model);
    m_pview->verticalScrollBar()->setObjectName("sub");
    m_pview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pview->setAlternatingRowColors(true);
    m_pview->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pview->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pview->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(
        m_pview->selectionModel(), &QItemSelectionModel::selectionChanged,
        [=](const QItemSelection &selected, const QItemSelection &) {
            ui->pushButton_dl->setEnabled(!selected.isEmpty());
            m_peditor->clear();

            QString res;
            if (!selected.isEmpty()) {
                auto index = selected.indexes().first();
                res        = index.data().toString().prepend(" ").append("...");

                QString editor_text;
                editor_text.append(tr("Extension:") + "<br/>")
                    .append(m_model.index(index.row(), 1).data().toString());
                editor_text.append("<br/><br/>");
                editor_text.append(tr("Description:") + "<br/>")
                    .append(m_model.index(index.row(), 2).data().toString());

                m_peditor->setHtml(editor_text);
            }
            ui->pushButton_dl->setText(m_const_text + res);
        });
}

CSettingsWnd4PluginsAvailable::~CSettingsWnd4PluginsAvailable()
{
    delete ui;
}

void CSettingsWnd4PluginsAvailable::UpdateUI(CScreenAdapter *dpi)
{
    auto f = qApp->font();
    f.setPixelSize(14 * dpi->ratio());
    ui->pushButton_dl->setFont(f);
    ui->wnd_btns->setFixedHeight(ui->pushButton_dl->fontMetrics().height()
                                 * 1.5);
}

void CSettingsWnd4PluginsAvailable::InitData()
{
    const auto data = g_pini->GetIni(Ini::plugins_data_all_bytes).toByteArray();
    const auto doc  = QJsonDocument::fromJson(data);
    if (doc.isEmpty()) {
        elprint;
        return;
    }
    const auto arr = doc.array();
    if (arr.isEmpty()) {
        elprint;
        return;
    }

    QList<QStringList> items_res;
    const auto arr_t = arr.size();
    for (int i = 0; i < arr_t; ++i) {
        const auto sub_arr = arr[i].toArray();
        if (!sub_arr.isEmpty()) {
            // size = 4
            const auto sub_arr_t = sub_arr.size();
            QStringList list;
            for (int j = 0; j < sub_arr_t; ++j) {
                list.append(sub_arr.at(j).toString());
            }
            items_res.append(list);
        }
    }

    QList<QStandardItem *> items;
    foreach (const auto &i, items_res) {
        items.clear();
        foreach (const QString &j, i) {
            items.append(new QStandardItem(j));
        }
        m_model.appendRow(items);
    }
    m_model.setHorizontalHeaderLabels(QStringList()
                                      << tr("Name") << tr("Extension")
                                      << tr("Description") << tr("Download"));
}

void CSettingsWnd4PluginsAvailable::on_pushButton_dl_clicked()
{
    const auto index = m_pview->currentIndex();
    if (index.isValid()) {
        OpenByDefaultApp(
            QUrl(m_model.item(index.row(), 3)->text(), QUrl::TolerantMode));
    }
}

void CSettingsWnd4PluginsAvailable::on_lineEdit_textChanged(const QString &arg)
{
    const QString text = arg.trimmed();
    const auto row_t   = m_model.rowCount();
    for (int i = 0; i < row_t; ++i) {
        m_pview->setRowHidden(
            i,
            !(m_model.item(i, 0)->text().contains(text, Qt::CaseInsensitive)
              || m_model.item(i, 1)->text().contains(text, Qt::CaseInsensitive)
              || m_model.item(i, 2)->text().contains(text, Qt::CaseInsensitive)
              /*|| m_pmodel->item(i, 2)->text().contains(text, Qt::CaseInsensitive)*/));
    }
}
