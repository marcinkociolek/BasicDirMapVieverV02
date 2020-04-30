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

    void on_checkBoxShowSudoColor_toggled(bool checked);

    void on_comboBoxDisplayRange_currentIndexChanged(int index);

    void on_doubleSpinBoxImMin_valueChanged(double arg1);

    void on_doubleSpinBoxImMax_valueChanged(double arg1);

    void on_doubleSpinBoxDirectionLineLenght_valueChanged(double arg1);

    void on_spinBoxDirectionLineWidth_valueChanged(int arg1);

    void on_spinBoxShapeLineWidth_valueChanged(int arg1);

    void on_checkBoxShowSape_toggled(bool checked);

    void on_checkBoxShowDirection_toggled(bool checked);

    void on_spinBoxScalePower_valueChanged(int arg1);

    void on_spinBoxScaleBase_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
