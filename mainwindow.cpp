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

//#include "mazdaroi.h"
//#include "mazdaroiio.h"

#include <tiffio.h>

#define PI 3.14159265

using namespace std;
using namespace boost::filesystem;
using namespace boost;
using namespace cv;

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//          Local Functions
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void ShowShape(Mat ImShow, int x,int y, int tileShape, int tileSize, int tileLineThickness, Scalar LineColor)
{
    if(ImShow.empty())
        return;
    switch (tileShape)
    {
    case 1:
        ellipse(ImShow, Point(x, y),
        Size(tileSize / 2, tileSize / 2), 0.0, 0.0, 360.0,
        LineColor, tileLineThickness);
        break;
    case 2:

    {
        int edgeLength = tileSize / 2;
        Point vertice0(x - edgeLength / 2, y - (int)((float)edgeLength * 0.8660254));
        Point vertice1(x + edgeLength - edgeLength / 2, y - (int)((float)edgeLength * 0.8660254));
        Point vertice2(x + edgeLength, y);
        Point vertice3(x + edgeLength - edgeLength / 2, y + (int)((float)edgeLength * 0.8660254));
        Point vertice4(x - edgeLength / 2, y + (int)((float)edgeLength * 0.8660254));
        Point vertice5(x - edgeLength, y);

        line(ImShow, vertice0, vertice1, LineColor, tileLineThickness);
        line(ImShow, vertice1, vertice2, LineColor, tileLineThickness);
        line(ImShow, vertice2, vertice3, LineColor, tileLineThickness);
        line(ImShow, vertice3, vertice4, LineColor, tileLineThickness);
        line(ImShow, vertice4, vertice5, LineColor, tileLineThickness);
        line(ImShow, vertice5, vertice0, LineColor, tileLineThickness);
    }
        break;
    default:
        rectangle(ImShow, Point(x - tileSize / 2, y - tileSize / 2),
            Point(x - tileSize / 2 + tileSize, y - tileSize / 2 + tileSize / 2),
            LineColor, tileLineThickness);
        break;
    }
}
//------------------------------------------------------------------------------------------------------------------------------
vector<int> GetDirFromFile(boost::filesystem::path FileToOpen, int firstDataLine = 1, int dirColumn = 3)
{
    vector<int> directionsVect;

    //check if file exists
    if (!exists(FileToOpen))
        return directionsVect;

    std::ifstream inFile(FileToOpen.string());
    if (!inFile.is_open())
    {
        return directionsVect;
    }
    //goto first dala line
    int lineNr = 0;
    std::string Line;
    while(inFile.good() && lineNr < firstDataLine)
    {
        getline(inFile, Line);
        lineNr++;
    }

//read directionalities
    while(inFile.good())
    {
        std::string Line2;
        lineNr++;
        if(lineNr == 363)
        {
            int a = 1;
            int b = a+1;
        }

        getline(inFile,Line2);

        if(Line2 == "")
            break;


        std::stringstream InStringStream(Line2);

        std::string subStr;

        int column = 0;

        while(InStringStream >> subStr && column < dirColumn)
        {

            column++;
        }
        InStringStream >> subStr;

        if(subStr == "NAN")
            directionsVect.push_back(-1000);
        else
            directionsVect.push_back(stoi(subStr));
    }
    inFile.close();
    return directionsVect;
}
//-----------------------------------------------------------------------------------------------------------
String GerTextStartingFromLine(boost::filesystem::path FileToOpen, int firstDataLine = 0)
{
    String OutStr;
    //check if file exists
    if(!exists( FileToOpen))
        return OutStr;

    std::ifstream inFile(FileToOpen.string());
    if(!inFile.is_open())
        return OutStr;

    //goto first dala line
    int lineNr = 0;
    std::string Line;
    while(inFile.good() && lineNr <= firstDataLine)
    {
        getline(inFile, Line);
        lineNr++;
    }

    //read lines
    while(inFile.good())
    {
        //std::string Line2;
        lineNr++;
        getline(inFile,Line);
        if(Line == "")
            break;
        OutStr += Line;
    }
    inFile.close();

    return OutStr;
}
//------------------------------------------------------------------------------------------------------------------------------
//          constructor Destructor
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->comboBoxDisplayRange->addItem("Manual Min - Max");
    ui->comboBoxDisplayRange->addItem("Image Min - Max");
    ui->comboBoxDisplayRange->addItem("Image Mean +/- STD");
    ui->comboBoxDisplayRange->addItem("Image 1% - 99%");
    ui->comboBoxDisplayRange->setCurrentIndex(2);

}
//------------------------------------------------------------------------------------------------------------------------------
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
    ui->textEditOut->clear();
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
        wregex FilePattern(ui->lineEditRegexDirectionFile->text().toStdWString());
        if (!regex_match(FileToProcess.path().filename().wstring(), FilePattern ))
            continue;
        path PathLocal = FileToProcess.path();
        if (!exists(PathLocal))
        {
            ui->textEditOut->append(QString::fromStdWString(PathLocal.filename().wstring()) + " File not exists" );
            break;
        }
        ui->listWidgetDirectionFiles->addItem(QString::fromStdWString(PathLocal.filename().wstring()));
    }
    ui->textEditOut->append(QString::number(ui->listWidgetDirectionFiles->count()) + " files was found)");
}
//------------------------------------------------------------------------------------------------------------------------------
void MainWindow::OpenImageFolder()
{
    ui->listWidgetImageFiles->clear();
    QString FolderQString = ui->lineEditImageFolder->text();
    path FolderPath = FolderQString.toStdWString();

    if(!exists(FolderPath))
    {
        ui->textEditOut->append("image folder: "+ FolderQString + "does not exists");
        return;
    }
    if (!is_directory(FolderPath))
    {
        ui->textEditOut->append("image folder: "+ FolderQString + "is not a directory");
        return;
    }
    int directionFilesCount = ui->listWidgetDirectionFiles->count();
    for (int i = 0; i < directionFilesCount; i++)
    {
        QString FileNameQStr = ui->listWidgetDirectionFiles->item(i)->text();
        path FileToOpen = FolderPath;
        FileToOpen.append(FileNameQStr.toStdWString());
        FileToOpen.replace_extension();
        //FileToOpen.append()
        wregex FilePattern(ui->lineEditRegexImageFile->text().toStdWString());
        if (!regex_match(FileToOpen.filename().wstring(), FilePattern ))
            continue;
        if (!exists(FileToOpen))
        {
            ui->textEditOut->append(QString::fromStdWString(FileToOpen.filename().wstring()) + "not exist" );
            break;
        }
        ui->listWidgetImageFiles->addItem(QString::fromStdWString(FileToOpen.filename().wstring()));
    }
    ui->textEditOut->append(QString::number(ui->listWidgetImageFiles->count()) + " files was found)");
}
//------------------------------------------------------------------------------------------------------------------------------
void MainWindow::ShowImage()
{
    Mat ImToShow;
    if(ImIn.empty())
    {
        ui->textEditOut->append("Empy Image");
        return;
    }
    double minIm = ui->doubleSpinBoxImMin->value();
    double maxIm = ui->doubleSpinBoxImMax->value();

    switch (ui->comboBoxDisplayRange->currentIndex())
    {
    case 1:
        NormParamsMinMax(ImIn, &maxIm, &minIm);
        break;
    case 2:
        NormParamsMeanP3Std(ImIn, &maxIm, &minIm);
        break;
    case 3:
        NormParams1to99perc(ImIn, &maxIm, &minIm);
        break;
    default:
        break;
    }

    double directionLineLenght = ui->doubleSpinBoxDirectionLineLenght->value();

    size_t numOfDirections = InFileParams.ParamsVect.size();

    bool showSudocolor = ui->checkBoxShowSudoColor->checkState();
    bool showShape = ui->checkBoxShowSape->checkState();
    bool showDirection = ui->checkBoxShowDirection->checkState();

    int tileLineWidth = ui->spinBoxShapeLineWidth->value();
    int directionLineWidth = ui->spinBoxDirectionLineWidth->value();

    if(showSudocolor)
        ImToShow = ShowImage16PseudoColor(ImIn,minIm,maxIm);
    else
        ImToShow = ShowImage16Gray(ImIn,minIm,maxIm);

    for(size_t i = 0; i < numOfDirections; i++)
    {
        int x  = InFileParams.ParamsVect[i].tileX;
        int y  = InFileParams.ParamsVect[i].tileY;
        double angle = InFileParams.ParamsVect[i].Params[0];

        int lineOffsetX = (int)round(directionLineLenght * sin(angle * PI / 180.0));
        int lineOffsetY = (int)round(directionLineLenght * cos(angle * PI / 180.0));

        if (angle >= -600.0 /* && Params.ParamsVect[i].Params[3] >= meanIntensityTreshold */ )
        {
            if(showShape)
            {
                if(showSudocolor)
                    ShowShape(ImToShow, x, y, InFileParams.tileShape, InFileParams.tileSize, tileLineWidth,Scalar(0.0, 0.0, 0.0, 0.0));
                else
                    ShowShape(ImToShow, x, y, InFileParams.tileShape, InFileParams.tileSize, tileLineWidth,Scalar(0.0, 0.0, 255.0, 0.0));
            }
            if(showDirection)
            {
                if(showSudocolor)
                    line(ImToShow, Point(x - lineOffsetX, y - lineOffsetY), Point(x + lineOffsetX, y + lineOffsetY), Scalar(0.0, 0.0, 0.0, 0.0), directionLineWidth);
                else
                    line(ImToShow, Point(x - lineOffsetX, y - lineOffsetY), Point(x + lineOffsetX, y + lineOffsetY), Scalar(0.0, 0.0, 255.0, 0.0), directionLineWidth);
            }
        }
    }






    double imageScale = pow(double(ui->spinBoxScaleBase->value()), double(ui->spinBoxScalePower->value()));
    if (imageScale !=1.0)
        cv::resize(ImToShow,ImToShow,Size(),imageScale,imageScale,INTER_NEAREST);
    imshow("Image",ImToShow);
}

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
//------------------------------------------------------------------------------------------------------------------------------
void MainWindow::on_lineEditDirectionFolder_returnPressed()
{
    OpenDirectionFolder();
    OpenImageFolder();
}
//------------------------------------------------------------------------------------------------------------------------------
void MainWindow::on_pushButtonOpenImageFolder_clicked()
{
    QFileDialog dialog(this, "Open Image Folder");
    dialog.setFileMode(QFileDialog::Directory);

    QString FolderQString = ui->lineEditImageFolder->text();
    path FolderPath;

    dialog.setDirectory(FolderQString);

    if(dialog.exec())
    {
        FolderQString = dialog.directory().path();
        FolderPath = FolderQString.toStdWString();
        if(!exists(FolderPath))
        {
            ui->textEditOut->append("image folder: "+ FolderQString + "does not exists");
            return;
        }
        if (!is_directory(FolderPath))
        {
            ui->textEditOut->append("image folder: "+ FolderQString + "is not a directory");
            return;
        }
    }
    else
        return;

    ui->lineEditImageFolder->setText(FolderQString);
    OpenImageFolder();
}
//------------------------------------------------------------------------------------------------------------------------------
void MainWindow::on_lineEditRegexImageFile_returnPressed()
{
    OpenImageFolder();
}
//------------------------------------------------------------------------------------------------------------------------------
void MainWindow::on_listWidgetDirectionFiles_currentTextChanged(const QString &currentText)
{
    QString FileNameQStr = currentText;

    QString DirectionsFolderQString = ui->lineEditDirectionFolder->text();
    path DirectionsFolderPath = DirectionsFolderQString.toStdWString();
    if(!exists(DirectionsFolderPath))
    {
        ui->textEditOut->append("image folder: "+ DirectionsFolderQString + "does not exists");
        return;
    }
    if (!is_directory(DirectionsFolderPath))
    {
        ui->textEditOut->append("image folder: "+ DirectionsFolderQString + "is not a directory");
        return;
    }
    path DirectionFileToOpen = DirectionsFolderPath;
    DirectionFileToOpen.append(FileNameQStr.toStdWString());
    InFileParams.GetFromFile(DirectionFileToOpen);




    QString ImagesFolderQString = ui->lineEditImageFolder->text();
    path ImagesFolderPath = ImagesFolderQString.toStdWString();
    if(!exists(ImagesFolderPath))
    {
        ui->textEditOut->append("image folder: "+ ImagesFolderQString + "does not exists");
        return;
    }
    if (!is_directory(ImagesFolderPath))
    {
        ui->textEditOut->append("image folder: "+ ImagesFolderQString + "is not a directory");
        return;
    }

    path ImageFileToOpen = ImagesFolderPath;
    ImageFileToOpen.append(FileNameQStr.toStdWString());
    ImageFileToOpen.replace_extension();

    ImIn = imread(ImageFileToOpen.string(),cv::IMREAD_ANYDEPTH);
    if(ImIn.empty())
    {
        ui->textEditOut->append("image : " + QString::fromStdString(ImageFileToOpen.string()) + "can not be opened");
    }

    ShowImage();
}
//------------------------------------------------------------------------------------------------------------------------------

void MainWindow::on_pushButtonSF_clicked()
{


    int size = ui->listWidgetImageFiles->count();// FileParVect.size();

    double rmse;
    double maxError = 0.0;
    int count = 0;
    int errorCount = 0;
    double sumSquareError = 0.0;

    double sumSquareError0 = 0.0;
    double sumSquareError2000 = 0.0;
    double sumSquareError4000 = 0.0;
    double sumSquareError6000 = 0.0;
    double sumSquareError8000 = 0.0;
    double sumSquareError3 = 0.0;
    double sumSquareError5 = 0.0;
    double sumSquareError7 = 0.0;
    double sumSquareError9 = 0.0;
    int count0 = 0;
    int count2000 = 0;
    int count4000 = 0;
    int count6000 = 0;
    int count8000 = 0;
    int count3 = 0;
    int count5 = 0;
    int count7 = 0;
    int count9 = 0;
    for(int i = 0; i< size; i++)
    {
        path ImageFilename(ui->lineEditImageFolder->text().toStdWString());
        path EstimatedDirectionFilename(ui->lineEditDirectionFolder->text().toStdWString());
        path TrueDirectionFilename(ui->lineEditImageFolder->text().toStdWString());

        ImageFilename.append(ui->listWidgetImageFiles->item(i)->text().toStdString());

        string LocalFileName = ImageFilename.stem().string();

        EstimatedDirectionFilename.append(LocalFileName + ".tiff.txt");
        TrueDirectionFilename.append(LocalFileName + "info.txt");

        vector<int> trueDirVect = GetDirFromFile(TrueDirectionFilename,1,2);
        vector<int> barWidthVect = GetDirFromFile(TrueDirectionFilename,1,3);
        vector<int> barFreqVect = GetDirFromFile(TrueDirectionFilename,1,4);
        vector<int> distortionVect = GetDirFromFile(TrueDirectionFilename,1,5);
        vector<int> estDirVect = GetDirFromFile(EstimatedDirectionFilename,37,1);


        if(trueDirVect.size() == estDirVect.size())
        {
            size_t vectSize = trueDirVect.size();
            for(size_t k = 0; k < vectSize; k++)
            {

                double difference;
                double trueDirection = trueDirVect[k] - 90;
                if (trueDirection < 0)
                    trueDirection += 180;
                double estDirection = estDirVect[k];
                if(trueDirection > estDirection)
                    difference = trueDirection - estDirection;
                else
                    difference = estDirection - trueDirection;

                if(difference > 90)
                    difference = 180 - difference;

                if(difference > 0)
                {
                    errorCount++;
                }

                if(maxError < difference)
                    maxError = difference;
                count++;
                sumSquareError += difference * difference;
                if(distortionVect[k] == 0)
                {
                    count0++;
                    sumSquareError0 += difference * difference;
                }
                if(distortionVect[k] == 2000)
                {
                    count2000++;
                    sumSquareError2000 += difference * difference;
                }
                if(distortionVect[k] == 4000)
                {
                    count4000++;
                    sumSquareError4000 += difference * difference;
                }
                if(distortionVect[k] == 6000)
                {
                    count6000++;
                    sumSquareError6000 += difference * difference;
                }
                if(distortionVect[k] == 8000)
                {
                    count8000++;
                    sumSquareError8000 += difference * difference;
                }
                if(distortionVect[k] == 3)
                {
                    count3++;
                    sumSquareError3 += difference * difference;
                }
                if(distortionVect[k] == 5)
                {
                    count5++;
                    sumSquareError5 += difference * difference;
                }
                if(distortionVect[k] == 7)
                {
                    count7++;
                    sumSquareError7 += difference * difference;
                }
                if(distortionVect[k] == 9)
                {
                    count9++;
                    sumSquareError9 += difference * difference;
                }
            }

        }

    }
    ui->textEditOut->append("size: " + QString::number(size));
    ui->textEditOut->append("count: " + QString::number(count));
    ui->textEditOut->append("max error: " + QString::number(maxError));
    ui->textEditOut->append("sum Sq Error: " + QString::number(sumSquareError));
    ui->textEditOut->append("RMSE: " + QString::number(sqrt(sumSquareError/count)));
    ui->textEditOut->append("errorCount: " + QString::number(errorCount));

    ui->textEditOut->append("count0: " + QString::number(count0));
    ui->textEditOut->append("RMSE0: " + QString::number(sqrt(sumSquareError0/count0)));
    ui->textEditOut->append("count2000: " + QString::number(count2000));
    ui->textEditOut->append("RMSE2000: " + QString::number(sqrt(sumSquareError2000/count2000)));
    ui->textEditOut->append("count4000: " + QString::number(count4000));
    ui->textEditOut->append("RMSE4000: " + QString::number(sqrt(sumSquareError4000/count4000)));
    ui->textEditOut->append("count6000: " + QString::number(count6000));
    ui->textEditOut->append("RMSE6000: " + QString::number(sqrt(sumSquareError6000/count6000)));
    ui->textEditOut->append("count8000: " + QString::number(count8000));
    ui->textEditOut->append("RMSE8000: " + QString::number(sqrt(sumSquareError8000/count8000)));
    ui->textEditOut->append("count3: " + QString::number(count3));
    ui->textEditOut->append("RMSE3: " + QString::number(sqrt(sumSquareError3/count3)));
    ui->textEditOut->append("count5: " + QString::number(count5));
    ui->textEditOut->append("RMSE5: " + QString::number(sqrt(sumSquareError5/count5)));
    ui->textEditOut->append("count7: " + QString::number(count7));
    ui->textEditOut->append("RMSE7: " + QString::number(sqrt(sumSquareError7/count7)));
    ui->textEditOut->append("count9: " + QString::number(count9));
    ui->textEditOut->append("RMSE9: " + QString::number(sqrt(sumSquareError9/count9)));





    //ui->textEditOut->append(QString::fromStdString(LocalFileName + " ->" + outStr + " ->" + to_string(dir)));
}

void MainWindow::on_checkBoxShowSudoColor_toggled(bool checked)
{
    ShowImage();
}

void MainWindow::on_comboBoxDisplayRange_currentIndexChanged(int index)
{
    ShowImage();
}

void MainWindow::on_doubleSpinBoxImMin_valueChanged(double arg1)
{
    ShowImage();
}

void MainWindow::on_doubleSpinBoxImMax_valueChanged(double arg1)
{
    ShowImage();
}

void MainWindow::on_doubleSpinBoxDirectionLineLenght_valueChanged(double arg1)
{
    ShowImage();
}

void MainWindow::on_spinBoxDirectionLineWidth_valueChanged(int arg1)
{
    ShowImage();
}

void MainWindow::on_spinBoxShapeLineWidth_valueChanged(int arg1)
{
    ShowImage();
}

void MainWindow::on_checkBoxShowSape_toggled(bool checked)
{
    ShowImage();
}

void MainWindow::on_checkBoxShowDirection_toggled(bool checked)
{
    ShowImage();
}

void MainWindow::on_spinBoxScalePower_valueChanged(int arg1)
{
    ShowImage();
}

void MainWindow::on_spinBoxScaleBase_valueChanged(int arg1)
{
    ShowImage();
}

void MainWindow::on_pushButtonSF2_clicked()
{
/*
    String OutStr;
    int size = ui->listWidgetDirectionFiles->count();// FileParVect.size();
    int count = 0;

    path EstimatedDirectionFilename(ui->lineEditDirectionFolder->text().toStdWString());

    ImageFilename.append(ui->listWidgetImageFiles->item(i)->text().toStdString());

    string LocalFileName = ImageFilename.stem().string();

    EstimatedDirectionFilename.append(LocalFileName + ".tiff.txt");
    TrueDirectionFilename.append(LocalFileName + "info.txt");




    for(int i = 0; i< size; i++)
    {
        path EstimatedDirectionFilename(ui->lineEditDirectionFolder->text().toStdWString());

        string LocalFileName = ImageFilename.stem().string();

        EstimatedDirectionFilename.append(LocalFileName + ".tiff.txt");

        vector<int> estDirVect = GetDirFromFile(EstimatedDirectionFilename,37,1);

        //check if file exists
        if(exists(EstimatedDirectionFilename))
        {
            std::ifstream inFile(FileToOpen.string());
            if (inFile.is_open())
            {
                //goto first dala line
                int lineNr = 0;
                std::string Line;
                while(inFile.good() && lineNr < firstDataLine)
                {
                    getline(inFile, Line);
                    lineNr++;
                }
            //read directionalities
                while(inFile.good())
                {
                    //std::string Line2;
                    lineNr++;
                    getline(inFile,Line);
                    if(Line2 == "")
                        break;
                    OutStr += Line;
                }
                inFile.close();

            }
            return directionsVect;
        }













        if(trueDirVect.size() == estDirVect.size())
        {
            size_t vectSize = trueDirVect.size();
            for(size_t k = 0; k < vectSize; k++)
            {

                double difference;
                double trueDirection = trueDirVect[k] - 90;
                if (trueDirection < 0)
                    trueDirection += 180;
                double estDirection = estDirVect[k];
                if(trueDirection > estDirection)
                    difference = trueDirection - estDirection;
                else
                    difference = estDirection - trueDirection;

                if(difference > 90)
                    difference = 180 - difference;

                if(difference > 0)
                {
                    errorCount++;
                }

                if(maxError < difference)
                    maxError = difference;
                count++;
                sumSquareError += difference * difference;
                if(distortionVect[k] == 0)
                {
                    count0++;
                    sumSquareError0 += difference * difference;
                }
                if(distortionVect[k] == 2000)
                {
                    count2000++;
                    sumSquareError2000 += difference * difference;
                }
                if(distortionVect[k] == 4000)
                {
                    count4000++;
                    sumSquareError4000 += difference * difference;
                }
                if(distortionVect[k] == 6000)
                {
                    count6000++;
                    sumSquareError6000 += difference * difference;
                }
                if(distortionVect[k] == 8000)
                {
                    count8000++;
                    sumSquareError8000 += difference * difference;
                }
                if(distortionVect[k] == 3)
                {
                    count3++;
                    sumSquareError3 += difference * difference;
                }
                if(distortionVect[k] == 5)
                {
                    count5++;
                    sumSquareError5 += difference * difference;
                }
                if(distortionVect[k] == 7)
                {
                    count7++;
                    sumSquareError7 += difference * difference;
                }
                if(distortionVect[k] == 9)
                {
                    count9++;
                    sumSquareError9 += difference * difference;
                }
            }

        }

    }
    ui->textEditOut->append("size: " + QString::number(size));
    ui->textEditOut->append("count: " + QString::number(count));
    ui->textEditOut->append("max error: " + QString::number(maxError));
    ui->textEditOut->append("sum Sq Error: " + QString::number(sumSquareError));
    ui->textEditOut->append("RMSE: " + QString::number(sqrt(sumSquareError/count)));
    ui->textEditOut->append("errorCount: " + QString::number(errorCount));

    ui->textEditOut->append("count0: " + QString::number(count0));
    ui->textEditOut->append("RMSE0: " + QString::number(sqrt(sumSquareError0/count0)));
    ui->textEditOut->append("count2000: " + QString::number(count2000));
    ui->textEditOut->append("RMSE2000: " + QString::number(sqrt(sumSquareError2000/count2000)));
    ui->textEditOut->append("count4000: " + QString::number(count4000));
    ui->textEditOut->append("RMSE4000: " + QString::number(sqrt(sumSquareError4000/count4000)));
    ui->textEditOut->append("count6000: " + QString::number(count6000));
    ui->textEditOut->append("RMSE6000: " + QString::number(sqrt(sumSquareError6000/count6000)));
    ui->textEditOut->append("count8000: " + QString::number(count8000));
    ui->textEditOut->append("RMSE8000: " + QString::number(sqrt(sumSquareError8000/count8000)));
    ui->textEditOut->append("count3: " + QString::number(count3));
    ui->textEditOut->append("RMSE3: " + QString::number(sqrt(sumSquareError3/count3)));
    ui->textEditOut->append("count5: " + QString::number(count5));
    ui->textEditOut->append("RMSE5: " + QString::number(sqrt(sumSquareError5/count5)));
    ui->textEditOut->append("count7: " + QString::number(count7));
    ui->textEditOut->append("RMSE7: " + QString::number(sqrt(sumSquareError7/count7)));
    ui->textEditOut->append("count9: " + QString::number(count9));
    ui->textEditOut->append("RMSE9: " + QString::number(sqrt(sumSquareError9/count9)));





    //ui->textEditOut->append(QString::fromStdString(LocalFileName + " ->" + outStr + " ->" + to_string(dir)));
*/
}
