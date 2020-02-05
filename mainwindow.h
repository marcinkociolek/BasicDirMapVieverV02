#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <opencv2/core/core.hpp>

#include "tileparams.h"
#include "dirdetectionparams.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    cv::Mat ImIn;
    FileParams InFileParams;

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void OpenDirectionFolder();
    void OpenImageFolder();
    void ShowImage();

private slots:
    void on_pushButtonOpenDirectionFolder_clicked();

    void on_lineEditRegexDirectionFile_returnPressed();

    void on_lineEditDirectionFolder_returnPressed();

    void on_pushButtonOpenImageFolder_clicked();

    void on_lineEditRegexImageFile_returnPressed();

    void on_listWidgetDirectionFiles_currentTextChanged(const QString &currentText);

    void on_pushButtonSF_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
