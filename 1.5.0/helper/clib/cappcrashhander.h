#ifndef CAPPCRASHHANDER
#define CAPPCRASHHANDER

#include <Windows.h>
#include <QMessageBox>
#include <QProcess>

LONG ApplicationCrashHandler(EXCEPTION_POINTERS /**pException*/)
{
    foreach (QWidget *widget, QApplication::allWidgets()) {
        widget->close();
    }

    //程式异常捕获
    //这里弹出一个错误对话框并退出程序
    //    EXCEPTION_RECORD* record = pException->ExceptionRecord;
    //    QString errCode(QString::number(record->ExceptionCode, 16));
    //    QString errAdr(QString::number((uint)record->ExceptionAddress, 16));
    if (QMessageBox::Ok
        == QMessageBox::critical(NULL, "Sorry",
                                 QString("<FONT size=4><div><b>Something "
                                         "unexpected happened.</b><br/></div>"
                                         "Seer will start again.</FONT>"),
                                 QMessageBox::Ok, QMessageBox::Cancel)) {
        QProcess::startDetached(qApp->applicationFilePath());
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

#endif  // CAPPCRASHHANDER
