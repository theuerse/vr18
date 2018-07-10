#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QColorDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "clickablelabel.h"
#include "sqlitedb.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_debugButton_clicked();
    void on_ClickableLabel_clicked();
    void on_hsvToleranceSlider_valueChanged(int value);
    void on_adjacentFramesButton_clicked();
    void on_sendSelectedFrameButton_clicked();
    void on_synSetFilterButton_clicked();
    void on_CNNsearchButton_clicked();
    void on_colorSearchButton_clicked();
    void on_edgeSearchButton_clicked();
    void on_keypointSearchButton_clicked();

    void on_ipAddressLineEdit_textEdited(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QGridLayout *gridLayout;
    QHBoxLayout *hBoxlayout;
    QColorDialog *colorPicker;
    ClickableLabel *lastSelectedImage = nullptr;
    SqliteDb *dataBase;
    QNetworkAccessManager *networkManager;
    QNetworkRequest networkRequest;
    QString urlIp;
    QString urlParams;

    int gridLayoutCounter_row;
    int gridLayoutCounter_col;
    int gridLayoutMaxCols;
    QString shotpath;
    QString synsetPath;
    QStringList synset;
    int imageHeight;
    int imageWidth;


    /**
     * @brief Puts an image it on the gridview to be displayed and manages gridLayout's index overflows
     * @param img
     * @todo override QWidget::resizeEvent() to get correct gridLayoutMaxCols after resize.
     */
    void displayImage(imgstruct imageInfo);

    /**
     * @brief Puts an image on the videobar
     * @param imageInfo
     */
    void displayVideoImage(imgstruct imageInfo);

    /**
     * @brief Deletes all images that are currently displayed on the gridLayout
     * @note Does nothing if there are no images in gridLayout
     */
    void deleteDisplayedImages();

    /**
     * @brief Deletes all images that are currently displayed on the hboxLayout
     * @note Does nothing if there are no images in hboxLayout
     */
    void deleteDisplayedVideoImages();

    /**
     * @brief Creates,sets up and configures color picker on its place UI window
     */
    void setupColorPicker();

    /**
     * @brief reads entries from synnet.txt file and save them into a data structure
     */
    void readSynSet();
};

#endif // MAINWINDOW_H
