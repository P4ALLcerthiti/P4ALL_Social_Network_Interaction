#include "DynamicAD.h"

#include <fstream>
#include <iostream>
#include <sstream>
//#include <Eigen/SVD>
#include <Eigen/Eigenvalues> 
#include "DynamicADCommon.h"
#include "DynamicADResult.h"
#include <QtCore/QDateTime>
#include <QtCore/QStringList>
DynamicAD::DynamicAD(const std::string& strName)
{
	m_strName = strName;
	m_strTimeDataFile = "";
}

DynamicAD::~DynamicAD()
{
	clearAll();
	for (unsigned int i=0; i<m_vResult.size(); i++) delete m_vResult[i];
}

bool DynamicAD::LoadTimeCSV(const std::string& strFilename, const std::string& strName, int num_of_semantic_characters,string time_type/*""*/,string dataset_name/*""*/,string timeperiod/*""*/)
{
	clearAll();
	m_strTimeDataFile = strFilename;
	m_strTimeDataName = strName;
	
	std::ifstream ifs(strFilename.c_str());

	if (!ifs.is_open() || !ifs.good()) return false;
	char buffer[10024];
	DynamicADTimeEntry* dynTE = NULL;
	
	QDateTime first_datetime;
	QDateTime last_datetime;
	QDateTime current_datetime;
	if(dataset_name=="harmur2")
	{
		int pos_of_komma=timeperiod.find(",");
		string first_datetime_str=timeperiod.substr(0,pos_of_komma);
		string last_datetime_str=timeperiod.substr(pos_of_komma+1);

		
		first_datetime = QDateTime::fromString(first_datetime_str.c_str() ,"yyyy-MM-dd HH:mm:ss");
		last_datetime = QDateTime::fromString(last_datetime_str.c_str() ,"yyyy-MM-dd HH:mm:ss");


	}
	while (!ifs.eof())
	{
		DynamicADTimeEntry::TimeType timetype;
		dynTE = new DynamicADTimeEntry(timetype);
		ifs.getline(buffer, 1024, ',');
		if (strcmp(buffer, ""))
		{
			dynTE->SetKeyData(buffer);
			ifs.getline(buffer, 10024, '\n');
			string time_not_cutted=buffer;
			string time_cutted=buffer;
			int point_to_cut=0;
			if(dataset_name=="harmur2")
			{
				QString line_string=buffer;
				QStringList dates_in_line = line_string.split( "|" );
				int num_seen_red=0;
				for(int i=0;i<dates_in_line.length();i++)
				{
					QString date_in_line=dates_in_line.value(i).mid(0,19);
					current_datetime = QDateTime::fromString(date_in_line ,"yyyy-MM-dd HH:mm:ss");
					if(current_datetime>first_datetime&&current_datetime<last_datetime)
					{
						time_not_cutted=date_in_line.toLatin1().data();
						num_seen_red++;
						break;
					}
				}
				if(num_seen_red==0)
					continue;
			}
			if(time_type=="day")
			{
				point_to_cut=time_not_cutted.find_first_of("T");
				if(point_to_cut<0) point_to_cut=time_not_cutted.find_first_of(" ");
				time_cutted=time_not_cutted.substr(0,point_to_cut);

			}
			else if(time_type=="hour")
			{
				point_to_cut=time_not_cutted.find_first_of(":");
				time_cutted=time_not_cutted.substr(0,point_to_cut);
			}
			else if(time_type=="cut a number of characters")
			{
				point_to_cut=num_of_semantic_characters;
				time_cutted=time_not_cutted.substr(0,point_to_cut);
			}

			dynTE->SetTimeData(time_cutted.c_str());
			dynTE->FixDataFormat();
			m_vTimeEntry.push_back(dynTE);

			m_mapTimeEntry[dynTE->GetKeyData()] = dynTE;

			//std::map<std::string, DynamicADTimeEntry*> m_mapTimeEntry;	
		}
	}
	ifs.close();
	std :: sort(m_vTimeEntry.begin(),m_vTimeEntry.end(),&DynamicAD::sortmvtimeentry);
	std::cout << "Time entries: " << m_vTimeEntry.size() << "\n";

	distributeDataIntoTimeGroups();
	std::cout << "Time instances: " << m_vTimeGroup.size() << "\n";

	return true;
}

void DynamicAD::PrintTimeSetToFile(const std::string& strFile)
{
	std::ofstream ofs(strFile.c_str());

	for (unsigned int i=0; i<m_vTimeGroup.size(); i++)
	{
		ofs << m_vTimeGroup[i]->GetCommonTimeData() << "\n";
	}
	ofs.close();
}

DynamicADTimeGroup* DynamicAD::GetTimeGroup(const std::string& strTime)
{
	for (unsigned int i=0; i<m_vTimeGroup.size(); i++)
	{
		if (m_vTimeGroup[i]->GetCommonTimeData() == strTime)
		{
			return m_vTimeGroup[i];
		}
	}
	return NULL;
}

std::vector< std::pair<std::string, unsigned int> > DynamicAD::GetDataDistributionPerTime()
{
	std::vector< std::pair<std::string, unsigned int> > vPair;

	std::pair<std::string, unsigned int> tPair;
	for (unsigned int i=0; i<m_vTimeGroup.size(); i++)
	{
		tPair.first = m_vTimeGroup[i]->GetCommonTimeData();
		tPair.second = m_vTimeGroup[i]->GetNumEntries();

		vPair.push_back(tPair);
	}

	return vPair;
}

void DynamicAD::LoadFeatureCSV(const std::string& strFilename, const std::string& strName, unsigned int iDiscardWordsWithSizeLessThan, bool bSplitData, char chDelimiter)
{
	DynamicADFeature* feature = new DynamicADFeature();
	feature->SetName(strName);
	feature->LoadCSV(strFilename, iDiscardWordsWithSizeLessThan, bSplitData, chDelimiter);
	
	
	std::cout << "assigning time values to feature data...";

	/* //OLD METHOD WORKS OK, BUT VERY SLOW:
	//assign time data: this is the slowest part!
	//	int fd, td;
	#pragma omp parallel for
	for (int fd=0; fd<feature->GetNumEntries(); fd++)
	{
		for (unsigned int td=0; td<m_vTimeEntry.size(); td++)
		{
			if (feature->GetEntry(fd)->GetKeyDataInv() == m_vTimeEntry[td]->GetKeyDataInv())
			//if (!std::strcmp(feature->GetEntry(fd)->GetKeyData().c_str(), m_vTimeEntry[td]->GetKeyData().c_str()))
			{
				feature->GetEntry(fd)->SetTimeEntry(m_vTimeEntry[td]);
				break;
			}
		}
	}
	*/

	//NEW METHOD:
	//ofstream ofs5("deleted_featurelabels.txt");
	std::map<std::string, DynamicADTimeEntry*>::iterator it;
	std::map <std::string, unsigned int>::iterator m_mapId2Num_it;
	for (unsigned int fd=0; fd<feature->GetNumEntries(); fd++)
	{
		it = m_mapTimeEntry.find(feature->GetEntry(fd)->GetKeyData());
		std::string featurekey=feature->GetEntry(fd)->GetKeyData();
		std::string featurevalue=feature->GetEntry(fd)->GetFeatureDataSingle();
		if (it != m_mapTimeEntry.end())
		{
			feature->GetEntry(fd)->SetTimeEntry(it->second);
		}
		else
		{
			
			feature->GetTimesPerDataWord().find(feature->GetEntry(fd)->GetFeatureDataSingle())->second--;
			//feature->GetTimesPerDataWord().erase(m_mapId2Num_it);
			std::vector<DynamicADFeatureEntry*>::iterator Iter=feature->Get_m_vEntry()->begin()+fd;
			feature->Get_m_vEntry()->erase(Iter);
			
			fd--;
			
			//delete *Iter;
			
		}
	}
	/*ofstream ofs4("featurelabels.txt");
	for(int i=0;i<feature->GetNumEntries();i++)
	{
		ofs4<<feature->GetEntry(i)->GetKeyData()<<" "<<feature->GetEntry(i)->GetFeatureDataSingle()<<"\n";
	}
	ofs4.close();*/

	m_vFeature.push_back(feature);

	std::cout << " ok!\n";
}

void DynamicAD::RemoveFeatureCSV(DynamicADFeature* feature)
{
	for (std::vector<DynamicADFeature*>::iterator iter = m_vFeature.begin(); iter != m_vFeature.end(); iter++)
	{
		if (*iter == feature)
		{
			m_vFeature.erase(iter);
			delete feature;
			//delete any graphs:
			for (unsigned int i=0; i<m_vGraph.size(); i++) delete m_vGraph[i];
			m_vGraph.clear();
			break;
		}
	}
}

DynamicADFeature* DynamicAD::GetFeature(const std::string& strName)
{
	for (unsigned int i=0; i<m_vFeature.size(); i++)
	{
		if (m_vFeature[i]->GetName() == strName)
		{
			return m_vFeature[i];
		}
	}
	return NULL;
}

void DynamicAD::CreateGraphs()
{
	//clear previous information:
	
	for (unsigned int i=0; i<m_vGraph.size(); i++) 
		delete m_vGraph[i];
	m_vGraph.clear();
	//m_vGraph.clear();

	//the number of graphs is equal to the number of time instances:
	int mvgraphsize=m_vGraph.size();
	int mvfeaturesize=m_vFeature.size();

	//create the graphs
	for (int t=0; t<m_vTimeGroup.size(); t++)
	{
		DynamicADGraph* graph = new DynamicADGraph(m_vTimeGroup[t]->GetCommonTimeData(), m_vFeature.size());
		m_vGraph.push_back(graph);
	}

	for (int f=0; f<m_vFeature.size(); f++)
	{		
		for (int fd=0; fd<m_vFeature[f]->GetNumEntries(); fd++)
		{
			m_vGraph[m_vFeature[f]->GetEntry(fd)->GetTimeEntry()->GetTimeGroup()->GetIdx()]->AddFeatureEntry(m_vFeature[f]->GetEntry(fd), f);
		}
	}

	//create the big sparse matrix indices:
	std::vector< std::set<std::string> > vIdSet;
	std::map<std::string, int> idMap;

	int j=0;
	for (unsigned int i=0; i<m_vFeature.size(); i++)
	{
		vIdSet.push_back(m_vFeature[i]->GetIdSet());
	
		for (std::set<std::string>::iterator itSet=m_vFeature[i]->GetIdSet().begin(); itSet!=m_vFeature[i]->GetIdSet().end(); ++itSet)
		{
			std::pair<std::map<std::string, int>::iterator,bool> ins_succeed=idMap.insert(std::pair<std::string, int>(*itSet, j++));
			//if(ins_succeed.second==true) j++;
		}	
	}
	
	//*
	//connect the nodes:
	/*std::ofstream ofs("hasnodes.txt");
	std::ofstream ofs2("idmap.txt");
	std::ofstream ofs3("idset.txt");
	for(map<std::string,int>::iterator  i=idMap.begin();i!=idMap.end();i++)
	{
		ofs2<<(*i).first<<" "<<(*i).second<<"\n";
	}
	ofs2.close();
	for(int i=0;i<vIdSet.size();i++)
	{
		for(std::set<std::string>::iterator it=vIdSet[i].begin();it!=vIdSet[i].end();it++)
		{
			ofs3<<*it<<"\n";
		}
		//ofs3<<"\n";
	}
	ofs3.close();*/
	for (int i=0; i<m_vGraph.size(); i++)
	{
		m_vGraph[i]->CreateGraph(idMap, vIdSet);
		std::stringstream ss;
		ss << "Graph t:" << i << " has " << m_vGraph[i]->GetNumNodes() << " nodes and " << m_vGraph[i]->GetNumEdges() << " edges!\n";
		//ofs<< "Graph t:" << i << " has " << m_vGraph[i]->GetNumNodes() << " nodes and " << m_vGraph[i]->GetNumEdges() << " edges!\n";
		std::cout << ss.str();
	}
	//ofs.close();
	std::cout << "Graphs created!\n";
	//*/
}
//std::ofstream matd("matd.txt",std::ofstream::app); 
void DynamicAD::PerformMDS(const DynamicADGraph::DistanceType& distanceType, std::string strDir/*=""*/)
{
	unsigned int i,j;
	unsigned int dimD = m_vGraph.size();

	//create distance matrix
	Eigen::MatrixXd matD(dimD, dimD);

	double d;
	for (i=0; i<dimD-1; i++)
	{
		for (j=i+1; j<dimD; j++)
		{
			d = m_vGraph[i]->GetDistanceToGraph(m_vGraph[j], distanceType);
			matD(i,j) = matD(j,i) = d;
			//matd<<d<<" ";
		}
		//matd<<"\n";
	}
	
	for (i=0; i<dimD; i++)
	{
		matD(i,i) = 0.0;
	}

	Eigen::MatrixXd cOnes(dimD,1);
	for (i=0; i<dimD; i++)
	{
		cOnes(i,0) = 1.0;
	}

	Eigen::MatrixXd I(dimD, dimD);
	for (i=0; i<dimD; i++)
	{
		for (j=0; j<dimD; j++)
		{
			I(i,j) = i==j? 1.0 : 0.0;
		}
	}

	Eigen::MatrixXd J = I - cOnes * cOnes.transpose() * (1.0/dimD);
	Eigen::MatrixXd B = J * matD * J * (-0.5);

	//perform the EigenDecomposition of B
	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigenSolver(B);
	std::cout << "\nEIGEN VALUES:\n============\n" << eigenSolver.eigenvalues() << "\n";
	//V einai to Q eno D einai to L
	Eigen::MatrixXd D(eigenSolver.eigenvalues().asDiagonal());
	Eigen::MatrixXd V(eigenSolver.eigenvectors());

	Eigen::MatrixXd P_inv(V*D.cwiseSqrt());
	Eigen::MatrixXd P(P_inv.rows(), P_inv.cols());
	for (i=0; i<P_inv.cols(); i++)
	{
		P.col(i) = P_inv.col(P.cols()-i-1);
	}
	for(int j=0;j<P.cols();j++)
	{
		if(P.coeff(0,j)<0)
		{
			for(int i=0;i<P.rows();i++)
			{
				P.coeffRef(i,j)=(-1)* P.coeff(i,j);
			}

		}

	}
	Eigen::MatrixXd P_3D(P.rows(), 3);
	for (i=0; i<3; i++)
	{
		P_3D.col(i) = P.col(i);
	}

	//create new result:
	DynamicADResult* result = new DynamicADResult(this);
	result->SetDistanceType(distanceType);
	result->SetMatD(matD);
	result->SetMatP(P);
	m_vResult.push_back(result);

	//save to disk?
	if (strDir != "") 
	{
		strDir.append("/");
		DynamicADCommon::WriteMatrixToFile(matD, strDir+"Mat_Distance.csv");
		DynamicADCommon::WriteMatrixToFile(B, strDir+"Mat_B.csv");
		DynamicADCommon::WriteMatrixToFile(D, strDir+"Mat_D.csv");
		DynamicADCommon::WriteMatrixToFile(V, strDir+"Mat_U.csv");
		DynamicADCommon::WriteMatrixToFile(P_3D, strDir+"Mat_P3D.csv");
	}

	//Eigen::EigenSolver<Eigen::MatrixXd> eigenSolver(B, true);
	//DyanmicADCommon::WriteMatrixToFile(eigenSolver.eigen);
	//std::cout << "\nEIGEN VALUES:\n============\n" << eigenSolver.eigenvalues().real() << "\n";
	//std::cout << "\nPSEUDO EIGEN MATIRX:\n==========\n" << eigenSolver.pseudoEigenvalueMatrix() << "\n";
	//DynamicADCommon::WriteMatrixToFile(eigenSolver.pseudoEigenvalueMatrix(), "RemTest\\Mat_D.csv");
	//DynamicADCommon::WriteMatrixToFile(eigenSolver.pseudoEigenvectors(), "RemTest\\Mat_U.csv");

	/* //perform SVD-decomposition
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(B, Eigen::ComputeFullU);
	Eigen::MatrixXd S(svd.singularValues().asDiagonal());
	Eigen::MatrixXd U(svd.matrixU());
	//write to a file:
	//std::ofstream ofsU("RemTest/U.csv");
	//for (i=0; i<dimD; i++)
	//{
	//	for (j=0; j<dimD; j++)
	//	{
	//		ofsU << U.coeff(i,j) << '\t';
	//	}
	//	ofsU << '\n';
	//}
	//ofsU.close();
	Eigen::MatrixXd X(U * S.cwiseSqrt());
	for (i=0; i<dimD; i++)
	{
		for (j=0; j<3; j++)
		{
			std::cout << X.coeff(i,j) << "\t";
		}
		std::cout << '\n';
	}
	//std::cout << "Singular values: \n" << S;
	*/
}

void DynamicAD::clearAll()
{
	for (unsigned int i=0; i<m_vTimeEntry.size(); i++) delete m_vTimeEntry[i];
	for (unsigned int i=0; i<m_vTimeGroup.size(); i++) delete m_vTimeGroup[i];
	for (unsigned int i=0; i<m_vFeature.size(); i++) delete m_vFeature[i];
	for (unsigned int i=0; i<m_vGraph.size(); i++) delete m_vGraph[i];

	m_vTimeEntry.clear();
	m_mapTimeEntry.clear();
	m_vTimeGroup.clear();
	m_vFeature.clear();
	m_vGraph.clear();
}

void DynamicAD::distributeDataIntoTimeGroups()
{
	DynamicADTimeGroup* dTGroup = NULL;

	for (unsigned int i=0; i<m_vTimeEntry.size(); i++)
	{
		dTGroup = GetTimeGroup(m_vTimeEntry[i]->GetTimeData());
		if (!dTGroup)
		{
			dTGroup = new DynamicADTimeGroup(m_vTimeEntry[i]->GetTimeData());
			dTGroup->SetIdx(m_vTimeGroup.size());
			m_vTimeGroup.push_back(dTGroup);
		}
		dTGroup->AddEntry(m_vTimeEntry[i]);
	}
	//std :: sort(m_vTimeGroup.begin(),m_vTimeGroup.end(),&DynamicAD::sortmvtimegroup);
	//set indices

}
bool DynamicAD::sortmvtimegroup (DynamicADTimeGroup* i,DynamicADTimeGroup* j)
{

	return  (i->GetCommonTimeData() < j->GetCommonTimeData()) ;
}
bool DynamicAD::sortmvtimeentry (DynamicADTimeEntry* i,DynamicADTimeEntry* j)
{

	return  (i->GetTimeData() < j->GetTimeData()) ;
}

