#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QDebug>
#include <QThread>
#include <QCryptographicHash>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QByteArray fileChecksum( QCryptographicHash::Algorithm hashAlgorithm);
   // QString wholeData = "";
     bool checkSum(QByteArray);
     QByteArray wholeArrayData ;

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;

    void initializeSerialPort();
private slots:
    void serialPortReceived();
};
#endif // MAINWINDOW_H
