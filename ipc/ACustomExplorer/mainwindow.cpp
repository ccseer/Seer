#include "mainwindow.h"

#include <Shlobj.h>
#include <Shlwapi.h>
#include <tchar.h>

#include <QDir>
#include <QEvent>
#include <QFileSystemModel>

#include "../seer_ipc.h"
#include "ui_mainwindow.h"

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shlwapi.lib")

#define WND_TEXT "ACustomExplorer_Title"
// Qt internal defined, not unique, so we need window text
#define WND_CLASSNAME "Qt5152QWindowIcon"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(WND_TEXT);

    initView();
    initSeer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initSeer()
{
    ///////////////////////////////////////////////////////////////
    /// \brief
    /// This is how IPC works:
    /// The folder for path_json is created when the Seer starts.
    /// 1. we need to create a json file with your classname in it.
    ///    This function(MainWindow::initSeer) is for this purpose.
    ///    In most cases, the classname is unique, so no need for window text.
    ///
    /// 2. When Seer starts, Seer reads the json file and stores your classname
    ///    in memory. When Seer detects a space message, Seer tries to match
    ///    the classname(GetForegroundWindow) with yours.
    ///
    /// 3. When the classname matches, Seer sends a WM_COPYDATA message to your
    ///    program, the dwData is SEER_REQUEST_PATH.
    ///
    /// 4. When your program receives the message, please send back the full
    ///    path of the selected file immediately. The example code is written
    ///    in MainWindow::onCopyDataMsg
    ///
    /// 5. Finally, Seer gets the SEER_RESPONSE_PATH then preview the file.

    TCHAR filename[256]  = _T("your_unique_file.json");
    TCHAR classname[256] = _T(WND_CLASSNAME);
    // if your classname is unique, then no need to pass windowtext
    // TCHAR windowtext[256] ={0};
    TCHAR windowtext[256] = _T(WND_TEXT);
    // 1. prepare file path
    TCHAR path_json[MAX_PATH] = {0};
    if (FAILED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path_json))) {
        // should not happen
        OutputDebugString(_T("SHGetFolderPath error\n"));
        return;
    }
    // path_doc = %USERPROFILE%\Documents
    _tcscat(path_json, _T("\\Seer\\"));
    _tcscat(path_json, _T(SEER_EXPLORER_FOLDER));
    // path_doc = %USERPROFILE%\Documents\Seer\explorers

    // 2. check file
    if (!PathFileExists(path_json)) {
        // Seer will try to create the folder everytime it starts.
        // If the folder is not found, then Seer is not installed,
        // or Seer has never been run.
        OutputDebugString(_T("folder not found\n"));
        return;
    }
    _tcscat(path_json, _T("\\"));
    _tcscat(path_json, filename);
    // path_doc = %USERPROFILE%\Documents\Seer\explorers\file_name
    if (PathFileExists(path_json)) {
        // no need to write again
        OutputDebugString(_T("file exists\n"));
        return;
    }

    // 3. write the file
    HANDLE handle = CreateFile(path_json, GENERIC_WRITE, 0, NULL, CREATE_NEW,
                               FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        // should not happen
        OutputDebugString(_T("CreateFile error\n"));
        return;
    }

    std::string buf;
    buf.append("{\"");
    buf.append(SEER_JSON_KEY_CLASSNAME);
    buf.append("\":\"");
    std::wstring wstr = classname;
    buf.append(std::string(wstr.begin(), wstr.end()));
    buf.append("\"");
    wstr = windowtext;
    if (wstr.length()) {
        buf.append(",\"");
        buf.append(SEER_JSON_KEY_WINDOWTEXT);
        buf.append("\":\"");
        buf.append(std::string(wstr.begin(), wstr.end()));
        buf.append("\"");
    }
    buf.append("}");
    bool ret = WriteFile(handle, buf.c_str(), buf.length(), nullptr, nullptr);
    if (!ret) {
        OutputDebugString(_T("WriteFile error\n"));
    }
    else {
        /// {
        ///   "classname": "classnanme of your window",
        ///   "windowtext": "windowtext of your window [optional]",
        ///   "appname": "name of your software [optional]"
        /// }
    }
    CloseHandle(handle);
}

void MainWindow::onCopyDataFromSeer()
{
    //***function has to be done in 150ms, otherwise it fails***
    if (HWND h = FindWindowEx(nullptr, nullptr, SEER_CLASS_NAME, nullptr)) {
        // send selected file path to Seer
        const QString path_qt = getSelectedFilePath();
        // target file
        TCHAR path[MAX_PATH] = {0};
        path_qt.toWCharArray(path);
        // _tcscpy(path, L"C:\\D\\7z.exe");

        COPYDATASTRUCT cd;
        cd.cbData = (_tcslen(path) + 1) * sizeof(TCHAR);
        cd.lpData = (LPVOID)path;
        cd.dwData = SEER_RESPONSE_PATH;
        if (FAILED(SendMessage(h, WM_COPYDATA, 0, (LPARAM)&cd))) {
            OutputDebugString(_T("SendMessage error\n"));
        }
    }
    else {
        OutputDebugString(_T("SEER_CLASS_NAME not found\n"));
    }
}

///////////////////////////////////////////////////////////////////
/// UI Related
bool MainWindow::nativeEvent(const QByteArray &, void *m, long *result)
{
    const auto msg = (MSG *)m;
    if (msg->message == WM_COPYDATA) {
        if (auto cds = (PCOPYDATASTRUCT)msg->lParam) {
            if (cds->dwData == SEER_REQUEST_PATH) {
                onCopyDataFromSeer();
                // LRESULT returns to Seer, but not used currently
                // *result = 1;
                return true;
            }
        }
    }
    return false;
}

void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::ActivationChange && isActiveWindow()) {
        auto hwnd           = GetForegroundWindow();
        TCHAR buf[MAX_PATH] = {'\0'};
        if (GetWindowText(hwnd, buf, MAX_PATH)) {
            QString info = "WindowText:" + QString::fromWCharArray(buf);
            info.append("\t");
            buf[0] = _T('\0');
            GetClassName(hwnd, buf, MAX_PATH);
            info.append("ClassName:" + QString::fromWCharArray(buf));
            ui->label_info->setText(info);
        }
        else {
            ui->label_info->setText("GetWindowText Error");
        }
    }
    __super::changeEvent(e);
}

void MainWindow::initView()
{
    auto fsm = new QFileSystemModel(this);
    fsm->setRootPath(QDir::drives().first().absoluteFilePath());
    ui->treeView->setModel(fsm);
    ui->treeView->expand(fsm->index(0, 0));
    ui->treeView->setColumnWidth(0, 400);
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
