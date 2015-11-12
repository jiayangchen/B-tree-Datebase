
#include <iostream>
#include <string>
#include "BPtree.h"

using namespace std;

//B����ʵ�ֺ���

BPtree::BPtree(string Dname, bool judgefile) :name(Dname)
{
	if (judgefile == true) 
	{
		bRoot = new DataNode;
		Fstream = fopen((name + DATSUFFIX).c_str(), "wb+"); //�½���ǰĿ¼�µ�name�ļ�������һ���ļ��Ķ�д
	}
	else  
	{
		FILE * Idxfile;
		Idxfile = fopen((name + IDXSUFFIX).c_str(), "rb");   
		bRoot = getindex(Idxfile, 0);                    //���ļ����Ŀ�ͷ���������ڴ�
		fclose(Idxfile); //�ر��ļ���
		Fstream = fopen((name + DATSUFFIX).c_str(), "rb+");
	}
}

BPtree::~BPtree()
{
	FILE * Idxfile;
	Idxfile = fopen((name + IDXSUFFIX).c_str(), "wb");     //����д���ļ�����
	writeindex(Idxfile, bRoot);     //д�뺯��
	fclose(Idxfile);  //�ر������ļ���
	freemem(bRoot);   //�ڴ��ͷ�
	fclose(Fstream);  //�ر���
}

bool BPtree::Insert(const string & newKey, const string & Newdat)  //����ʵ��
{
	BNode * nNode = bRoot;                                      //�������ڵ�
	while (nNode->species == BN_MID)                            //��node�������Ƿ�Ҷ�ӽڵ�ʱ
	{                                                          //�ݹ�Ѱ��Ҷ�ڵ�
		MidNode * nMid = (MidNode *)nNode;
		int i;
		for (i = 0; i<nMid->size; i++)
		{
			if (newKey<nMid->key[i])                          //ѭ���ҵ���Ŀ���ֵС�ĵ�һ����ֵ
				break;                                        //���ҵ�������
		}
		nNode = nMid->child[i];                                
	}
	if (nNode->species == BN_LEAF)                             //node�������Ѿ���Ҷ�ڵ�
	{
		DataNode * nLeaf = (DataNode *)nNode;                 
		int i, pos;                                             
		for (pos = 0; pos<nLeaf->size; pos++)
		{
			if (newKey < nLeaf->key[pos])                     //�ҵ�Ҷ�ڵ��еĲ���ص�si
				break;
			if (newKey == nLeaf->key[pos])                    //�������ʹ��ڲ���Ҫ����
				return false;
		}
		for (i = nLeaf->size; i>pos; i--)
		{
			nLeaf->key[i] = nLeaf->key[i - 1];             //��Ҫ����������ڳ��ռ�
			nLeaf->offset[i] = nLeaf->offset[i - 1];       //����ڳ����ļ��е�λ��
		}
		nLeaf->key[pos] = newKey;                           //����

		BDAT Predat;                                       
		Predat.len = int(Newdat.length());                 //��ò������ݵĳ���
		strcpy(Predat.dat, Newdat.c_str());                //����д��
		nLeaf->offset[pos] = adddat(Fstream, &Predat);       //offset������ļ����λ��
		nLeaf->size++;                                     //size+1

		//���ǲ�����ƽ������

		if (nLeaf->size == BT_M + 1)                      //�������leafsize
		{
			int cut = (BT_M + 1) / 2;                     //���м��Ԫ��Ϊ�ֽ磬����Ҷ�ڵ�
			DataNode * Rleaf = new DataNode;              //�½��ҽڵ�   
			nLeaf->size = cut;
			Rleaf->size = BT_M + 1 - cut;                //�����ҽڵ��size
			for (i = cut; i <= BT_M; i++){
				Rleaf->key[i - cut] = nLeaf->key[i];      //�����ڵ㵽�µ��ҽڵ�
				Rleaf->offset[i - cut] = nLeaf->offset[i];  //�ļ����λ��
			}
			
			MidNode * insertNode = nLeaf->father;
			string insertKey = nLeaf->key[cut];           //���м�Ԫ���ϵ������ڵ�
			BNode * lNode = nLeaf;
			BNode * rNode = Rleaf;
			
			if (insertNode == NULL)                      //����Ǹ��ڵ�
			{
				insertNode = new MidNode;
				bRoot = insertNode;
			}
			while (1)                                   //�����Ҷ�ڵ�ͬ��
			{
				for (pos = 0; pos<insertNode->size; pos++)
				{
					if (insertKey<insertNode->key[pos]) //�ҵ�λ��
						break;
				}
				for (i = insertNode->size; i>pos; i--)
				{
					insertNode->key[i] = insertNode->key[i - 1];     //����
					insertNode->child[i + 1] = insertNode->child[i];
				}
				lNode->father = insertNode;         //����ָ�����ӵ����ڵ�
				rNode->father = insertNode;
				insertNode->key[pos] = insertKey;      
				insertNode->child[pos] = lNode;
				insertNode->child[pos + 1] = rNode;
				insertNode->size++;
				
				if (insertNode->size == BT_M + 1)                //����ԭ��������Ҷ�ڵ�
				{
					cut = (BT_M + 1) / 2;                      //ȡ�м�Ԫ��
					MidNode * rightMid = new MidNode;       
					insertNode->size = cut;
					rightMid->size = BT_M - cut;              //�µ��ҽڵ��size
					for (i = cut + 1; i <= BT_M; i++)
					{
						rightMid->key[i - cut - 1] = insertNode->key[i];          //�������µ��ҽڵ�
						rightMid->child[i - cut - 1] = insertNode->child[i];
						rightMid->child[i - cut - 1]->father = rightMid;
					}
					rightMid->child[i - cut - 1] = insertNode->child[i];
					rightMid->child[i - cut - 1]->father = rightMid;

					insertKey = insertNode->key[cut];
					lNode = insertNode;
					rNode = rightMid;
					
					if (insertNode->father == NULL)                     //�����µĸ��ڵ㣬��ԭ���ĸ��ڵ���Ϊ�¸��ڵ�Ķ���
					{
						insertNode = new MidNode;
						bRoot = insertNode;
					}
					else
					{
						insertNode = insertNode->father;
					}
				}
				else
				{
					return true;
				}
			}
		}
		else
		{
			return true;
		}
	}
	return true;         //���ز���Ҫ����
}

bool BPtree::Delete(const string & Tkey)
{
	BNode * nNode = bRoot;
	int i;
	while (1)
	{
		if (nNode->species == BN_MID)                          //���Ȼ����ҵ�Ҫɾ��Ҷ�ӽڵ�
		{
			MidNode * nMid = (MidNode *)nNode;
			for (i = 0; i<nMid->size; i++)
			{
				if (Tkey<nMid->key[i])                        //�ҵ�������
					break;
			}
			nNode = ((MidNode *)nNode)->child[i];
		}
		else if (nNode->species == BN_LEAF)                 //Ҷ�ӽڵ���
		{
			DataNode * nLeaf = (DataNode *)nNode;
			for (i = 0; i<nLeaf->size; i++)
			{
				if (Tkey < nLeaf->key[i])
					return false;
				if (Tkey == nLeaf->key[i])                 //�Ҵ�Ҫɾ��Ŀ��keyֵ
				{
					break;
				}
			}
			if (i == nLeaf->size)                          //ѭ����ĩβ����û���ҵ��򷵻�ʧ��
				return false;
			
			//����ɾ�������
			nLeaf->size--;                               
			for (; i<nLeaf->size; i++)                     //iָ����Ҫɾ��key��λ��
			{
				nLeaf->key[i] = nLeaf->key[i + 1];         //�����keyֵǰ��
				nLeaf->offset[i] = nLeaf->offset[i + 1];   //�ļ����λ�ñ��Ҳǰ��
			}
			if (nLeaf->father == NULL)                         //����Ǹ��ڵ���ɾ��
				return true;

			                                             
			int sti = BT_M / 2;                           //B+�����ʵ�����
			if (nLeaf->size<sti)                          //�鿴ɾ�����Ƿ�С��B+����Ҫ��
			{
				MidNode * deleteFrom = nLeaf->father;        //Ҷ�ڵ�ĸ��ڵ�            
				BNode * deleteChild = nLeaf;             //ɾ����Ҷ�ڵ�

				while (1)
				{
					int deleteI;
					int i;
					
					for (i = 0; i <= deleteFrom->size; i++)     //�ҵ�Ҫɾ�����Ǹ��ڵ��ָ��λ��
						if (deleteFrom->child[i] == deleteChild)
							break;
					deleteI = i;                                  //��¼����
					
					//��Ϊ���ڽڵ�Ԫ��̫�٣��������ߵ��ֵܽڵ��һ��Ԫ��
					if (deleteI > 0 && deleteFrom->child[deleteI - 1]->size>sti) //�������ֵܵ�keyֵ���������޴�����Խ��
					{
						BNode * lBrother = deleteFrom->child[deleteI - 1];        //ȡ��߽ڵ�������keyֵ
						for (i = deleteChild->size; i>0; i--)
							deleteChild->key[i] = deleteChild->key[i - 1];        //�ճ�ɾ���ڵ������ߵ�λ��
						
						if (deleteChild->species == BN_LEAF)
						{
							for (i = deleteChild->size; i>0; i--)
								((DataNode *)deleteChild)->offset[i] = ((DataNode *)deleteChild)->offset[i - 1];
							deleteChild->key[0] = lBrother->key[lBrother->size - 1];          //keyֵ����
							((DataNode *)deleteChild)->offset[0] = ((DataNode *)lBrother)->offset[lBrother->size - 1];
							deleteFrom->key[deleteI - 1] = deleteChild->key[0];  //���¸��ڵ������keyֵ
						}
						else
						{
							for (i = deleteChild->size + 1; i>0; i--)
							{
								((MidNode *)deleteChild)->child[i] = ((MidNode *)deleteChild)->child[i - 1];
								((MidNode *)deleteChild)->child[i]->father = (MidNode *)deleteChild;
							}
							deleteChild->key[0] = deleteFrom->key[deleteI - 1];
							deleteFrom->key[deleteI - 1] = lBrother->key[lBrother->size - 1];
							((MidNode*)deleteChild)->child[0] = ((MidNode *)lBrother)->child[lBrother->size];
							((MidNode*)deleteChild)->child[0]->father = (MidNode *)deleteChild;
						}
						deleteChild->size++;  //��ɽ�key
						lBrother->size--;    
						return true;
					}
					//���ұ��ֵܽ�
					if (deleteI < deleteFrom->size && deleteFrom->child[deleteI + 1]->size>sti)   
					{
						BNode * rBrother = deleteFrom->child[deleteI + 1];
						if (deleteChild->species == BN_LEAF)
						{
							deleteChild->key[deleteChild->size] = rBrother->key[0];
							((DataNode *)deleteChild)->offset[deleteChild->size] = ((DataNode *)rBrother)->offset[0];
							deleteFrom->key[deleteI] = rBrother->key[1];
							for (i = 1; i<rBrother->size; i++)
								((DataNode *)rBrother)->offset[i - 1] = ((DataNode *)rBrother)->offset[i];
						}
						else
						{
							deleteChild->key[deleteChild->size] = deleteFrom->key[deleteI];
							deleteFrom->key[deleteI] = rBrother->key[0];
							((MidNode *)deleteChild)->child[deleteChild->size] = ((MidNode *)rBrother)->child[0];
							((MidNode *)deleteChild)->child[deleteChild->size]->father = (MidNode *)deleteChild;
							for (i = 0; i<rBrother->size; i++)
								((MidNode *)rBrother)->child[i] = ((MidNode *)rBrother)->child[i + 1];
						}
						for (i = 1; i<rBrother->size; i++)
							rBrother->key[i - 1] = rBrother->key[i];
						deleteChild->size++;  //��ɽ�key
						rBrother->size--;
						return true;
					}

					//��������ֵܶ����ܽ������ɾ���Ľڵ����keyֵ�ϲ�����ڵ�
					if (deleteI>0)
					{
						BNode * lBrother = deleteFrom->child[deleteI - 1];
						if (lBrother->species == BN_LEAF)
						{
							for (i = 0; i<sti - 1; i++)
							{
								lBrother->key[sti + i] = deleteChild->key[i];      //����
								((DataNode *)lBrother)->offset[sti + i] = ((DataNode *)deleteChild)->offset[i];
							}
							lBrother->size += sti - 1;   //������ڵ��size
						}
						else
						{
							lBrother->key[sti] = deleteFrom->key[deleteI - 1];
							for (i = 0; i<sti; i++)
							{
								lBrother->key[sti + 1 + i] = deleteChild->key[i];
								((MidNode *)lBrother)->child[sti + 1 + i] = ((MidNode *)deleteChild)->child[i];
								((MidNode *)lBrother)->child[sti + 1 + i]->father = (MidNode *)lBrother;
							}
							lBrother->size += sti;
						}
						for (i = deleteI; i<deleteFrom->size; i++)        //����ԭ���ĸ��ڵ�
						{
							deleteFrom->key[i - 1] = deleteFrom->key[i];
							deleteFrom->child[i] = deleteFrom->child[i + 1];
						}
					}
					else if (deleteI < deleteFrom->size)
					{
						BNode * rBrother = deleteFrom->child[deleteI + 1];      //���Һϲ�
						if (rBrother->species == BN_LEAF)
						{
							for (i = 0; i<sti; i++)
							{
								deleteChild->key[sti - 1 + i] = rBrother->key[i];
								((DataNode *)deleteChild)->offset[sti - 1 + i] = ((DataNode *)rBrother)->offset[i];
							}
							deleteChild->size += sti;
						}
						else
						{
							deleteChild->key[sti - 1] = deleteFrom->key[deleteI];
							for (i = 0; i<sti + 1; i++)
							{
								deleteChild->key[sti + i] = rBrother->key[i];
								((MidNode *)deleteChild)->child[sti + i] = ((MidNode *)rBrother)->child[i];
								((MidNode *)deleteChild)->child[sti + i]->father = (MidNode *)deleteChild;
							}
							deleteChild->size += sti + 1;
						}
						for (i = deleteI + 1; i<deleteFrom->size; i++)
						{
							deleteFrom->key[i - 1] = deleteFrom->key[i];
							deleteFrom->child[i] = deleteFrom->child[i + 1];
						}
					}
					deleteFrom->size--;
		
					if (deleteFrom->father == NULL)       //������ڵ�Ϊ���ڵ�
					{
						if (deleteFrom->size == 0)
						{
							bRoot = deleteFrom->child[0];    //�½����ڵ�
							bRoot->father = NULL;
							delete deleteFrom;
						}
						return true;
					}
					else if (deleteFrom->size<sti)
					{
						deleteChild = deleteFrom;
						deleteFrom = deleteFrom->father;
					}
					else
					{
						return true;
					}
				}
			}
			else
			{
				return true;
			}
		}
	}
	return false;
}

string * BPtree::Find(const string & Tkey)const
{
	BNode * nNode = bRoot;
	int i;
	while (1)
	{
		if (nNode->species == BN_MID)                //����Ƿ�Ҷ�ӽ��
		{
			MidNode * nMid = (MidNode *)nNode;
			for (i = 0; i<nMid->size; i++)           //ѭ���ҵ���Ŀ���ֵ��ĵ�һ����ֵ
			{
				if (Tkey<nMid->key[i])                //���ҵ�������
					break;
			}
			nNode = ((MidNode *)nNode)->child[i];
		}
		else if (nNode->species == BN_LEAF)        //�������Ҷ�ӽڵ�
		{
			DataNode * nLeaf = (DataNode *)nNode;
			for (i = 0; i<nLeaf->size; i++)         //ѭ��Ѱ��Ŀ���ֵ
			{
				if (Tkey < nLeaf->key[i])           //ѭ��������С�ڵĻ�ֱ�ӷ���δ�ҵ�
					return NULL;
				if (Tkey == nLeaf->key[i])          //�ҵ���Ӧ��ֵ�Ļ�
				{
					BDAT Tdat;                   //Ŀ���������
					getintofile(Fstream, nLeaf->offset[i], &Tdat);     //���ļ����ҳ�����
					return (new string(Tdat.dat));     //����Ŀ������ֵ
				}
			}
			return NULL;      //��֮������ʧ��
		}
	}
}

bool BPtree::Replace(const string & Tkey, const string & Newdat)       //�滻
{
	BNode * nNode = bRoot;
	int i;
	while (1)
	{
		if (nNode->species == BN_MID)                        //�������Ȳ��ҵ�Ҫ�滻�ļ�ֵ����������
		{
			MidNode * nMid = (MidNode *)nNode;
			for (i = 0; i<nMid->size; i++)
			{
				if (Tkey<nMid->key[i])
					break;
			}
			nNode = ((MidNode *)nNode)->child[i];
		}
		else if (nNode->species == BN_LEAF)               //�Ѿ���Ҷ�ӽڵ�ʱ
		{
			DataNode * nLeaf = (DataNode *)nNode;
			for (i = 0; i<nLeaf->size; i++)
			{
				if (Tkey < nLeaf->key[i])              
					return false;
				if (Tkey == nLeaf->key[i])          //�������д
				{
					BDAT Predat;
					Predat.len = int(Newdat.length()); //�������ݵĳ����滻�����ݵĳ���
					strcpy(Predat.dat, Newdat.c_str());  //�����滻
					writeintofile(Fstream, nLeaf->offset[i], &Predat); //д���ļ�
					return true;
				}
			}
			return false;  //��֮ʧ��
		}
	}
}



