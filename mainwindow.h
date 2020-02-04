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

    void OpenDirectionFolder();
    void OpenImageFolder();

private slots:
    void on_pushButtonOpenDirectionFolder_clicked();

    void on_lineEditRegexDirectionFile_returnPressed();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
