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
#include "socialnetworkinteractionclass.h"
#include "mdichild.h"
#include <windows.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>

SocialNetworkInteractionClass::SocialNetworkInteractionClass()
{
    mainWin=new MainWindow("","","","","","","fakelosdynamic","","");

    ch = mainWin->createMdiChild();
}



SocialNetworkInteractionClass::~SocialNetworkInteractionClass()
{
    vec.clear();

    if( mainWin!=NULL)
    {
        delete mainWin;
        mainWin = NULL;
    }
    if(ch!=NULL)
    {
        delete ch;
        ch = NULL;
    }

}

void SocialNetworkInteractionClass::addFile(string inputPath)
{
    vec.push_back(inputPath);
}

void SocialNetworkInteractionClass::computePositions(float dampenLast, float Dimension, float restDistance, float epsilon, float repulseConstant, float springConstant)
{
    for(int i=0; i<vec.size();i++)
    {
        vector<string> filenames;
        filenames.push_back(vec[i]);
        ch->readFile(filenames);
    }

    for (int i = 0; i < 60; i++)
    {

        ch->setPositions( dampenLast, Dimension,restDistance,epsilon,repulseConstant,springConstant);
    }
}


void SocialNetworkInteractionClass::savePositionsToFile(QString outputPath)
{
    ch->writeVertexPositionstoFile2(outputPath);
}

void SocialNetworkInteractionClass::computeAbstractPositions(float dampenLast, float Dimension, float restDistance, float epsilon, float repulseConstant, float springConstant){
    if (ch){
        ch->toggleLayoutView(true);
    }
    for (int i = 0; i < 60; i++)
    {
        ch->setPositions();
    }
}

