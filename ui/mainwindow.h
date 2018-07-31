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
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void on_action_Quit_triggered();
    void on_action_Directory_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
