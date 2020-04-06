#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QErrorMessage>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initializeSerialPort();
    connect(serial,SIGNAL(readyRead()),this,SLOT(serialPortReceived()));
}

MainWindow::~MainWindow()
{
    serial->close();
    delete ui;
}

void MainWindow::initializeSerialPort()
{
    QThread *serialThread = new QThread;
    serial = new QSerialPort(this);
    //serial->moveToThread(serialThread);
    serial->setParent(NULL);
    serial->moveToThread(serialThread);
    serial->setPortName("COM15");
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    connect(serialThread,SIGNAL(finished()),serialThread,SLOT(deleteLater()));
    connect(serialThread,SIGNAL(finished()),serial,SLOT(deleteLater()));
    connect(serialThread,&QThread::started,[=]()->void{
    if(!serial->open(QIODevice::ReadWrite)) // possible race condition but shouldn't cause any problems at this stage
    {
        qDebug() << "cihaz açılmıyor";
        qDebug() << "error code = " << serial->error();
        qDebug() << "error string = " << serial->errorString();
        serialThread->quit();
    }else{
        qDebug() << "cihaz açıldı";
    }
    });
    serialThread->start();
    serial->write("written");
}

void MainWindow::serialPortReceived()
{
    QByteArray bytArrayData = serial->readAll();//serial->read(100);
    //QString stringData = QString::fromStdString(bytArrayData.toStdString());
    qDebug() << "bytArrayData  Result:  " << bytArrayData;
    qDebug() << "bytArrayData  Lenght:  " << bytArrayData.length();
    wholeArrayData.append(bytArrayData);
    qDebug() << "wholeArrayData  :  " << wholeArrayData;
      //QString new_value = QString().number(value, 16).prepend("0x");
    if(wholeArrayData.length()>1){
        if(wholeArrayData.at(0) != static_cast<char> (0xAF) || wholeArrayData.at(1) != static_cast<char> (0xAF) ){
            qDebug() << "Wrong Data Rejected" ;
            wholeArrayData = "";
            return;
        }
     //   qDebug() << "End ::" << wholeArrayData.end();

        if(wholeArrayData.data()[wholeArrayData.length()-1] == static_cast<char> (0xFA) )
        {
            if(wholeArrayData.data()[wholeArrayData.length()-2] == static_cast<char>(0xFA)) {
                qDebug() << "WholeData  Result:  " << wholeArrayData;

                if(checkSum(wholeArrayData))
                    qDebug() << "PAKET DOĞRU " ;
                else
                    qDebug() << "PAKET YANLIŞ " ;

                wholeArrayData = "";
            }
        }

    }
   serial->flush();
}


bool MainWindow::checkSum(QByteArray XORArray)
{
    qDebug() << "XOR size" <<XORArray.size();
    QByteArray arr;
    char key = XORArray[2];
    qDebug() << "key11 ::  " <<key;
    for (int i = 3; i < XORArray.size()-3; i++)
        key = key ^ XORArray[i];

    qDebug() << "key ::  " <<key;
    if(key == XORArray[XORArray.size()-3])
        return true ;
     else
        return false ;
}
