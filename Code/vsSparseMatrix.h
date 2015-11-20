#ifndef VSSPARSEMATRIX_H_
#define VSSPARSEMATRIX_H_
#include "fstream"
class vsSparseMatrixElement{
public:
	void setValue(int C, float V){
		this->Index = C;
		this->Value = V;
	}

	int Index;
	float Value;
};

class vsVector{
public:
	std::vector<vsSparseMatrixElement* > Element;
	int GlobalIndex;
	int NoElements;

	vsVector(){
		Element.reserve(1);
		NoElements=0;
	}
	~vsVector(){
		for(int i=0;i<Element.size();i++)
		{
			delete Element[i];
		}
		this->Element.clear();
		//this->Element = NULL;
		this->NoElements = 0;
	}

	void initialize(){
		//this->Element = NULL;
		this->NoElements = 0;
		//this->GlobalIndex = 0;
	}
	void destroy(){
		//free(this->Element);
		//this->Element = NULL;
		for(int i=0;i<Element.size();i++)
		{
			delete Element[i];
		}
		this->Element.clear();
		this->NoElements = 0;
		
	}
	void insertValue(int C, float value ,int towrite=1){

		//this->Element.push_back(vsSparseMatrixElement a) = (vsSparseMatrixElement*) realloc(this->Element, (this->NoElements + 1) * sizeof(vsSparseMatrixElement));
		vsSparseMatrixElement* a=new vsSparseMatrixElement;
		a->setValue(C, value);
		this->Element.push_back(a);
		
		//this->Element[this->NoElements].setValue(C, value);

		this->NoElements++;
	}

	float getValue(int C, int &next, int &nextindex, bool erase = false){
		float value;

		for (int i = nextindex; i < this->NoElements; ++i){
			if (this->Element[i]->Index < C)
				continue;
			if (this->Element[i]->Index > C){
				next = Element[i]->Index;
				nextindex = i;
				return 0.0;
			}
			if (this->Element[i]->Index == C){
				if (i + 1 < this->NoElements){
					next = Element[i + 1]->Index;
					nextindex = i + 1;
				}
				else{
					nextindex = this->NoElements;
					next = this->NoElements;
				}
				value = this->Element[i]->Value;
				if (erase)
					this->Element[i]->Value = 0;
				return value;
			}
		}

		return 0.0;
	}
/*
	float searchanderaseElement(int C, int &next, int &nextindex)){
		float value;
		for (int i = 0; i < this->NoElements; ++i){
			if (this->Element[i].Index < C)
				continue;
			if (this->Element[i].Index > C){
				next = Element[i].Index;
				nextindex = i;
				return 0.0f;
			}
			if (this->Element[i].Index == C){
				if (i + 1 < this->NoElements){
					next = Element[i + 1].Index;
					nextindex = i + 1;
				}
				else{
					nextindex = this->NoElements;
					next = this->NoElements;
				}
				value = this->Element[i].Value;
				this->Element[i].Value = 0;
				return value;
			}
		}
		return 0.0f;
	}
*/
};

class vsMatrix{
public:
	std::vector<vsVector> Row;
	int NoRows;
	
	vsMatrix(int R){
		this->NoRows = R;

		/*this->Row = (vsVector*) malloc(R * sizeof(vsVector));
		if ( !this->Row){
			fprintf(stderr, "Cannot allocate memory for array Row\n.");fflush(stderr);
			exit(1);
		}*/
		Row.reserve(R);
		for	(int i = 0; i < R; ++i){
			vsVector a;
			this->Row.push_back(a);
		} 
	}

	~vsMatrix(){
		for (int i = 0; i < this->NoRows; ++i){
			this->Row[i].destroy();
		}
		Row.clear();
		//free(this->Row);
		//this->Row = NULL;
	}

	void initialize(){
		//this->Row = NULL;
		this->NoRows = 0;
	}

	void clear(){
		for (int i = 0; i < this->NoRows; ++i){
			this->Row[i].destroy();
		}
		//Row.clear();
	}

	void insertValue(int R, int RGI, int C, float value){
		this->Row[R].insertValue(C, value);
		this->Row[R].GlobalIndex = RGI;
	}

	void printSelf(char* name){
		std::ofstream ofs(name);
		for (int i = 0; i < this->NoRows; ++i){
			ofs<<i;
			fprintf(stderr, "%5d: ", i);fflush(stderr);
			for (int j = 0; j < this->Row[i].NoElements; ++j){
				ofs<<" "<<this->Row[i].Element[j]->Index<<" "<<this->Row[i].Element[j]->Value;
				fprintf(stderr, "(%d)%4.2f ", this->Row[i].Element[j]->Index, this->Row[i].Element[j]->Value);fflush(stderr);
			}
			ofs<<"\n";
			fprintf(stderr, "\n");fflush(stderr);
		}
		ofs.close();
	}

	void createFromDenseMatrix(float* M, int R, int C){
		this->clear();

		for (int i = 0; i < R; i++){
			this->Row[i].GlobalIndex = i;

 			for (int j = 0; j < C; j++)
				if (M[i * C + j] > 0){
					this->Row[i].insertValue(j, M[i * C + j]);
				}
		}

	}

	void createColumnMatrixFromDenseMatrix(float* M, int R, int C){
		this->clear();

		for (int i = 0; i < R; i++)
 			for (int j = 0; j < C; j++)
				if (M[i * C + j] > 0){
					this->Row[j].insertValue(i, M[i * C + j]);
					this->Row[j].GlobalIndex = j;
				}

	}

	void createFromSparseMatrix(vsMatrix* M){
		this->clear();

		vsSparseMatrixElement* e;

		for (int i = 0; i < M->NoRows; ++i){
			this->Row[i].GlobalIndex = M->Row[i].GlobalIndex; 

			for (int j = 0; j < M->Row[i].NoElements; ++j){
				e = M->Row[i].Element[j];
				if (e->Value > 0)
					this->Row[i].insertValue(e->Index, e->Value);
			}
		}
	}

	void transpose(vsMatrix* M){
		this->clear();

		vsSparseMatrixElement* e;

		for (int i = 0; i < M->NoRows; ++i){
			this->Row[i].GlobalIndex = M->Row[i].GlobalIndex; 

			for (int j = 0; j < M->Row[i].NoElements; ++j){
				e = M->Row[i].Element[j];
				if (e->Value > 0){
					this->Row[e->Index].insertValue(i, e->Value);
				}
			}
		}
	}

	void createFullMatrixFromSparseMatrix(float* M){
		vsSparseMatrixElement* e;
/*
		for (int i = 0; i < this->NoRows; ++i)
			for (int j = 0; j < this->NoRows; ++j)
				M[i * this->NoRows + j] = 0;
*/
		memset(M, 0, this->NoRows * this->NoRows * sizeof(float));

		for (int i = 0; i < this->NoRows; ++i)
			for (int j = 0; j < this->Row[i].NoElements; ++j){
				e = this->Row[i].Element[j];
				M[i * this->NoRows + e->Index] = e->Value;
			}
	}

	float getValue(int R, int C, int &next, int &nextindex){

		return this->Row[R].getValue(C, next, nextindex);
		//return this->Row[C].getValue(R);
	}

};


#endif
