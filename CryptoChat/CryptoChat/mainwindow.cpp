#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegex ("^" + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange + "$");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
    ui->lineEdit_IP->setValidator(ipValidator);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_Connect_clicked()
{
    // Сначала заблокировать кнопку Listen
    //ui->button_Listen->setDisabled(1);

    QString IP = ui->lineEdit_IP->text();

}
