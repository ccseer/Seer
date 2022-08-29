#include "mainwindow.h"

#include <tchar.h>
#include <windows.h>

#include <QDebug>
#include <QFileSystemModel>
#include <QShortcut>

#include "../seer_ipc.h"
#include "ui_mainwindow.h"

#pragma comment(lib, "User32.lib")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initView();

    // handle the key event from this
    auto sc = new QShortcut({"Space"}, this);
    connect(sc, &QShortcut::activated, this, &MainWindow::onSpacebarTriggered);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void sendPath2Seer(HWND seer, LPCWSTR path)
{
    COPYDATASTRUCT cd;
    cd.cbData = (_tcslen(path) + 1) * sizeof(TCHAR);
    cd.lpData = (LPVOID)path;
    cd.dwData = SEER_INVOKE_W32;
    SendMessage(seer, WM_COPYDATA, 0, (LPARAM)&cd);

    /*********************
    * invoke with Qt
    *********************
       QByteArray ba;
       QDataStream ds(&ba, QIODevice::WriteOnly);
       ds << QString(file_full_path);

       COPYDATASTRUCT cd;
       cd.cbData = ba.size();
       cd.lpData = (void *)ba.data();
       cd.dwData = SEER_INVOKE_QT;
       auto err = SendMessage(h, WM_COPYDATA, 0, (LPARAM)(LPVOID)&cd);
    */
}

/////////////////////////////////////////
/// \brief MainWindow::onSpacebarTriggered
/// This is used to hanle spacebar key event
void MainWindow::onSpacebarTriggered()
{
    qDebug() << "onSpacebarTriggered";

    HWND seer = FindWindowEx(nullptr, nullptr, SEER_CLASS_NAME, nullptr);
    if (!seer) {
        qDebug() << "seer not found";
        return;
    }
    TCHAR path[MAX_PATH] = {0};
    // _tcscpy(path, L"C:\\7z.exe");
    const QString path_qt = getSelectedFilePath();
    path_qt.toWCharArray(path);

    sendPath2Seer(seer, path);
}

/////////////////////////////////////////
/// \brief MainWindow::onSelectedFileChanged
/// This is used to hanle selection change.
/// Sometimes this function is used to handle arrow keys(Left/Right/Up/Down).
void MainWindow::onSelectedFileChanged()
{
    HWND seer = FindWindowEx(nullptr, nullptr, SEER_CLASS_NAME, nullptr);
    if (!seer) {
        return;
    }
    // Seer is not visible
    COPYDATASTRUCT cd;
    cd.cbData = 0;
    cd.lpData = nullptr;
    cd.dwData = SEER_IS_VISIBLE;
    if (SEER_IS_VISIBLE_FALSE
        == SendMessage(seer, WM_COPYDATA, 0, (LPARAM)&cd)) {
        return;
    }

    // Seer is visible and the selected file is changed,
    // then feed Seer with the new file path.
    // a. Sending an empty string has no effect.
    // b. Sending the previewing path closes the Seer window.
    TCHAR path[MAX_PATH]  = {0};
    const QString path_qt = getSelectedFilePath();
    path_qt.toWCharArray(path);

    sendPath2Seer(seer, path);
}

///////////////////////////////////////////////////////////
// UI related
void MainWindow::initView()
{
    auto fsm = new QFileSystemModel(this);
    fsm->setRootPath(QDir::drives().first().absoluteFilePath());
    ui->treeView->setModel(fsm);
    ui->treeView->expand(fsm->index(0, 0));
    ui->treeView->setColumnWidth(0, 400);
    auto sm = ui->treeView->selectionModel();
    connect(sm, &QItemSelectionModel::selectionChanged, this,
            &MainWindow::onSelectedFileChanged);
}

QString MainWindow::getSelectedFilePath() const
{
    auto sm         = ui->treeView->selectionModel();
    const auto list = sm->selectedRows(0);
    for (const auto &i : list) {
        auto fsm = qobject_cast<QFileSystemModel *>(ui->treeView->model());
        return fsm->filePath(i);
    }
    return {};
}
