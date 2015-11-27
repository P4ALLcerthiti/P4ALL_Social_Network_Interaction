#ifndef SOCIALNETWORKINTERACTIONCLASS_H
#define SOCIALNETWORKINTERACTIONCLASS_H

#define QT3_SUPPORT
#include <math.h>
#include <string>
#include "mdichild.h"

class KPartiteParams
{
public:
    int MaxFRRuns;
    float dampenLast;
    float Dimension;
    float restDistance;
    float repulseConstant;
    float springConstant;
};

class SocialNetworkInteractionClass
{
public:
    SocialNetworkInteractionClass();
    ~SocialNetworkInteractionClass();

    void addFile(string inputPath);
    bool computePositions();
    void savePositionsToFile(string outputPath);
    bool computeAbstractPositions();
    void setKPartiteParamsValues(int MaxFRRuns = 300, float dampenLast = 0.65f, float Dimension = 3000.0f, float restDistance = 10.0f, float repulseConstant =-1.0, float springConstant=-1.0);
    void setMaxFRRuns(int MaxFRRuns=300) {if(MaxFRRuns > 0) this->kparams.MaxFRRuns = MaxFRRuns;}
    void setdampenLast(float dampenLast = 0.65f) {if(dampenLast > 0) this->kparams.dampenLast = dampenLast;}
    void setDimension(int Dimension=3000.0f) {if(Dimension > 0) this->kparams.Dimension = Dimension;}
    void setrestDistance(int restDistance= 10.0f) {if(restDistance > 0) this->kparams.restDistance = restDistance;}
    void setrepulseConstant(int repulseConstant=-1.0) {if(repulseConstant > 0) this->kparams.repulseConstant = repulseConstant;}
    void setspringConstant(int springConstant=-1.0) {if(springConstant > 0) this->kparams.springConstant = springConstant;}

private:
    bool readFilesFromVec();

private:
    vector<string> vec;
    MdiChild *ch;
    bool haveReadFiles;

public:
    KPartiteParams kparams;
};

#endif // SOCIALNETWORKINTERACTIONCLASS_H
