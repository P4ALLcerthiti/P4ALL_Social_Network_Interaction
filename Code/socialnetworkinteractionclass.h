#ifndef SOCIALNETWORKINTERACTIONCLASS_H
#define SOCIALNETWORKINTERACTIONCLASS_H

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


#include "mdichild.h"


class SocialNetworkInteractionClass
{
public:
    SocialNetworkInteractionClass();
    ~SocialNetworkInteractionClass();

    void addFile(string inputPath);
    void computePositions(float dampenLast = 0.65f, float Dimension = 3000.0f, float restDistance = 10.0f, float epsilon = 0.0000001f, float repulseConstant =-1.0, float springConstant=-1.0);
    void savePositionsToFile(QString outputPath);
    void computeAbstractPositions(float dampenLast = 0.65f, float Dimension = 3000.0f, float restDistance = 10.0f, float epsilon = 0.0000001f, float repulseConstant =-1.0, float springConstant=-1.0);

private:
    vector<string> vec;
    MdiChild *ch;
    MainWindow *mainWin;
};

#endif // SOCIALNETWORKINTERACTIONCLASS_H
