#include <QCoreApplication>
#include <string>
#include <fstream>
#include <iostream>
#include "mdichild.h"
#include "socialnetworkinteractionclass.h"
#include <windows.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include "mdichild.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SocialNetworkInteractionClass *obj = new SocialNetworkInteractionClass();
    obj->addFile("C:\\TwitterData\\testFileFollowers.csv");
    obj->addFile("C:\\TwitterData\\testFileFollowing.csv");
//  obj->addFile("C:\\TwitterData\\test1.csv");
//  obj->addFile("C:\\TwitterData\\test2.csv");

    obj->computePositions();
    obj->savePositionsToFile("C:\\TwitterData\\testFileCommon.pos");
    obj->computeAbstractPositions();
    obj->savePositionsToFile("C:\\TwitterData\\testFileAbstract.pos");

    return a.exec();
}




