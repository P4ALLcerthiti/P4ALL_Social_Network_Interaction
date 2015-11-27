#define QT3_SUPPORT
#include <math.h>
#include <string>
#include <fstream>
#include <iostream>
#include "socialnetworkinteractionclass.h"
#include <windows.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <QDir>
#include <cstddef>

SocialNetworkInteractionClass::SocialNetworkInteractionClass()
{
    this->kparams.MaxFRRuns = 300;
    this->kparams.dampenLast = 0.65f;
    this->kparams.Dimension= 3000.0f;
    this->kparams.restDistance= 10.0f;
    this->kparams.repulseConstant = -1.0;
    this->kparams.springConstant = -1.0;
    this->haveReadFiles = false;

    this->ch = new MdiChild();
}


SocialNetworkInteractionClass::~SocialNetworkInteractionClass()
{
    vec.clear();

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


bool SocialNetworkInteractionClass::computePositions()
{

    if(readFilesFromVec())
    {
        for (int i = 0; i < this->kparams.MaxFRRuns; i++)
        {
            ch->setPositions(this->kparams.MaxFRRuns, this->kparams.dampenLast, this->kparams.Dimension, this->kparams.restDistance, this->kparams.repulseConstant, this->kparams.springConstant);
        }
        return true;
    }
    else
    {
        return false;
    }
}


void SocialNetworkInteractionClass::savePositionsToFile(string outputPath)
{
    size_t found = outputPath.find_last_of("/\\");
    QString outPath = QString::fromStdString((outputPath.substr(0,found))) ;


    QDir dir(outPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    ch->writeVertexPositionstoFile2(QString::fromStdString(outputPath));
}

bool SocialNetworkInteractionClass::computeAbstractPositions(){

    if(readFilesFromVec())
    {
        ch->toggleLayoutView(true);

        for (int i = 0; i < this->kparams.MaxFRRuns; i++)
        {
            ch->setPositions(this->kparams.MaxFRRuns, this->kparams.dampenLast, this->kparams.Dimension, this->kparams.restDistance, this->kparams.repulseConstant, this->kparams.springConstant);
        }
        return true;
    }
    else
    {
        return false;
    }

}

bool SocialNetworkInteractionClass::readFilesFromVec()
{
    if(!vec.size()==0)
    {
        if(!this->haveReadFiles)
        {
            for(int i=0; i<vec.size();i++)
            {
                vector<string> filenames;
                filenames.push_back(vec[i]);
                ch->readFile(filenames);
            }

            this->haveReadFiles = true;
        }
    }
    else
    {
        fprintf(stderr, "No files were added...\n");fflush(stderr);fflush(stderr);
        return false;
    }
    return true;
}

void SocialNetworkInteractionClass::setKPartiteParamsValues(int MaxFRRuns, float dampenLast, float Dimension, float restDistance, float repulseConstant, float springConstant)
{
    if(MaxFRRuns > 0) this->kparams.MaxFRRuns = MaxFRRuns;
    if(dampenLast>=0) this->kparams.dampenLast = dampenLast;
    if(Dimension>0) this->kparams.Dimension= Dimension;
    if(restDistance>=0)this->kparams.restDistance= restDistance;
    this->kparams.repulseConstant = repulseConstant;
    this->kparams.springConstant = springConstant;
}


