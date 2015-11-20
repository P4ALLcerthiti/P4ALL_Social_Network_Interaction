#include "DynamicADGraph.h"
#include "DynamicADFeatureEntry.h"
#include <iostream>
#include <map>
#include <fstream>
#include<iterator>
//std::ofstream nodesstream("nodes.txt",std::fstream::app);
const std::string DynamicADGraph::s_DistanceType2Str[] =
{
	"L1", //DistanceType_L1
	"L2", //DistanceType_L2
	"Euclidean", //DistanceType_EUCLIDEAN
	"Node Operations", //DistanceType_NodeOperations
	"Edge Operations", //DistanceType_EdgeOperations 
	"Graph Operations", //DistanceType_GraphOperations
};

DynamicADGraph::DynamicADGraph(const std::string& strName, const unsigned int& numFeatures)
{
	if (numFeatures < 2) 
	{
		std::cout << "ERROR: The number of features of the graph has to be greater or equal to 2\n";
		system("pause");
		exit(1);
	}

	m_strName = strName;
	m_numFeatures = numFeatures;
	m_vFeatureEntry = new std::vector<DynamicADFeatureEntry*>[numFeatures];
	m_vNode = new std::set<std::string>[numFeatures];

	m_numNodes = 0;
	m_numEdges = 0;
}

DynamicADGraph::~DynamicADGraph()
{
	for(int i=0;i<m_vFeatureEntry->size();i++) m_vFeatureEntry[i].clear();
	delete [] m_vFeatureEntry;
	
	for(int i=0;i<m_vNode->size();i++) m_vNode[i].clear();
	delete [] m_vNode;
}

void DynamicADGraph::AddFeatureEntry(DynamicADFeatureEntry* entry, const unsigned int& idxFeature)
{
	m_vFeatureEntry[idxFeature].push_back(entry);

	std::multimap<std::string, std::string>::iterator it;
	//edo den xreiazetai for giati panta eisagei ena stoixeio to entry->m_strRawFeatureData
	for (it=entry->GetFeatureData().begin(); it!=entry->GetFeatureData().end(); it++)
	{
		m_vNode[idxFeature].insert(it->second);
	}
}

/*
void DynamicADGraph::ConnectNodes()
{
	m_vEdge.clear();
	unsigned int f1, f2, i1, i2;
	std::pair<DynamicADFeatureEntry*, DynamicADFeatureEntry*> edge;

	for (f1=0; f1<m_numFeatures-1; f1++)
	{
		for (f2=f1+1; f2<m_numFeatures; f2++)
		{
			for (i1=0; i1<m_vFeatureEntry[f1].size(); i1++)
			{
				for (i2=0; i2<m_vFeatureEntry[f2].size(); i2++)
				{
					if (m_vFeatureEntry[f1][i1]->GetKeyData() == m_vFeatureEntry[f2][i2]->GetKeyData())
					{
						edge.first = m_vFeatureEntry[f1][i1];
						edge.second = m_vFeatureEntry[f2][i2];
						m_vEdge.push_back(edge);
					}
				}
			}
		}
	}

	m_numEdges = m_vEdge.size();
}
*/

void DynamicADGraph::CreateGraph(const std::map<std::string, int>& idMap, const std::vector< std::set<std::string> > &vIdSet)
{
	//std::ofstream ofs("test_dynamic_crash.txt");
	for (unsigned int i=0; i<m_numFeatures; i++)
	{
		m_numNodes += m_vNode[i].size();
	}

	//initialise the sparse matrix:
	int dim = idMap.size();
	m_matrix = Eigen::SparseMatrix<double>(dim, dim);

	//create the maps of each feature set:
	std::vector< std::multimap<std::string, std::string > > vMapFeature;
	for (int f=0; f<m_numFeatures; f++)
	{
		std::multimap< std::string, std::string > mapFeature;
		for (int k=0; k<m_vFeatureEntry[f].size(); k++)
		{
			mapFeature.insert(m_vFeatureEntry[f][k]->GetFeatureData().begin(), m_vFeatureEntry[f][k]->GetFeatureData().end());
		}
		vMapFeature.push_back(mapFeature);
	}
	
	//* //now create the edges:
	std::multimap<std::string, std::string>::iterator it, it2;
	std::pair<std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator> vRange;
	std::map<std::string, int>::iterator itMap;

	//std::vector<Eigen::Triplet<int> > vListOfTriples;

	//sta dio prota for sindiazei oles tis pithanes omades feature p.x. an exoume country bot another oi sindiasmoi 
	//einai country bot,country another,bot another
	unsigned int idxA, idxB;
	for (int f1=0; f1<m_numFeatures-1; f1++)
	{
		for (int f2=f1+1; f2<m_numFeatures; f2++)
		{
			for (it=vMapFeature[f1].begin(); it!=vMapFeature[f1].end(); ++it)
			{
				//vriskei sti deyteri omada feature posi einai i megaliteri apostasi pou periexetai to kleidi tou brazil
				//px meta to kleidi tou greece ktl
				//edo mporei  kai na min tairiazei to equal_range pou vriskei sinexomena omoia.mporei na prepei na psaxoume
				//gia ola ta equal kleidia tou protou set sto deytero set
				vRange = vMapFeature[f2].equal_range((*it).first);
				//int forespoumpike=0;
				for (it2=vRange.first; it2!=vRange.second; it2++)
				{
					//m_matrix(idMap.find((*it).second)->second, idMap.find((*ti2).second)->second
				
					//edge between (*it1).second and (*it2).second
					//edo afou exei vrei to kleidi tis imerominias 27/10/2012 me poia kleidia country tairiazei
					//px me brazil,greece paei ena ena kai psaxnei sto idmap to it->second pou einai 27/10/2012 (oxi to kleidi)
					//kai vriskei px 150..meta psaxnei kai to brazil sto id map kai vriskei px 1500.tis times tis vazei 
					//sta idxa,adxb

					//forespoumpike++;
					idxA = idMap.find(it->second)->second;
					idxB = idMap.find(it2->second)->second;
					//ofs<<idxA<<" "<<idxB<<"\n";
					//m_matrix->insert(idxA, idxB) = 0;
					m_matrix.coeffRef(idxA, idxB) += 1.0;
					m_matrix.coeffRef(idxB, idxA) += 1.0; //symmetric
				}
				/*if(forespoumpike>1)
				{
					forespoumpike=8;
				}*/
			}
		}
	}

	m_matrix.makeCompressed();

	m_numEdges = m_matrix.nonZeros();

	/*
	std::ofstream ofs("RemTest/MatrixTest" );
	unsigned r,c;
	for (r=0; r<m_matrix->rows(); r++)
	{
		for (c=0; c<m_matrix->cols(); c++)
		{
			ofs << m_matrix->coeffRef(r,c) << ",";
		}
		ofs << "\n";
	}
	ofs << m_matrix << "\n";
	ofs.close();
	*/
}

double DynamicADGraph::GetDistanceToGraph(DynamicADGraph* graph, const DistanceType& distanceType)
{
	if (distanceType == DistanceType_L1)
	{
		return (graph->m_matrix-m_matrix).cwiseAbs().sum();
	}
	else if (distanceType == DistanceType_L2)
	{
		return (graph->m_matrix-m_matrix).cwiseAbs2().sum();
	}
	else if (distanceType == DistanceType_EUCLIDEAN)
	{
		return sqrt((graph->m_matrix-m_matrix).cwiseAbs2().sum());
	}
	else if (distanceType == DistanceType_NodeOperations)
	{
		nodeoperations=getNumNodeOperations(graph);
		return nodeoperations;
	}
	else if (distanceType == DistanceType_EdgeOperations)
	{
		edgeoperations=getNumEdgeOperations(graph);
		return edgeoperations;
	}
	else if (distanceType == DistanceType_GraphOperations)
	{
		return nodeoperations + edgeoperations;
	}

	return 0.0;
}

unsigned int DynamicADGraph::getNumNodeOperations(DynamicADGraph* graph)
{
	std::vector<std::string> vOperations;
	
	//compare the nodes of every set:		
	for (unsigned int f=0; f<m_numFeatures; f++)
	{
		//ayto ousiastika vazei ta diaforetika stoixeia apo ta dio m_vnode
		//diladi an to ena exei brazil,greece, japan kai to allo japan,greece
		//tha mpei mono greece
		std::set_symmetric_difference(
			m_vNode[f].begin(), m_vNode[f].end(),
			graph->m_vNode[f].begin(), graph->m_vNode[f].end(),
			std::back_inserter(vOperations)
			);
		
	}

	return vOperations.size();
}
//std::ofstream m_matrixes("m_matrixes.txt",std::ofstream::app);
unsigned int DynamicADGraph::getNumEdgeOperations(DynamicADGraph* graph)
{
	unsigned int numOperations = 0;
	double value,value2;
	for (int k=0; k<m_matrix.outerSize(); ++k)
	{
		for (Eigen::SparseMatrix<double>::InnerIterator it(m_matrix,k); it; ++it)
		{
			value=it.value();
			value2=graph->m_matrix.coeff(it.row(), it.col());
			if(value2==0)
				numOperations++;
				//m_matrixes<<numOperations<<" "<<it.row()<<" "<<it.col()<<" "<<m_matrix.coeff(it.row(), it.col())<<" "<<graph->m_matrix.coeff(it.row(), it.col())<<"\n";
			
		}
	}

	for (int k=0; k<graph->m_matrix.outerSize(); ++k)
	{
		for (Eigen::SparseMatrix<double>::InnerIterator it(graph->m_matrix,k); it; ++it)
		{
			value=it.value();
			value2=m_matrix.coeff(it.row(), it.col());
			if(value2==0)
				numOperations++;
			
		}
	}

	//slow process:
		/*numOperations = 0;
		double coeffA, coeffB;
		int r,c;
		int nRows = m_matrix.rows();
		int nCols = m_matrix.cols();
		for (r=0; r<nRows; r++)
		{
			for (c=0; c<nCols; c++)
			{
				coeffA = m_matrix.coeff(r,c);
				coeffB = graph->m_matrix.coeff(r,c);

				if ((coeffA > 0.0 && coeffB == 0.0) ||
					(coeffA== 0.0 && coeffB > 0.0))
				{
					numOperations++;
				}
			}
		}
		*/

	return numOperations;
}
