#include "BPtree.h"
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <windows.h> 
#include <stdio.h>
using namespace std;

int main()
{
	cout << "***************" << endl;
	cout << "��ӭʹ�����ݿ�" << endl;
	SYSTEMTIME sys;                               //��ʾϵͳʱ��
	GetLocalTime(&sys);
	printf("%4d/%02d/%02d", sys.wYear, sys.wMonth, sys.wDay); cout << endl;
	printf("%02d:%02d:%02d.%03d ����%1d",  sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek); cout << endl;

	cout << "***************" << endl;
	flag:
	cout << "1���������ݿ�" << endl;
	cout << "2�����½����ݿ�" << endl;
	cout << "3��������" << endl;
	cout << "4�����˳�" << endl;
	char ch;
	string name,test;
	BPtree * Test = NULL;
	while (cin >> ch)                //���˵�
	{
		if (ch == '1')
		{
			cout << "������Ҫ�򿪵���������" << endl;
			cin >> name;
			
			Test = new BPtree(name.c_str(), false);
		}
		else if (ch == '2')
		{
			cout << "������Ҫ�½��������ļ�����" << endl;
			cin >> name;
			
			Test = new BPtree(name.c_str());
		}
		else if (ch == '3')
		{
			cout << "1�����޸�ϵͳʱ��" << endl;
			cout << "2�����޸����ݿ�����" << endl;
			cout << "3����������һ��" << endl;
			cin >> ch;
			if (ch == '1')
			{
				system("time");   //��bug
			}
			if (ch == '2')
			{
				cout << "XXX" << endl;
			}
			if (ch == '3')
			{
				goto flag;
			}
		}
		else if (ch == '4')
		{
			cout << "�ɹ��˳�" << endl;
			return 0;
		}
		else
		{
			cout << "�������" << endl;
			continue;
		}
		break;
	}
	cout << "**************************" << endl;         //�������ݿ�
	cout << "�ɹ��������ݿ⡪��" << name << endl;
	cout << "**************************" << endl;
	cout << "1������������" << endl;
	cout << "2����ɾ������" << endl;
	cout << "3�����޸�����" << endl;
	cout << "4������������" << endl;
	cout << "5��������" << endl;
	cout << "6�����˳�" << endl;
	
	char op;
	string key1, key2;
	while (cin >> op)
	{
		if (op == '1')
		{                             //�����¼
			//cout << "������Ҫ�����Key��Valueֵ���Կո����" << endl;
			ifstream ist("test001.txt");
			while (!ist.eof())
			{
				ist >> key1 >> key2;
				if (Test->Insert(key1.c_str(), key2.c_str()) == false)
				{
					cout << "����ʧ��" << endl;
				}
				/*cin >> key1 >> key2;
				if (Test->Insert(key1.c_str(), key2.c_str()) == false)
				{
				cout << "����ʧ��" << endl;
				}*/
				//cout << "�������" << endl;
				
			}
		}
		
		if (op == '2')
		{                             //ɾ����¼
			cout << "������Ҫɾ�����ݵ�Keyֵ" << endl;
			cin >> key1;
			Test->Delete(key1);
			cout << "ɾ���ɹ�" << endl;
		}
		if (op == '3')                   //�޸ļ�¼
		{
			cout << "������Ҫ�޸ĵ�Key��Valueֵ���Կո����" << endl;
			cin >> key1 >> key2;
			if (Test->Replace(key1.c_str(), key2.c_str()) == true)
			{
				cout << "�޸ĳɹ�" << endl;
			}
			else
			{
				cout << "�޸�ʧ��" << endl;
			}
		}
		if (op == '4')                    //���Ҽ�¼
		{
			cout << "������Ҫ���ҵ����ݵ�Keyֵ" << endl;
			cin >> key1;
			string *tag = Test->Find(key1.c_str());  //����Ŀ���¼
			if (tag == NULL)
			{
				cout << "��¼������" << endl;
			}
			else
			{
				cout << *tag << endl; //��֮���valueֵ
			}
		}
		if (op == '5')                                    //�������ݿ�
		{   
			/*cout << "1�����������" << endl;
			cout << "2����ɾ������" << endl;
			cout << "3�������Ҳ���" << endl;
			cout << "4�����滻����" << endl;*/

			clock_t start, finish,start1,finish1;
			ifstream ist("test002.dat");
			start = clock();
			while (!ist.eof())
			{
				ist >> key1 >> key2;
				if (Test->Insert(key1.c_str(), key2.c_str()) == false)
				{
					cout << "����ʧ��" << endl;
				}
				//cout << "�������" << endl;
				/*else
				{
				cout << "����ɹ�" << endl;
				}*/
			}
			finish = clock();
			cout << "ȫ������ɹ�" << endl;
			cout << "Running time is: " << static_cast<double>(finish - start) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
			
			//test
			/*ifstream istd("test08.dat");
			start1 = clock();
			while (!istd.eof())
			{
				istd >> key1 >> key2;
				Test->Replace(key1.c_str(), key2.c_str()) == true;
			}
			finish1 = clock();
			cout << "�滻�ɹ�" << endl;
			cout << "Running time is: " << static_cast<double>(finish1 - start1) / CLOCKS_PER_SEC * 1000 << "ms" << endl;*/
		}
		if (op == '6')
		{            //�˳�
			cout << "���������˳������Ե�..." << endl;
			break;
		}
		
		//Test->printT();
	}
	delete Test;
	return 0;
}