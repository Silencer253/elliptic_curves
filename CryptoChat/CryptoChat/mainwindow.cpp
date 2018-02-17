#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
#include "echoclient.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Валидация IP адреса
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegex ("^" + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange + "$");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
    ui->lineEdit_IP->setValidator(ipValidator);

    // Валидация порта
    QString portRange = "(?:([0-9]{1,4}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5]))";
    QRegExp portRegex ("^" + portRange + "$");
    QRegExpValidator *portValidator = new QRegExpValidator(portRegex, this);
    ui->lineEdit_Port->setValidator(portValidator);

    // Инициализация по-умолчанию
    ui->lineEdit_IP->setText("127.0.0.1");
    ui->lineEdit_Port->setText("6666");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_Connect_clicked()
{

}

void MainWindow::on_button_Listen_clicked()
{
    QString IP = ui->lineEdit_IP->text();
    const ushort port = ui->lineEdit_Port->text().toUShort();
    server *MyServer = new server(IP, port);
}
