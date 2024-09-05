//��������� ������� �����

#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <iomanip>
#include <vector>
using namespace std;

class MyConsole
{
public:
   int ReadDouble(double& num);
   int ReadString(std::string& inputString);
};

int MyConsole::ReadString(std::string& inputString)
{
    getline(std::cin, inputString);
	while (inputString.length()==0)
		getline(std::cin, inputString);
	return 0;
}

int MyConsole::ReadDouble(double& num)
{
	 /*����� ��� �� ����� ������ �������� � �������, ������� ������ ����� ������
	 ������ ������ ��� ������� ����� � ����� ��� �������
	cin>>num;
	while (cin.fail())
	{
		cin.clear();
		cin.ignore(32767,'\n');
		cout<<"�������� ��������, ������� ��� ���: ";
		cin>>num;
	}
	cin.clear();
	cin.ignore(32767,'\n');
	*/

	std::string inputString;
	getline(std::cin, inputString);
	while (inputString.length()==0)
		getline(std::cin, inputString);

	int i;
	for (i=0; i<inputString.length(); i++)
	{
		if (inputString[i]==',')
			inputString[i]='.'; 		
	}
	num = atof(inputString.c_str());

	return 0;
}

class Record
{
public:
	char monthAndYear[300];
//1
	double water_cnt; // ������ ���������� ����������� �����(�� ��������)
	double water_cost; // ��������� ������ ����
	double  water_subscription_fee; // ���������
	double water_full_cost; //������ ��������� ����� �� ����
//2
	double gas_cnt; // ������ ���������� ����������� �����(�� ��������)
	double gas_cost; // ��������� ������ ����
	double  gas_subscription_fee; //���������
	double gas_full_cost;
//3
	double electricityNight_cnt;
	double electricityNight_cost;
	double electricityNight_full_cost;

	double electricityDay_cnt;
	double electricityDay_cost;
	double electricityDay_full_cost;

	//float electricityNightAndDay_full_cost;
//4 internet
	double tenet_full_cost;
//5
	double kievstar_full_cost;
//6
	double garbage_full_cost;

//	int LoadFromFile(char* fileName);
//	int WriteToFile(char* fileName);
};

const char* FILE_NAME = "uslugi_danniye";
const char* TEMP_FILE_NAME = "uslugi_danniye_tempcopy";
const char* FILE_NAME_WAS_REMOVED = "uslugi_danniye_wasremoved"; // ��������� ����, ���� ����������, ����������, ��� ������� ����(FILE_NAME) ��� �����
																 // � �������� ��������������, � ��������, ��������� ��� �������������� �� ����� TEMP_FILE_NAME
void CreateEmptyFile(const char* aFileName)
{
	ofstream oFile(aFileName);
	if (!oFile.good())
	{
		cout<<"�������� ������� ���������� �����: ������ ��� �������� ����� "<<aFileName<<" �� ������\n";
		throw 10 ;
	} 
}

bool IsFileExist(const char* aFileName)
{
	bool bFileExist=true;
	ifstream iFile(aFileName);
	if (!iFile.good())
	{
		if (errno != 2) //����� ������ ����� FileNotFoundError
		{
			cout<<"�������� �� �������������: ������ �������� ����� "<<aFileName<<"�� ������";
			throw 10; // 10 -��������� �����, ���� �� ����� �������� ������ 
		}
		// ���� ������ FileNotFoundError, �� ���� ���� ���� ������ �� ����������
		bFileExist=false;
	} 
	return bFileExist;
}

void RestoreDataFilesAfterCrash()
{
	bool bMainFileExist;
	bool bTempMainFileExist;
	bool bTempWasRemovedFileExist;
	
	//��������� �� ������������� �������� ����� ������
	bMainFileExist = IsFileExist(FILE_NAME);
	//��������� �� ������������� �����(����������) �������� ����� ������
	bTempMainFileExist = IsFileExist(TEMP_FILE_NAME);
	//��������� �� ������������� �����(����������) �������� ����� ������
	bTempWasRemovedFileExist = IsFileExist(FILE_NAME_WAS_REMOVED);

	//���� ������� ���� �� ����������, � ����� ���������� � ���������� ���� FILE_NAME_WAS_REMOVED, ������ ��� ����� ����, ����� ������� ���� �������, � �������������
	//��������� �� ������. �������������, ��������������� ����� � �������
	if (!bMainFileExist && bTempMainFileExist && bTempWasRemovedFileExist)
	{
		const char* old_name = TEMP_FILE_NAME; // ������ �������� �����
		const char* new_name = FILE_NAME; // ����� �������� �����

		if (std::rename(old_name, new_name) != 0) {
			cout<<"������ �������������� ����� "<<old_name<<" errno="<<errno<<"\n";
			throw 10 ;
		}
		std::remove(FILE_NAME_WAS_REMOVED);
	}
}

class RecordsFile
{
public:
	RecordsFile();
	//Record rec[1000];
	vector<Record> mRec;
	int mRec_count;

	int LoadFromFile();
	int WriteToFile();

	int InputNewRecord();
	int PrintReport(int numberRepFromEnd); 
	int DeleteLastRecord();
};

RecordsFile::RecordsFile()
{
	mRec_count = 0;
}

int RecordsFile::LoadFromFile()
{
	bool createNewFile = 0;
	{//�������������� �������� ������ �������, ������ ��� ���� ���� iFile �������� �� ������ �� ������� ���������
     //� �������� �������� ����, � ������� ������ �������� �� ����� �����
		ifstream iFile(FILE_NAME);
     	if (!iFile.good())
		{
			if (errno != 2) //����� ������ ����� FileNotFoundError
			{
				cout<<"������ �������� ����� "<<FILE_NAME<<"�� ������";
				throw 10; // 10 -��������� �����, ���� �� ����� �������� ������ 
			}
		    // ���� ������ FileNotFoundError
			cout<<"���� "<<FILE_NAME<<" �� ������\n";		
			cout<<"������� ����� ����(����� 1) ��� ������ ������ � ������ ������ ������(����� ����� ������ �����)?\n";
			int l;
			cin>>l;
			if (l!=1)
			{
				throw 10; 
			}
			else
			{
				createNewFile = 1;
		    }		
		} 
		if (createNewFile == 0) // ���� �� ����� ��������� ����� ����
		{
			iFile>>mRec_count;
			int i;
			Record curRec;
			// ������� mRec
			mRec.clear();
			for (i=0; i<mRec_count;i++)
			{   
				iFile>>curRec.monthAndYear;
				//1 water
				iFile>>curRec.water_cnt;
				iFile>>curRec.water_cost; 
				iFile>>curRec.water_subscription_fee; 
				//2 gas
				iFile>>curRec.gas_cnt;
				iFile>>curRec.gas_cost;
				iFile>>curRec.gas_subscription_fee;
				//3 electricity
				iFile>>curRec.electricityNight_cnt;
				iFile>>curRec.electricityNight_cost;
				iFile>>curRec.electricityDay_cnt;
				iFile>>curRec.electricityDay_cost;
				//4 tenet(internet)
				iFile>>curRec.tenet_full_cost;
				//5 kievstar
				iFile>>curRec.kievstar_full_cost;
				//6 garbage
				iFile>>curRec.garbage_full_cost;

				mRec.push_back(curRec);
			}
		}
	}
	if (createNewFile)
	{
		mRec_count = 0;
		WriteToFile();
	}
	return 0;
}

int RecordsFile::WriteToFile()
{
	//���������� ����������: ���� ��������� ������ ������ ������ ��� ������ � ����(��� ���������� �����, ��������, ��������� ����)
	//���������� �� ����� �� ���������, � ����� ��������������� ��� � �������
	{//��� ������ �������, ���� ������ ���� ������ �� ������
		ofstream oFile(TEMP_FILE_NAME);
		if (!oFile.good())
		{
			cout<<"������ ��� �������� ����� "<<TEMP_FILE_NAME<<" �� ������\n";
			throw 10 ;
		} 

		oFile<<mRec_count<<"\n";
		int i;
		for (i=0; i<mRec_count;i++)
		{
			oFile<<mRec[i].monthAndYear<<"\n";

			 //1 water
			oFile<<setprecision(10)<<mRec[i].water_cnt<<" ";
			oFile<<setprecision(10)<<mRec[i].water_cost<<" "; 
			oFile<<setprecision(10)<<mRec[i].water_subscription_fee<<"\n"; 
			//2 gas
			oFile<<setprecision(10)<<mRec[i].gas_cnt<<" ";
			oFile<<setprecision(10)<<mRec[i].gas_cost<<" ";
			oFile<<setprecision(10)<<mRec[i].gas_subscription_fee<<"\n";
			//3 electricity
			oFile<<setprecision(10)<<mRec[i].electricityNight_cnt<<" ";
			oFile<<setprecision(10)<<mRec[i].electricityNight_cost<<" ";
			oFile<<setprecision(10)<<mRec[i].electricityDay_cnt<<" ";
			oFile<<setprecision(10)<<mRec[i].electricityDay_cost<<"\n";
			//4
			oFile<<setprecision(10)<<mRec[i].tenet_full_cost<<"\n";
			//5
			oFile<<setprecision(10)<<mRec[i].kievstar_full_cost<<"\n";
			//6
			oFile<<setprecision(10)<<mRec[i].garbage_full_cost<<"\n";
		}
	}

	const char* old_name = TEMP_FILE_NAME; // ������ �������� �����
    const char* new_name = FILE_NAME; // ����� �������� �����
	if (IsFileExist(new_name))
	{
		CreateEmptyFile(FILE_NAME_WAS_REMOVED);
		std::remove(new_name); //rename �� ��������� ������������� � ������������ ����,
	                       //������� ��� ���� �������������� �������. � ��� - ������ �����. �������� � ������ �������������
	                       //����������� �������. ��������� ����� ���������� � ���� �����(��������� ����), ����� �������� ����� ������,
	                       //�� �� �������������� ���������� � �������. �����, �������, ����������� �� ������ ����, ��� �����������
	                       //����� �� �������� ���������, �� �� �������� ���� ������ ���� ���������� ����. ������� � ������ ������� ��������� � ������ �������������� ����� ����
	                       //�� ������ ��������� ������ ���� ������: ���� �������(�������) ���� �� ����������, � ��������� ���� ����������,
	                       //� FILE_NAME_WAS_REMOVED ���������� ����, �� ����� ������������� ���� ��������� � �������
	}
	if (std::rename(old_name, new_name) != 0) {
		cout<<"������ �������������� ����� "<<old_name<<" errno="<<errno<<"\n";
		throw 10 ;
    }
	std::remove(FILE_NAME_WAS_REMOVED);
	return 0;
}

int RecordsFile::InputNewRecord()
{
		Record recTmp;
		MyConsole con;	

		cout<<"����� ����� � ��� ������(���������� �������): ";
		//cin>>recTmp.monthAndYear;
		std::string inputString;
		con.ReadString(inputString);
		int i;
		for (i=0; i<inputString.length(); i++)
		{
			if (inputString[i]!=' ')
			    recTmp.monthAndYear[i] = inputString[i]; 
			else
				recTmp.monthAndYear[i] = '-';
		}
		recTmp.monthAndYear[i]='\0';
	
        double temp;
		 
		cout<<"1. ����\n";
		cout<<"�� ��������: ";
		//cin>>recTmp.water_cnt;
		con.ReadDouble(recTmp.water_cnt);
        cout<<"���� �� ���(���� 0, �������� �� ����������� ������): ";		  
		con.ReadDouble(temp);
		//cin>>temp;
		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].water_cost;
		recTmp.water_cost = temp;
		cout<<"���������(���� 0, �������� �� ����������� ������): ";
		//cin>>temp;
		con.ReadDouble(temp);
		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].water_subscription_fee;
		recTmp.water_subscription_fee = temp;
	
        cout<<"2. ���\n";
		cout<<"�� ��������: ";
		//cin>>recTmp.gas_cnt;
		con.ReadDouble(recTmp.gas_cnt);
        cout<<"���� �� ���(���� 0, �������� �� ����������� ������): ";		  
		//cin>>temp;
		con.ReadDouble(temp);
		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].gas_cost;
		recTmp.gas_cost = temp;

		cout<<"���������(�������������)(���� 0, �������� �� ����������� ������): ";
		//cin>>temp;
		con.ReadDouble(temp);
		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].gas_subscription_fee;
		recTmp.gas_subscription_fee = temp;
		
		cout<<"3. �������������\n";
		cout<<"�� ��������(����): ";
		//cin>>recTmp.electricityNight_cnt;
		con.ReadDouble(recTmp.electricityNight_cnt);
        cout<<"���� �� �������(����)(���� 0, �������� �� ����������� ������): ";		  
		//cin>>temp;
		con.ReadDouble(temp);

		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].electricityNight_cost;
		recTmp.electricityNight_cost = temp;
	
		cout<<"�� ��������(����): ";
		//cin>>recTmp.electricityDay_cnt;
		con.ReadDouble(recTmp.electricityDay_cnt);
        cout<<"���� �� �������(����)(���� 0, �������� �� ����������� ������): ";		  
		//cin>>temp;
		con.ReadDouble(temp);
		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].electricityDay_cost;
		recTmp.electricityDay_cost = temp;
		
		cout<<"4. �����(��������)\n";
        cout<<"���������(���� 0, �������� �� ����������� ������): ";		  
		//cin>>temp;
		con.ReadDouble(temp);
		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].tenet_full_cost;
		recTmp.tenet_full_cost = temp;

		cout<<"5. ��������\n";
        cout<<"���������(���� 0, �������� �� ����������� ������): ";		  
		//cin>>temp;
		con.ReadDouble(temp);

		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].kievstar_full_cost;
		recTmp.kievstar_full_cost = temp;
	
		cout<<"6. �����\n";
        cout<<"���������(���� 0, �������� �� ����������� ������): ";		  
		// cin>>temp;
		con.ReadDouble(temp);
		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].garbage_full_cost;
		recTmp.garbage_full_cost = temp;
		
		mRec_count++;
		mRec.push_back(recTmp);
		  
		WriteToFile();	
		return 0;
}

//numberRep - ����� ������ ���������� � �����, � � ����
int RecordsFile::PrintReport(int numberRepFromEnd)
{
	if (mRec_count<=0)
	{
		cout<<"������ ������� ����, ���� ������ ��������";
		return 1;
	}
	int numberRep = mRec_count-1-numberRepFromEnd;
	if ((numberRep<0) || (numberRep>=mRec_count))
	{
		cout<<"����� ������ ��� ��������� ���������� �������";
		return 1;
	}

	bool bIsFirst; // ��� ����� ������ ������ ��� ���
	Record cur, prev;
	if (numberRep > 0)
	{
		cur = mRec[numberRep];
		prev = mRec[numberRep-1];
		bIsFirst = false;
	}
	else
	{
		cur = mRec[numberRep];
		prev = mRec[numberRep];
		bIsFirst = true;
	}

	double water_total, gas_total, electricity_total;

	cout<<"����� �� "<<cur.monthAndYear<<"\n";
	if (bIsFirst) cout<<"��� ����� ������ �����, ������� ��������� ������ ������, ��� ���������\n";

	cout<<"1. ����\n";
	cout<<"   �� ��������: "<<cur.water_cnt<<"\n";
    cout<<"   ���� �� ���: "<<cur.water_cost<<"\n";
	cout<<"   ���������: "<<cur.water_subscription_fee<<"\n";
	if (!bIsFirst)
	{
		water_total = (cur.water_cnt - prev.water_cnt)*cur.water_cost+cur.water_subscription_fee;
		cout<<"   �����: "<<water_total<<"\n";
	}

	cout<<"2. ���\n";
	cout<<"   �� ��������: "<<cur.gas_cnt<<"\n";
    cout<<"   ���� �� ���: "<<cur.gas_cost<<"\n";
	cout<<"   ���������: "<<cur.gas_subscription_fee<<"\n";
	if (!bIsFirst)
	{
		gas_total = (cur.gas_cnt - prev.gas_cnt)*cur.gas_cost+cur.gas_subscription_fee;
		cout<<"   �����: "<<gas_total<<"\n";
	}

	cout<<"3. �������������\n";
	cout<<"   �� ��������(����): "<<cur.electricityNight_cnt<<"\n";
	cout<<"   ���� �� �������(����): "<<cur.electricityNight_cost<<"\n";		  
	cout<<"   �� ��������(����): "<<cur.electricityDay_cnt<<"\n";
	cout<<"   ���� �� �������(����): "<<cur.electricityDay_cost<<"\n";	
	if (!bIsFirst)
	{
		electricity_total = (cur.electricityNight_cnt - prev.electricityNight_cnt)*cur.electricityNight_cost
		+ (cur.electricityDay_cnt - prev.electricityDay_cnt)*cur.electricityDay_cost;
		cout<<"   �����: "<<electricity_total<<"\n";
	}

	cout<<"4. �����(��������)\n";
	cout<<"   ���������: "<<cur.tenet_full_cost<<"\n";
	if (!bIsFirst)
	{
		cout<<"   �����: "<<cur.tenet_full_cost<<"\n";
	}

	cout<<"5. ��������\n";
    cout<<"   ���������: "<<cur.kievstar_full_cost<<"\n";		  
    if (!bIsFirst)
	{
	cout<<"   �����: "<<cur.kievstar_full_cost<<"\n";
	}

    cout<<"6. �����\n";
	cout<<"   ���������: "<<cur.garbage_full_cost<<"\n";
	if (!bIsFirst)
	{
		cout<<"   �����: "<<cur.garbage_full_cost<<"\n";
	}

	if (!bIsFirst)
	{
		cout<<"����� �� �Ѩ: "<<water_total+gas_total+electricity_total+cur.tenet_full_cost+cur.kievstar_full_cost+cur.garbage_full_cost<<"\n";
	}
}

int RecordsFile::DeleteLastRecord()
{
	int ok;
	cout<<"����� ������ ������� ��������� ������?(0-���, 1-��): ";
	cin>>ok;
	if (ok==1)
	{
		if (mRec_count>0)
		{
			mRec_count--;
		    mRec.pop_back();
			WriteToFile();
		}
	}
	return 0;
}

void SystemInitialisation()
{
    setlocale(LC_CTYPE, "");
	//setlocale(LC_ALL, "Russian");
	
	//std::locale::global(std::locale("rus_rus.866"));
    //std::cin.imbue(std::locale());


	//��������
	//cin.imbue(locale("rus_rus.866"));
	//cin.imbue(locale(".866"));
	//setlocale(LC_ALL, ".866");
	//setlocale(LC_ALL, "ru_RU_cp1251");
   	/*	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "Russian");*/

//	std::ios_base::sync_with_stdio(false);
//    std::cin.imbue(std::locale());	

/*	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_FONT_INFOEX fontInfo;
    fontInfo.cbSize = sizeof(fontInfo);
    GetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);

	cout<<fontInfo.dwFontSize.X<<" "<<fontInfo.dwFontSize.Y;
    fontInfo.dwFontSize.X = 12;  // ����������� ������ ������ � ��� ����
    fontInfo.dwFontSize.Y = 18;  // ����������� ������ ������ � ��� ����

    SetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);*/

	/*HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // �������� �������

CONSOLE_FONT_INFOEX fontInfo;
GetCurrentConsoleFontEx(hConsole, TRUE, &fontInfo); // �������� ������� �����

// �������� �����-�� ���������
//wcsncpy(L"Arial Cyr", fontInfo.FaceName, LF_FACESIZE);  // ���
fontInfo.dwFontSize.X = 10; // ������ (� ���������� ��������)

SetCurrentConsoleFontEx(hConsole, TRUE, &fontInfo); // ���������� �����

*/
}

void Run()
{
	cout<<"��������� ������� �����\n";
    //������� ����� ����� ������� � ����� �����, � ����� �������, ��� �������
	
	// ��������� ������ �� ����� ������
	RecordsFile r;
	r.LoadFromFile();

	int choise;
	do
	{
		cout<<"\n0-�����, 1-������ ������ �� ����� �����, 2-������� ��������� �����\n"
		<< "3-������� ��������� ������, 4-������� ��������� N �������\n";

	   cin>>choise;
	   if (choise == 1)
       {
		   r.InputNewRecord();
       }
	   else if (choise ==2) 
	   {
	        r.PrintReport(0);//���� ���������   
	   }
	   else if (choise ==3) 
	   {
	        r.DeleteLastRecord();	   
	   }
	   else if (choise ==4) 
	   {
		   cout<<"����� N: ";
		   int N,i;
		   cin>>N;
		   if ( (N>r.mRec_count) || (N<1))
			   N=r.mRec_count;
		   for (i=0; i<N; i++)
		   {
			   r.PrintReport(N-i-1);
			   cout<<"\n";
		   }
	   }
	}
	while (choise>0);
}

int main()
{
	SystemInitialisation(); //������
	RestoreDataFilesAfterCrash();//��� ������ ������� ��������� ���������� � ������� �� �������� ����� �������� �������� ������
	
	try
	{
		Run();
	}
	catch (...)
	{
		int exit;
		cout<<"\n�������� ������ � ���������, ������� ����� ����� ��� ������";
		cin>>exit;
	}

	return 0;
}

