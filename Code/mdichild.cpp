
//VIS-SENSE includes
#include "vsCSVFile.h"
#include "vsGraph.h"
#include "vsFeature.h"
#include "vsNode.h"
//VIS-SENSE Qt includes
#include "mdichild.h"
#include "glwidget.h"
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QUrl>
#include <QtCore/QTime>
#include <QtCore/QFile>
#include <QtCore/QProcess>
//Other incldues
#include "omp.h"
//#include "dirent.h"
#include <fstream>

MdiChild::MdiChild(){
    //this->parentmdichild=parentmdichild;
	//isUntitled = true;
	this->DockWin = NULL;
	this->NoDockWins = 0;

	this->Min.x = this->Min.y = this->Min.z = 0;
	this->Min.x = this->Min.y = this->Min.z = 0;

	this->csvList = NULL;
	this->csvListSize = 0;

	this->LastScreenSize.x = 0;
	this->LastScreenSize.y = 0;

    this->FRRuns = 0;//frlayout runs
    //this->MaxFRRuns = 300;
	this->FRRunning = false;

	this->SFDialog = NULL;
	this->graphFromClustersDialog = NULL;

    this->Graph = NULL;
    this->DrawingFeature =0;
    this->displayGraph =NULL;
    this->setNewGraph = NULL;

    this->Vertex = NULL;
    this->NoDrawingVertices =0;
    this->NoDrawingEvents =0;
    this->NoDrawingFeatureValues=0;
    this->NoShownVertices=0;

    this->Edge = NULL;
    this->NoEdges = 0;

    this->CuttingThreshold = -3.0f;
    this->edgeStatistics = NULL;

    abstractView = false;

    if (this->DrawingFeature == 0)
        //this->Layout = Geometric;
        this->Layout = FR;
    else
        this->Layout = FromFile;
    //this->Layout = Random;
    this->PositionsSet = false;

	fprintf(stderr, "Checking Consistency ... ");fflush(stderr);fflush(stderr);
	this->checkConsistency();
	fprintf(stderr, "OK\n");fflush(stderr);fflush(stderr);

//	matp_has_to_be_inserted=false;
//	matp_inserted=false;

}

void MdiChild::readFile(vector<string> files,std::vector<int> numberof_time_periods,std::string name_of_feature){

	vsCSVFile* CSVFile = new vsCSVFile(files,numberof_time_periods,name_of_feature); //CSVFile->printSelf();
	this->addToCSVList(CSVFile);

}

void MdiChild::checkConsistency(){
    if (this->setNewGraph){
        this->displayGraph = this->setNewGraph;
    }

    if (!this->displayGraph)
        return;

    int NoVertices = 0;

    switch (this->DrawingFeature){
        case 0:
            NoVertices = this->displayGraph->getNoVertices();
        break;
        default:
            NoVertices = this->displayGraph->getNoVertices(0) + this->displayGraph->getNoVertices(this->DrawingFeature);
    }

    if (NoVertices != this->NoDrawingVertices || this->setNewGraph){
        this->addDrawingVertices(NoVertices);

        if ( !this->abstractView){
            this->setDrawingVertices();
        }

        this->setDrawnVertices();
        this->RandomLayout();
        this->PositionsSet = false;
        this->FRRuns = 0;
        if (this->DrawingFeature != 0 && !this->abstractView)
            this->Layout = FromFile;
    }
//    if (this->DrawingFeature == 0)
//        for (int i = this->NoDockWins; i < this->displayGraph->getNoFeatures() - 1; ++i)
//            this->createDockWin();
    this->setNewGraph = NULL;
    fprintf(stderr, "New Graph Set\n");fflush(stderr);fflush(stderr);
}

void MdiChild::addToCSVList(vsCSVFile *f){

    this->csvList = (vsCSVFile**) realloc(this->csvList, (this->csvListSize + 1) * sizeof(vsCSVFile*));
    if (!this->csvList){
        fprintf(stderr, "Cannot allocate memory for array CSVList. Exiting ...\n");fflush(stderr);fflush(stderr);
        exit(9);
    }

    this->csvList[this->csvListSize] = f;
    this->csvListSize++;
}

void MdiChild::addDrawingVertices(int N){
    //fprintf(stderr, "\n\n\n\n Feature: %d\n\n\n", this->DrawingFeature);fflush(stderr);fflush(stderr);

    this->Vertex = (vsDrawingVertex*) realloc(this->Vertex, N * sizeof(vsDrawingVertex));
    if ( !this->Vertex ){
        fprintf(stderr, "Cannot allocate memory for array Vertex. Exiting ...\n");fflush(stderr);fflush(stderr);
        exit(9);
     }

    this->NoDrawingVertices = N;
    this->NoDrawingEvents = this->displayGraph->getNoVertices(0);
    this->NoDrawingFeatureValues = N - this->NoDrawingEvents;


    if (this->DrawingFeature == 0){
        for (int i = 0; i < N; i++){
            this->Vertex[i].initialize(i, this->displayGraph->Vertex[i]);
            this->displayGraph->Vertex[i]->DV = (&this->Vertex[i]);
        }
    }
    else{
        int i = 0;
        int f = this->DrawingFeature;
        for (int k = 0; k < this->NoDrawingEvents; ++k, ++i){
            this->Vertex[i].initialize(i, this->displayGraph->Feature[0]->Vertex[k]);
            this->displayGraph->Feature[0]->Vertex[k]->DV = (&this->Vertex[i]);
        }
        for (int k = 0; k < this->NoDrawingFeatureValues; ++k, ++i){
            this->Vertex[i].initialize(i, this->displayGraph->Feature[f]->Vertex[k]);
            this->displayGraph->Feature[f]->Vertex[k]->DV = &(this->Vertex[i]);
        }

    }

    free(this->Edge);
    this->Edge = NULL;
    this->NoEdges = 0;

    for (int i = this->NoDrawingEvents; i < this->NoDrawingVertices; ++i){
        for (int j = 0; j < this->Vertex[i].Map->NoEdges; ++j){
            if ( !(this->Vertex[i].Map->Edge[j]->getSource()->Feature == this->Vertex[i].Map->Edge[j]->getTarget()->Feature || this->Vertex[i].Map->Edge[j]->getTarget()->Feature->getID() == 0) && this->DrawingFeature != 0)
            continue;

            this->NoEdges++;
            this->Edge = (vsDrawingEdge*) realloc(this->Edge, this->NoEdges * sizeof(vsDrawingEdge));
            if ( !this->Edge ){
                fprintf(stderr, "Cannot allocate memory for array Edge. Exiting ...\n");fflush(stderr);fflush(stderr);
                exit(9);
            }
            vsDrawingVertex* s = this->Vertex[i].Map->Edge[j]->getSource()->DV;
            vsDrawingVertex* t = this->Vertex[i].Map->Edge[j]->getTarget()->DV;
            this->Edge[this->NoEdges - 1].initialize(s, t);
        }
    }
}

void MdiChild::setDrawingVertices(){
    if (this->DrawingFeature == 0){
        this->NoShownVertices = this->NoDrawingVertices;
        return;
    }

    vsFeature *curFeature = this->displayGraph->Feature[0];
    vsNode *curVertex;

    for (int i = 0; i < curFeature->NoVertices; i++){
        curVertex = curFeature->Vertex[i];
        this->Vertex[i].Draw = false;

        for (int j = 0; j < curVertex->NoEdges; j++){
            if (curVertex->Edge[j]->getTarget()->Feature->getID() == this->DrawingFeature){
                this->Vertex[i].Draw = true;
                break;
            }
            //fprintf(stderr, "%d: %d ->> %d\n", i, curVertex->Edge[j]->getTarget()->Feature->getID(), this->DrawingFeature);fflush(stderr);fflush(stderr);
        }
    }

    for (int i = 0; i < this->NoDrawingVertices; i++)
        if (this->Vertex[i].Draw)
            this->NoShownVertices++;

    fprintf(stderr, "%d %d\n", this->NoShownVertices, this->NoDrawingVertices);fflush(stderr);fflush(stderr);
}

void MdiChild::setDrawnVertices(){
    if (this->DrawingFeature != 0){
        if (this->abstractView){

            this->setAbstractDrawingVertices(false);
        }
        return;
    }

    int f1, f2;

    for (int i = 0; i < this->NoEdges; ++i){
        f1 = this->Edge[i].Source->Map->Feature->getID() - 1;
        f2 = this->Edge[i].Target->Map->Feature->getID() - 1;
        //fprintf(stderr, "%d %d %d\n", this->NoDockWins, f1, f2);fflush(stderr);fflush(stderr);

        if (f1 >= this->NoDockWins || f2>= this->NoDockWins)
            continue;

        if (f1 >= 0 || f2 >= 0){
            //this->Edge[i].Source->Draw = false;
            //this->Edge[i].Target->Draw = false;
            this->Edge[i].Draw = false;
        }
        else
            this->Edge[i].Draw = true;

    }

    if (this->abstractView){
        this->calculateEdgeMeanSD(true);
        this->setAbstractDrawingVertices(true);
    }
    else{
        for (int i = 0; i < this->NoDrawingVertices; ++i)
            this->Vertex[i].Draw = false;

        for (int i = 0; i < this->NoEdges; ++i){
            if (this->Edge[i].Draw){
                this->Edge[i].Source->Draw = true;
                this->Edge[i].Target->Draw = true;
            }
        }
    }
}

void MdiChild::RandomLayout(){

    srand(1268);

    for (int i = 0; i < this->NoDrawingVertices; i++){
        float RN1 = rand() / static_cast<float>( RAND_MAX - 1);
        float RN2 = rand() / static_cast<float>( RAND_MAX - 1);
        this->Vertex[i].Pos.x = 3000 * RN1;
        this->Vertex[i].Pos.y = 3000 * RN2;
    }
}

bool MdiChild::setPositions(int MaxFRRuns, float dampenLast, float Dimension, float restDistance, float repulseConstant, float springConstant){

    if (this->csvListSize != 0 || this->setNewGraph){
//        fprintf(stderr,"%d\n",this->csvListSize);fflush(stderr);fflush(stderr);
        this->checkExistenceOfNewFiles();
        this->checkConsistency();
    }

    if (!this->Graph){
        return false;
    }

    switch (this->Layout){
        case 0:
            this->RandomLayout();
            //fprintf(stderr, "%d\n", this->Layout);fflush(stderr);fflush(stderr);
            this->PositionsSet = true;
        break;
        case 1:
            //this->GeometricLayout();
            this->PositionsSet = true;
        break;
        case 2:
            if (!this->FRRunning){
//                int N = 5;
//                if (this->DrawingFeature != 0)
//                    N = 5;


//                for (int i = 0; i < N; i++)
                    if (this->FRRuns < MaxFRRuns)
                        this->FRLayout(MaxFRRuns, dampenLast,  Dimension,  restDistance, repulseConstant, springConstant);
            }
            if (this->FRRuns >= MaxFRRuns){
                this->PositionsSet = true;
//                if (this->writePositionsToFile && this->abstractView)
//                    this->writeVertexPositionstoFile2("C:\\TwitterData\\testFileAbstractGui.pos");
//                if (this->writePositionsToFile && !this->abstractView)
//                    this->writeVertexPositionstoFile();
            }

            /*this->PositionsSet = true;
            if (this->writePositionsToFile && !this->abstractView)
                this->writeVertexPositionstoFile();*/
            fprintf(stderr, "%d\n", this->FRRuns);fflush(stderr);fflush(stderr);
        break;
        case 3:
            //this->RandomLayout();
            //if ( !this->loadVertexPositionsFile() )
            //    this->Layout = FR;
        break;
        default:
            this->RandomLayout();
            this->PositionsSet = true;
        break;
    }

    fprintf(stderr, "Physical Positions Set \n");fflush(stderr);fflush(stderr);

    return true;
}

void MdiChild::checkExistenceOfNewFiles(){

    if (this->csvListSize == 0)
        return;

    if (!this->Graph){
        this->Graph = new vsGraph;
        this->displayGraph = this->Graph;
    }

    for (int i = 0; i < this->csvListSize; i++){
        if (strcmp(this->csvList[i]->FileName->Extension, "csv") == 0){
            this->Graph->createBipartiteFromCSVFile(this->csvList[i]); //graph->printSelf();
            this->Graph->createAbstractGraph();
            if (this->abstractView){

            }
//            this->createDockWin();
        }
//        else if (strcmp(this->csvList[i]->FileName->Extension, "clu") == 0){
//            this->Graph->importClusteringReasultsFromCSVFile(this->csvList[i]);
//            this->hasClusteringResult = true;
//            this->toggleClustersView(true);
//        }
//        else if (strcmp(this->csvList[i]->FileName->Extension, "pos") == 0){
//            if (this->readVertexPositionsfromFile(this->csvList[i])){
//                this->FRRuns = MaxFRRuns;
//                this->PositionsSet = true;
//                this->setDrawingPositions();
//            }
//            this->Layout = FR;
//        }

        delete this->csvList[i];
    }
    free(this->csvList);
    this->csvList = NULL;
    this->csvListSize = 0;


}

void MdiChild::FRLayout(int MaxFRRuns, float dampenLast, float Dimension, float restDistance, float repulseConstant, float springConstant)
{


    fprintf(stderr, "FR Started (%d)...", this->NoShownVertices);fflush(stderr);
/*
    if (this->DrawingFeature == 0 && this->NoDockWins == 1){
        for (int i = 0; i < this->DockWin[0]->NoDrawingVertices; ++i){
            this->Vertex[i].Pos.x =  this->DockWin[0]->Vertex[i].Pos.x;
            this->Vertex[i].Pos.y =  this->DockWin[0]->Vertex[i].Pos.y;
        }
        return;
    }
*/
    this->FRRunning = true;

    int numVerts = this->NoDrawingVertices;

    //float dampenLast = 0.65f;
    //float dampenLast = dampenLast;

    //float Dimension = 3000.0f;
    //float Dimension = Dimension;
    if (Dimension < 0)
    {
        Dimension = 3000.0f;
    }
    float area = Dimension * Dimension;
    float k = sqrt(area/numVerts);

    if (repulseConstant  < 0)
    {
        repulseConstant = pow(k, 2);
    }

    if(springConstant < 0)
    {
        springConstant = 1.0/k;
    }
    //float repulseConstant = pow(k, 2);
    //float springConstant = 1.0/k;
    //float restDistance = 10.0f;

    float epsilon = 0.0000001f;
    float minD = 100 * pow(k, 2);
    minD /= 5;

    //float T = pow((300 - FRRuns)/float(301), 2) * numVerts (10 + 0.5*FRRuns);
    //float T = sqrt(k) * (300 - FRRuns)/float(301);
    //float T = k * pow((300 - FRRuns)/float(301), 2);
    float FRRatio = (MaxFRRuns - this->FRRuns) / (float)(MaxFRRuns + 1);
    float T = k * FRRatio;
    //float T = 1000;
    //fprintf(stderr, "T: %f\n", T);fflush(stderr);fflush(stderr);

    int i, j, l;
    float deltaX, deltaY, distSquared, dist, prod, force, w;
    float Fo;
    float first, second;
    float a, b;
    vsPoint **Force;
    int tid, nthreads;
    int chunk = 10;

#pragma omp parallel
{
    nthreads = omp_get_num_threads();
}
    Force = new vsPoint*[nthreads];
    for (i = 0; i < nthreads; i++){
        Force[i] = new vsPoint[this->NoDrawingVertices];
        for (j = 0; j < this->NoDrawingVertices; ++j){
            Force[i][j].x = 0;
            Force[i][j].y = 0;
        }
    }


#pragma omp parallel shared(Force) private(tid, i, j, l, deltaX, deltaY, distSquared, dist, prod, force, w, first, second, a, b)
{

#pragma omp for schedule (static, chunk)
    for (i = 0; i < numVerts - 1; ++i){
        if (!this->Vertex[i].Draw)
            continue;

        tid = omp_get_thread_num();

        for (j = i + 1; j < numVerts; ++j){
            if (!this->Vertex[j].Draw)
                continue;
            //fprintf(stderr," fr layout %d\n", i);fflush(stderr);
            deltaX = this->Vertex[i].Pos.x - this->Vertex[j].Pos.x;
            deltaY = this->Vertex[i].Pos.y - this->Vertex[j].Pos.y;

            distSquared = (float)deltaX*(float)deltaX + (float)deltaY*(float)deltaY + epsilon;
            // Avoid divide by zero
            //if (distSquared == 0)
                //distSquared += epsilon;

            if ( distSquared < minD ){
                //a = log10(this->Vertex[i].Map->NoEdges);
                //b = log10(this->Vertex[j].Map->NoEdges);
                a = this->Vertex[i].NormalSize;
                b = this->Vertex[j].NormalSize;
                //a = 1;
                //b = 1;
                //a = 1 + (this->Vertex[i].NormalSize - 1) * (1 - FRRatio);
                //b = 1 + (this->Vertex[j].NormalSize - 1) * (1 - FRRatio);
                a = a > b ? a : b;
                //a = a * a;

                prod = a * (float)repulseConstant / (float)distSquared;
                first = (float)prod * (float)deltaX;
                second = (float)prod * (float)deltaY;

                Force[tid][i].x += first;
                Force[tid][i].y += second;
                Force[tid][j].x -= first;
                Force[tid][j].y -= second;
            }

        }
    }

#pragma omp for schedule (static, chunk)
    for (l = 0; l < this->NoEdges; ++l){
        if (!this->Edge[l].Draw)
            continue;
        //fprintf(stderr," fr layout a %d\n", i);fflush(stderr);
        tid = omp_get_thread_num();

        //w = this->Edge[l].NormalWeight;
//		if (this->FRRuns < this->MaxFRRuns / 2)
//			w = 1;
//		else
            //w = 1.0f / this->Edge[l].Target->Map->NoEdges;
        //w = 1.0f / sqrt(this->Edge[l].Source->Map->NoEdges * this->Edge[l].Target->Map->NoEdges);

        w = 1.0f / this->Edge[l].Target->Map->NoEdges + (1 - 1.0f / this->Edge[l].Target->Map->NoEdges) * FRRatio;

        i = this->Edge[l].Source->ID;
        j = this->Edge[l].Target->ID;

        if (!this->Vertex[i].Draw || !this->Vertex[j].Draw)
            continue;

        deltaX = this->Vertex[i].Pos.x - this->Vertex[j].Pos.x;
        deltaY = this->Vertex[i].Pos.y - this->Vertex[j].Pos.y;

        distSquared = (float)deltaX*(float)deltaX + (float)deltaY*(float)deltaY;
        dist = sqrt(distSquared) + epsilon;
        //dist = sqrt(distSquared) + epsilon - restDistance;

        force = springConstant * pow( (dist - 1 * restDistance), 2);
        prod = (float)w * (float)force / (float)dist;

        //prod = (float)w * (float)springConstant * (float)dist;
        first = (float)prod * (float)deltaX;
        second = (float)prod * (float)deltaY;

        Force[tid][i].x -= first;
        Force[tid][i].y -= second;
        Force[tid][j].x += first;
        Force[tid][j].y += second;
    }
}
//#pragma omp parallel shared(numVerts) private(i, j, Fo, dampenLast)
//{
//#pragma omp for schedule (static, chunk)
    for (i = 0; i < numVerts; ++i){
/*		if ((this->MouseButtonPressed == 0 || this->MouseButtonPressed == 2) && u == this->HitVertex){
            continue;
        }
        if (fixedPos && vtkVariant(fixedPos->GetTuple1(u)).ToInt() == 1)
            continue;
*/		//fprintf(stderr," fr layout b %d\n", i);fflush(stderr);
        if (!this->Vertex[i].Draw)
            continue;

//        if (this->mousePressed && (this->HitVertex == i))
//            continue;

        for (j = 0; j < nthreads; j++){
            this->Vertex[i].F.x += Force[j][i].x;
            this->Vertex[i].F.y += Force[j][i].y;
        }

        Fo = sqrt((float)this->Vertex[i].F.x*(float)this->Vertex[i].F.x + (float)this->Vertex[i].F.y*(float)this->Vertex[i].F.y) + epsilon;
        //T = this->Impl->F[u].first;
        this->Vertex[i].F.x = (fabs(this->Vertex[i].F.x) < T? fabs(this->Vertex[i].F.x) : T) * this->Vertex[i].F.x / Fo;
        //T = this->Impl->F[u].second;
        this->Vertex[i].F.y = (fabs(this->Vertex[i].F.y) < T? fabs(this->Vertex[i].F.y) : T) * this->Vertex[i].F.y / Fo;

        if (this->FRRuns == 0)
            dampenLast = 0;

        this->Vertex[i].Velocity.x = dampenLast * this->Vertex[i].Velocity.x + (1 - dampenLast) * this->Vertex[i].F.x;
        this->Vertex[i].Velocity.y = dampenLast * this->Vertex[i].Velocity.y + (1 - dampenLast) * this->Vertex[i].F.y;
        //uVel[0] = dampenLast*uVel[0] + this->Impl->F[u].first;
        //uVel[1] = dampenLast*uVel[1] + this->Impl->F[u].second;
        this->Vertex[i].Pos.x += 1.00 * this->Vertex[i].Velocity.x;
        this->Vertex[i].Pos.y += 1.00 * this->Vertex[i].Velocity.y;
        //fprintf(stderr, "%d %f %f %f %f\n", i, this->Vertex[i].Pos.x, this->Vertex[i].Pos.y, this->Vertex[i].Pos.x - this->Vertex[i].Velocity.x, this->Vertex[i].Pos.y - this->Vertex[i].Velocity.y);fflush(stderr);fflush(stderr);

        this->Vertex[i].F.x = 0;
        this->Vertex[i].F.y = 0;
        for (j = 0; j < nthreads; j++){
            Force[j][i].x = 0;
            Force[j][i].y = 0;
        }
    }
//}

    for (i = 0; i < nthreads; i++)
        delete []Force[i];
    //free(Force);
    delete []Force;


    //if (this->MouseButtonPressed == -1)
//    if ( !(this->mousePressed && (this->HitVertex != this->NoDrawingVertices)) )
      this->FRRuns += 1;

    this->FRRunning = false;
    fprintf(stderr, "OK\n");fflush(stderr);fflush(stderr);
}

void MdiChild::writeVertexPositionstoFile2(QString outputPath){

    if (this->DrawingFeature != 0)
        return;
    FILE * pFile;

    QString str2 = outputPath;

    QByteArray bytes = str2.toUtf8();
    char *ptr = new char[bytes.size()];
    ptr = bytes.data();
    fprintf(stderr, "\n\n%s\n\n", ptr);fflush(stderr);fflush(stderr);
    //fprintf(stderr, "\n\n%s\t %d\n\n", ptr1, this->PathName.size());fflush(stderr);fflush(stderr);

    pFile = fopen (ptr,"w");

    for (int i = 0; i < this->NoDrawingVertices; ++i)
    {

        fprintf(pFile, "%s,%d,%f,%f,%d\n", this->Vertex[i].Map->getLabel(), this->Vertex[i].Map->Feature->getID(), this->Vertex[i].Pos.x, this->Vertex[i].Pos.y, this->Vertex[i].Map->NoEdges);
    }

    fclose (pFile);
    //delete[] ptr;
}

void MdiChild::toggleLayoutView(bool enabled){
    if(!this->Graph)
    {
        this->checkExistenceOfNewFiles();
    }
    if ( !this->Graph->abstractGraph )
        this->Graph->createAbstractGraph();

    if (enabled)
        this->setNewGraph = this->Graph->abstractGraph;
    else
        this->setNewGraph = this->Graph;

    this->abstractView = enabled;

    //this->changedLayoutView(enabled);
}

void MdiChild::setAbstractDrawingVertices(bool onlyDrawn){
    this->NoShownVertices = 0;

    //if (this->EdgeMean == 0 && this->EdgeSD == 0){
    //if (this->edgeStatistics == NULL)
        this->calculateEdgeMeanSD(onlyDrawn);

    //double mean = this->EdgeMean;
    //double var = this->EdgeSD;

    float z = this->CuttingThreshold;

    for (int i = 0; i < this->NoDrawingVertices; i++){
        this->Vertex[i].Draw = false;
    }

    for (int i = 0; i < this->NoEdges; ++i){
        if (onlyDrawn && !this->Edge[i].Draw)
            continue;

        int f1, f2, E;
        if (this->DrawingFeature == 0){
            f1 = this->Edge[i].Source->Map->Feature->getID() - 1;
            f2 = this->Edge[i].Target->Map->Feature->getID() - 1;
            E = this->getNoFeatures() - 1;
        }
        else{
            f1 = 0;
            f2 = 0;
            E = 0;
        }

        if ( (this->Edge[i].ActualWeight - this->edgeStatistics[f1 * E + f2].mean) / this->edgeStatistics[f1 * E + f2].SD < z ){
            this->Edge[i].Draw = false;
        }
        else{
            this->Edge[i].Draw = true;

            this->Edge[i].Source->Draw = true;
            this->Edge[i].Target->Draw = true;
        }
    }

    for (int i = 0; i < this->NoDrawingVertices; i++)
        if (this->Vertex[i].Draw)
            this->NoShownVertices++;
    fprintf(stderr, "NoShownVertices:%d NoVertices:%d, CutThres: %f\n", this->NoShownVertices, this->NoDrawingVertices, this->CuttingThreshold);fflush(stderr);
    //fprintf(stderr, "CutThres: %f, Mean: %f, SD: %f\n", this->CuttingThreshold, this->EdgeMean, this->EdgeSD);fflush(stderr);
}

void MdiChild::calculateEdgeMeanSD(bool onlyDrawn){
    float f;

    int E = 0;

    if (this->DrawingFeature == 0)
        E = this->getNoFeatures() - 1;
    else
        E = 1;

    free(this->edgeStatistics);
    this->edgeStatistics = (vsSD*) malloc(E * E * sizeof(vsSD));

    for	(int i = 0; i < E * E; ++i){
        this->edgeStatistics[i].N = 0;
        this->edgeStatistics[i].mean = 0;
        this->edgeStatistics[i].SD = 0;
    }

    for (int i = 0; i < this->NoEdges; ++i){
        if (onlyDrawn && !this->Edge[i].Draw)
            continue;

        int f1, f2;
        if (this->DrawingFeature == 0){
            f1 = this->Edge[i].Source->Map->Feature->getID() - 1;
            f2 = this->Edge[i].Target->Map->Feature->getID() - 1;
        }
        else{
            f1 = 0;
            f2 = 0;
        }

        this->edgeStatistics[f1 * E + f2].mean += this->Edge[i].ActualWeight;
        this->edgeStatistics[f1 * E + f2].N++;
    }

    for	(int i = 0; i < E; ++i){
        for	(int j = i + 1; j < E; ++j){
            this->edgeStatistics[i * E + j].mean += this->edgeStatistics[j * E + i].mean;
            this->edgeStatistics[j * E + i].mean = this->edgeStatistics[i * E + j].mean;
            this->edgeStatistics[i * E + j].N += this->edgeStatistics[j * E + i].N;
            this->edgeStatistics[j * E + i].N = this->edgeStatistics[i * E + j].N;
        }
    }
    for	(int i = 0; i < E * E; ++i){
        this->edgeStatistics[i].mean /= this->edgeStatistics[i].N;
    }

    for (int i = 0; i < this->NoEdges; ++i){
        if (onlyDrawn && !this->Edge[i].Draw)
            continue;

        int f1, f2;
        if (this->DrawingFeature == 0){
            f1 = this->Edge[i].Source->Map->Feature->getID() - 1;
            f2 = this->Edge[i].Target->Map->Feature->getID() - 1;
        }
        else{
            f1 = 0;
            f2 = 0;
        }

        f = (this->Edge[i].ActualWeight - this->edgeStatistics[f1 * E + f2].mean);

        this->edgeStatistics[f1 * E + f2].SD += f * f;
    }

    for	(int i = 0; i < E; ++i){
        for	(int j = i + 1; j < E; ++j){
            this->edgeStatistics[i * E + j].SD += this->edgeStatistics[j * E + i].SD;
            this->edgeStatistics[j * E + i].SD = this->edgeStatistics[i * E + j].SD;
        }
    }
    for	(int i = 0; i < E * E; ++i){
        this->edgeStatistics[i].SD /= this->edgeStatistics[i].N;
        this->edgeStatistics[i].SD /= sqrt(this->edgeStatistics[i].SD);
    }

    //this->EdgeMean = mean;
    //this->EdgeSD = var;
}

int MdiChild::getNoFeatures(){
    if (this->Graph)
        return this->Graph->getNoFeatures();

    return 0;
}
