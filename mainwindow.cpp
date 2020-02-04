#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

#include <string>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>


#include <math.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "NormalizationLib.h"
#include "DispLib.h"
#include "histograms.h"
#include "RegionU16Lib.h"
#include "StringFcLib.h"

#include "mazdaroi.h"
#include "mazdaroiio.h"

#include <tiffio.h>

using namespace std;
using namespace boost::filesystem;
using namespace boost;
using namespace cv;

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//          Local Functions
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//          constructor Destructor
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//          CLASS FUNCTIONS
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void MainWindow::OpenDirectionFolder()
{
    ui->listWidgetDirectionFiles->clear();
    QString FolderQString = ui->lineEditDirectionFolder->text();
    path FolderPath = FolderQString.toStdWString();

    if(!exists(FolderPath))
    {
        ui->textEditOut->append("direction folder: "+ FolderQString + "does not exists");
        return;
    }
    if (!is_directory(FolderPath))
    {
        ui->textEditOut->append("direction folder: "+ FolderQString + "is not a directory");
        return;
    }

    for (directory_entry& FileToProcess : directory_iterator(FolderPath))
    {
        regex FilePattern(ui->lineEditRegexDirectionFile->text().toStdString());
        if (!regex_match(FileToProcess.path().filename().string().c_str(), FilePattern ))
            continue;
        path PathLocal = FileToProcess.path();
        if (!exists(PathLocal))
        {
            ui->textEditOut->append(QString::fromStdString(PathLocal.filename().string() + " File not exists" ));
            break;
        }
        ui->listWidgetDirectionFiles->addItem(PathLocal.filename().string().c_str());
    }
    ui->textEditOut->append(QString::number(ui->listWidgetDirectionFiles->count()) + " files was found)");

}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//          Slots
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void MainWindow::on_pushButtonOpenDirectionFolder_clicked()
{
    QFileDialog dialog(this, "Open Direction Folder");
    dialog.setFileMode(QFileDialog::Directory);

    QString FolderQString = ui->lineEditDirectionFolder->text();
    path FolderPath;

    dialog.setDirectory(FolderQString);

    if(dialog.exec())
    {
        FolderQString = dialog.directory().path();
        FolderPath = FolderQString.toStdWString();
        if(!exists(FolderPath))
        {
            ui->textEditOut->append("direction folder: "+ FolderQString + "does not exists");
            return;
        }
        if (!is_directory(FolderPath))
        {
            ui->textEditOut->append("direction folder: "+ FolderQString + "is not a directory");
            return;
        }
    }
    else
        return;

    ui->lineEditDirectionFolder->setText(FolderQString);
    OpenDirectionFolder();
}
//------------------------------------------------------------------------------------------------------------------------------

void MainWindow::on_lineEditRegexDirectionFile_returnPressed()
{
    OpenDirectionFolder();
}
