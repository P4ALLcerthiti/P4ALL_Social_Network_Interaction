//#include <QtGui/QApplication>
#define QT3_SUPPORT
#include <QApplication>
#include <QDialog>
#include <QMainWindow>
#include <QSplashScreen>
#include "mainwindow.h"
#include <math.h>
#include <qapplication.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxy>
#include "teststauros2.h"
#include <string>
#include <fstream>
#include <iostream>

#include "mdichild.h"
#include "socialnetworkinteractionclass.h"
#include <windows.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>


using namespace std;

void slotRequestFinished(QNetworkReply *reply);
int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    float dampenLast = 0.65f;
    float Dimension= 3000.0f;
    float restDistance= 10.0f;
    float epsilon= 0.0000001f;

    SocialNetworkInteractionClass *obj = new SocialNetworkInteractionClass();
    obj->addFile("C:\\TwitterData\\testFileFollowers.csv");
    obj->addFile("C:\\TwitterData\\testFileFollowing.csv");


    obj->computePositions(dampenLast,Dimension,restDistance,epsilon);
    obj->savePositionsToFile("C:\\TwitterData\\testFileCommon.pos");
    obj->computeAbstractPositions(dampenLast,Dimension,restDistance,epsilon);
    obj->savePositionsToFile("C:\\TwitterData\\testFileAbstract.pos");


    return app.exec();
}
