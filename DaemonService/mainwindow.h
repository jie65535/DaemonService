#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QList<int> getInputPortList();

private slots:
    void on_pushButton_clicked();
    void log_append(QString msg);
    void on_btnClosePort_clicked();

    void on_BtnClear_clicked();

    void on_btnAddIP_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
