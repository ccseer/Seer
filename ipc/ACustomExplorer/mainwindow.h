#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qt_windows.h>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // QWidget interface
protected:
    virtual void changeEvent(QEvent *) override;
    virtual bool nativeEvent(const QByteArray &e, void *m, long *result) override;

private:
    void onCopyDataFromSeer();
    QString getSelectedFilePath() const;
    void initSeer();
    void initView();

    Ui::MainWindow *ui;
};
#endif  // MAINWINDOW_H
