#ifndef _BTREE_H_
#define _BTREE_H_

#include <iostream>
#include <string>
using namespace std;

#define DATSUFFIX ".dat" //�����ļ�
#define IDXSUFFIX ".idx"  //�����ļ�

#define BT_M 16            //B+������
#define BN_MID 0          //���Ҷ�Ӻͷ�Ҷ�ӽڵ�
#define BN_LEAF 1

#define MAXDATLEN 50      //����data����󳤶�
#define MAXKEYLEN 10       //����keyֵ����󳤶�

class BPtree
{
	//��Ҫ���ܺ���
public:
	BPtree(string dtn, bool newfile = true);             //��ʼ��
	~BPtree();                                           //��������
	bool Insert(const string &, const string &);		//�ж��Ƿ����ɹ�
	bool Delete(const string &);                     //�ж��Ƿ�ɾ���ɹ�
	string * Find(const string &)const;              //���ҵļ�¼�Ƿ�Ϊ��
	bool Replace(const string &, const string &);	    //�ж��Ƿ��޸ĳɹ�

private:
	string name;  //���ݿ������
	FILE * Fstream;  //�ļ���
	
	typedef struct Bdata
	{
		int len;              //������ݳ���
		char dat[MAXDATLEN];  
	}BDAT;

	struct SNode   //��������ѡ�õĽṹ��
	{
		char type;
		int s;
		char key[BT_M + 1][MAXKEYLEN];
		long long offset[BT_M + 1];       //��keyֵ��Ӧ
	};

	class MidNode;

	class BNode       //�ڵ���
	{
	public:
		int species;
		int size;
		MidNode * father;
		BNode(){ size = 0; father = NULL; }
		string key[BT_M + 1];   //keyֵ
	};

	BNode * bRoot;  

	class MidNode :public BNode //��Ҷ�ڵ�
	{
	public:
		BNode *child[BT_M + 2];                    //�������飬������key��1
		MidNode(){ species = BN_MID; }          
	};

	class DataNode :public BNode    //Ҷ�ӽڵ�
	{
	public:
		long long offset[BT_M + 1];
		DataNode(){ species = BN_LEAF; }
	};

	void writeintofile(FILE * stream, long long offset, const BDAT * val)   //������д���ļ�
	{
		long long curpos;  
		curpos = ftell(stream); 
		fseek(stream, offset, SEEK_SET);                      //��λ��д���ݵĵ�ַ
		fwrite(val, sizeof(BDAT), 1, stream);                  //������д������
		fseek(stream, curpos, SEEK_SET);                      //ָ��ָ��ĩβ           
		return;
	}

	void getintofile(FILE * stream, long long offset, BDAT * val) const  //��ȡ����
	{
		long long curpos;
		curpos = ftell(stream);
		fseek(stream, offset, SEEK_SET);                    //��λ�������ݵĵ�ַ
		fread(val, sizeof(BDAT), 1, stream);                //�����������
		fseek(stream, curpos, SEEK_SET);                         
		return;
	}

	void writeindex(FILE * stream, const BNode * nNode)   //������д���ļ�
	{
		long long curpos;
		curpos = ftell(stream);
		SNode Temp;                         //�ļ��ﴢ��������node
		if (nNode->species == BN_MID)      //����Ƿ�Ҷ�ڵ�
		{
			fwrite(&Temp, sizeof(SNode), 1, stream);
			MidNode * tmpNode = (MidNode *)nNode;        //��Ҫд��������node
			Temp.type = 0;
			Temp.s = tmpNode->size;
			int i;
			for (i = 0; i<tmpNode->size; i++)
			{
				strcpy_s(Temp.key[i], tmpNode->key[i].c_str());  //����Ҫд���ļ���������key���������snode
			}
			for (i = 0; i <= tmpNode->size; i++)
			{
				Temp.offset[i] = ftell(stream);
				writeindex(stream, tmpNode->child[i]);      //�ݹ�д���ļ�
			}

			fseek(stream, curpos, SEEK_SET);
			fwrite(&Temp, sizeof(SNode), 1, stream);
		}
		else if (nNode->species == BN_LEAF)
		{
			DataNode * tmpNode = (DataNode *)nNode;
			Temp.type = 1;
			Temp.s = tmpNode->size;
			int i;
			for (i = 0; i<tmpNode->size; i++)
			{
				strcpy_s(Temp.key[i], tmpNode->key[i].c_str());        //�����Ҷ�ӽڵ�ֱ�Ӷ���������keyֵ��λ����Ϣ
				Temp.offset[i] = tmpNode->offset[i];
			}
			fwrite(&Temp, sizeof(SNode), 1, stream);
		}
		fseek(stream, 0L, SEEK_END);                          //����ָ���ļ�ĩβ
		return;
	}

	BNode * getindex(FILE * stream, long long offset)       //���ļ��������������������������Ĺ���
	{
		SNode Temp;
		fseek(stream, offset, SEEK_SET);
		fread(&Temp, sizeof(SNode), 1, stream);    //�����ж�һ������                 
		if (Temp.type == 0)                        //����Ƿ�Ҷ�ڵ�
		{
			MidNode * retNode = new MidNode;        //�½��ڵ�
			retNode->species = BN_MID;
			retNode->size = Temp.s;
			int i;
			for (i = 0; i<retNode->size; i++)
			{
				retNode->key[i] = Temp.key[i];         //����keyֵ
			}
			for (i = 0; i <= retNode->size; i++)
			{
				retNode->child[i] = getindex(stream, Temp.offset[i]);  //�ݹ������ȡ����ֱ��Ҷ�ӽڵ�
				retNode->child[i]->father = retNode;
			}
			return retNode;
		}
		else
		{                                                         //�����Ҷ�ڵ�
			DataNode * retNode = new DataNode;
			retNode->species = BN_LEAF;
			retNode->size = Temp.s;
			int i;
			for (i = 0; i<retNode->size; i++)
			{
				retNode->key[i] = Temp.key[i];               //����keyֵ��λ����Ϣ
				retNode->offset[i] = Temp.offset[i];
			}
			return retNode;
		}
	}

	long long adddat(FILE * stream, const BDAT * val)  //�ļ�β�������
	{                                                              
		long long curpos, length;
		curpos = ftell(stream);
		fseek(stream, 0L, SEEK_END);
		length = ftell(stream);
		fwrite(val, sizeof(BDAT), 1, stream);
		fseek(stream, curpos, SEEK_SET);
		return	length;
	}

	void freemem(BNode * fNode)        //�ͷŽڵ��ڴ�ĺ���
	{
		if (fNode->species == BN_LEAF)   //Ҷ�ӽڵ�
		{
			delete fNode;          //ɾ��
			return;
		}
		int i;
		for (i = 0; i <= fNode->size; i++)
			freemem(((MidNode *)fNode)->child[i]);
		delete fNode;
		return;
	}
	
};

#endif