//программа расчета услуг

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
	 /*такой код не очень хорошо работает в виндовс, поэтому вводим целую строку
	 заодно делаем без разницы точка в числе или запятая
	cin>>num;
	while (cin.fail())
	{
		cin.clear();
		cin.ignore(32767,'\n');
		cout<<"неверное значение, введите ещё раз: ";
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
	double water_cnt; // полное количество накрученных кубов(на счётчике)
	double water_cost; // стоимость одного куба
	double  water_subscription_fee; // абонплата
	double water_full_cost; //полная стоимость услуг за воду
//2
	double gas_cnt; // полное количество накрученных кубов(на счётчике)
	double gas_cost; // стоимость одного куба
	double  gas_subscription_fee; //абонплата
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
const char* FILE_NAME_WAS_REMOVED = "uslugi_danniye_wasremoved"; // временный файл, если существует, показывает, что главный файл(FILE_NAME) был удалён
																 // в процессе переименования, и возможно, требуется его восстановление из копии TEMP_FILE_NAME
void CreateEmptyFile(const char* aFileName)
{
	ofstream oFile(aFileName);
	if (!oFile.good())
	{
		cout<<"создание пустого временного файла: ошибка при открытии файла "<<aFileName<<" на запись\n";
		throw 10 ;
	} 
}

bool IsFileExist(const char* aFileName)
{
	bool bFileExist=true;
	ifstream iFile(aFileName);
	if (!iFile.good())
	{
		if (errno != 2) //любая ошибка кроме FileNotFoundError
		{
			cout<<"проверка на существование: ошибка открытия файла "<<aFileName<<"на чтение";
			throw 10; // 10 -случайное число, пока не имеет никакого смысла 
		}
		// если ошибка FileNotFoundError, то есть если файл данных не существует
		bFileExist=false;
	} 
	return bFileExist;
}

void RestoreDataFilesAfterCrash()
{
	bool bMainFileExist;
	bool bTempMainFileExist;
	bool bTempWasRemovedFileExist;
	
	//проверяем на существование главного файла данных
	bMainFileExist = IsFileExist(FILE_NAME);
	//проверяем на существование копии(временного) главного файла данных
	bTempMainFileExist = IsFileExist(TEMP_FILE_NAME);
	//проверяем на существование копии(временного) главного файла данных
	bTempWasRemovedFileExist = IsFileExist(FILE_NAME_WAS_REMOVED);

	//если главный файл не существует, а копия существует и существует файл FILE_NAME_WAS_REMOVED, значит был такой краш, когда главный файл удалили, а переименовать
	//временный не успели. Следовательно, переименовываем копию в главный
	if (!bMainFileExist && bTempMainFileExist && bTempWasRemovedFileExist)
	{
		const char* old_name = TEMP_FILE_NAME; // старое название файла
		const char* new_name = FILE_NAME; // новое название файла

		if (std::rename(old_name, new_name) != 0) {
			cout<<"ошибка переименования файла "<<old_name<<" errno="<<errno<<"\n";
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
	{//дополнительные фигурные скобки значимы, служат для того чтоб iFile удалился по выходу из области видимости
     //и отпустил открытый файл, в который другой функцией мы потом пишем
		ifstream iFile(FILE_NAME);
     	if (!iFile.good())
		{
			if (errno != 2) //любая ошибка кроме FileNotFoundError
			{
				cout<<"ошибка открытия файла "<<FILE_NAME<<"на чтение";
				throw 10; // 10 -случайное число, пока не имеет никакого смысла 
			}
		    // если ошибка FileNotFoundError
			cout<<"файл "<<FILE_NAME<<" не найден\n";		
			cout<<"создать новый файл(введи 1) или выдать ошибку и будете искать старый(введи любое другое число)?\n";
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
		if (createNewFile == 0) // если не нужно создавать новый файл
		{
			iFile>>mRec_count;
			int i;
			Record curRec;
			// очищаем mRec
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
	//безопасное сохранение: чтоб исключить полную потерю данных при записи в файл(при внутренних сбоях, например, выключили свет)
	//записываем их рядом во временный, а потом переименовываем его в целевой
	{//эти скобки значимы, файл должен быть закрыт по выходу
		ofstream oFile(TEMP_FILE_NAME);
		if (!oFile.good())
		{
			cout<<"ошибка при открытии файла "<<TEMP_FILE_NAME<<" на запись\n";
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

	const char* old_name = TEMP_FILE_NAME; // старое название файла
    const char* new_name = FILE_NAME; // новое название файла
	if (IsFileExist(new_name))
	{
		CreateEmptyFile(FILE_NAME_WAS_REMOVED);
		std::remove(new_name); //rename не позволяет переименовать в существующий файл,
	                       //поэтому его надо предварительно удалить. И это - слабое место. Недочёты и косяки разработчиков
	                       //стандартных функций. Программа может прерваться в этом месте(выключили свет), после удаления файла данных,
	                       //но до переименования временного в целевой. Можно, конечно, понадеяться на высшие силы, что вероятность
	                       //этого на практике маленькая, но по хорошему этот случай надо обработать тоже. Поэтому в начале запуска программы в фунции восстановления после збоя
	                       //мы должны проверить только один случай: если целевой(главный) файл не существует, а временный файл существует,
	                       //и FILE_NAME_WAS_REMOVED существует тоже, то нужно переименовать этот временный в целевой
	}
	if (std::rename(old_name, new_name) != 0) {
		cout<<"ошибка переименования файла "<<old_name<<" errno="<<errno<<"\n";
		throw 10 ;
    }
	std::remove(FILE_NAME_WAS_REMOVED);
	return 0;
}

int RecordsFile::InputNewRecord()
{
		Record recTmp;
		MyConsole con;	

		cout<<"введи месяц и год записи(латинскими буквами): ";
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
		 
		cout<<"1. Вода\n";
		cout<<"На счётчике: ";
		//cin>>recTmp.water_cnt;
		con.ReadDouble(recTmp.water_cnt);
        cout<<"Цена за куб(если 0, значение из предыдущего месяца): ";		  
		con.ReadDouble(temp);
		//cin>>temp;
		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].water_cost;
		recTmp.water_cost = temp;
		cout<<"Абонплата(если 0, значение из предыдущего месяца): ";
		//cin>>temp;
		con.ReadDouble(temp);
		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].water_subscription_fee;
		recTmp.water_subscription_fee = temp;
	
        cout<<"2. Газ\n";
		cout<<"На счётчике: ";
		//cin>>recTmp.gas_cnt;
		con.ReadDouble(recTmp.gas_cnt);
        cout<<"Цена за куб(если 0, значение из предыдущего месяца): ";		  
		//cin>>temp;
		con.ReadDouble(temp);
		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].gas_cost;
		recTmp.gas_cost = temp;

		cout<<"Абонплата(распределение)(если 0, значение из предыдущего месяца): ";
		//cin>>temp;
		con.ReadDouble(temp);
		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].gas_subscription_fee;
		recTmp.gas_subscription_fee = temp;
		
		cout<<"3. Электричество\n";
		cout<<"На счётчике(ночь): ";
		//cin>>recTmp.electricityNight_cnt;
		con.ReadDouble(recTmp.electricityNight_cnt);
        cout<<"Цена за киловат(ночь)(если 0, значение из предыдущего месяца): ";		  
		//cin>>temp;
		con.ReadDouble(temp);

		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].electricityNight_cost;
		recTmp.electricityNight_cost = temp;
	
		cout<<"На счётчике(день): ";
		//cin>>recTmp.electricityDay_cnt;
		con.ReadDouble(recTmp.electricityDay_cnt);
        cout<<"Цена за киловат(день)(если 0, значение из предыдущего месяца): ";		  
		//cin>>temp;
		con.ReadDouble(temp);
		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].electricityDay_cost;
		recTmp.electricityDay_cost = temp;
		
		cout<<"4. Тенет(интернет)\n";
        cout<<"Абонплата(если 0, значение из предыдущего месяца): ";		  
		//cin>>temp;
		con.ReadDouble(temp);
		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].tenet_full_cost;
		recTmp.tenet_full_cost = temp;

		cout<<"5. Киевстар\n";
        cout<<"Абонплата(если 0, значение из предыдущего месяца): ";		  
		//cin>>temp;
		con.ReadDouble(temp);

		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].kievstar_full_cost;
		recTmp.kievstar_full_cost = temp;
	
		cout<<"6. Мусор\n";
        cout<<"Абонплата(если 0, значение из предыдущего месяца): ";		  
		// cin>>temp;
		con.ReadDouble(temp);
		if ((temp == 0) && (mRec_count>0)) temp = mRec[mRec_count-1].garbage_full_cost;
		recTmp.garbage_full_cost = temp;
		
		mRec_count++;
		mRec.push_back(recTmp);
		  
		WriteToFile();	
		return 0;
}

//numberRep - номер отчёта отсчитывая с конца, и с нуля
int RecordsFile::PrintReport(int numberRepFromEnd)
{
	if (mRec_count<=0)
	{
		cout<<"список записей пуст, пока нечего выводить";
		return 1;
	}
	int numberRep = mRec_count-1-numberRepFromEnd;
	if ((numberRep<0) || (numberRep>=mRec_count))
	{
		cout<<"номер отчёта вне диапазона допустимых номеров";
		return 1;
	}

	bool bIsFirst; // это самая первая запись или нет
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

	cout<<"Отчёт за "<<cur.monthAndYear<<"\n";
	if (bIsFirst) cout<<"это самый первый отчёт, поэтому выводятся только данные, без рассчётов\n";

	cout<<"1. Вода\n";
	cout<<"   На счётчике: "<<cur.water_cnt<<"\n";
    cout<<"   Цена за куб: "<<cur.water_cost<<"\n";
	cout<<"   Абонплата: "<<cur.water_subscription_fee<<"\n";
	if (!bIsFirst)
	{
		water_total = (cur.water_cnt - prev.water_cnt)*cur.water_cost+cur.water_subscription_fee;
		cout<<"   Итого: "<<water_total<<"\n";
	}

	cout<<"2. Газ\n";
	cout<<"   На счётчике: "<<cur.gas_cnt<<"\n";
    cout<<"   Цена за куб: "<<cur.gas_cost<<"\n";
	cout<<"   Абонплата: "<<cur.gas_subscription_fee<<"\n";
	if (!bIsFirst)
	{
		gas_total = (cur.gas_cnt - prev.gas_cnt)*cur.gas_cost+cur.gas_subscription_fee;
		cout<<"   Итого: "<<gas_total<<"\n";
	}

	cout<<"3. Электричество\n";
	cout<<"   На счётчике(ночь): "<<cur.electricityNight_cnt<<"\n";
	cout<<"   Цена за киловат(ночь): "<<cur.electricityNight_cost<<"\n";		  
	cout<<"   На счётчике(день): "<<cur.electricityDay_cnt<<"\n";
	cout<<"   Цена за киловат(день): "<<cur.electricityDay_cost<<"\n";	
	if (!bIsFirst)
	{
		electricity_total = (cur.electricityNight_cnt - prev.electricityNight_cnt)*cur.electricityNight_cost
		+ (cur.electricityDay_cnt - prev.electricityDay_cnt)*cur.electricityDay_cost;
		cout<<"   Итого: "<<electricity_total<<"\n";
	}

	cout<<"4. Тенет(интернет)\n";
	cout<<"   Абонплата: "<<cur.tenet_full_cost<<"\n";
	if (!bIsFirst)
	{
		cout<<"   Итого: "<<cur.tenet_full_cost<<"\n";
	}

	cout<<"5. Киевстар\n";
    cout<<"   Абонплата: "<<cur.kievstar_full_cost<<"\n";		  
    if (!bIsFirst)
	{
	cout<<"   Итого: "<<cur.kievstar_full_cost<<"\n";
	}

    cout<<"6. Мусор\n";
	cout<<"   Абонплата: "<<cur.garbage_full_cost<<"\n";
	if (!bIsFirst)
	{
		cout<<"   Итого: "<<cur.garbage_full_cost<<"\n";
	}

	if (!bIsFirst)
	{
		cout<<"ИТОГО ЗА ВСЁ: "<<water_total+gas_total+electricity_total+cur.tenet_full_cost+cur.kievstar_full_cost+cur.garbage_full_cost<<"\n";
	}
}

int RecordsFile::DeleteLastRecord()
{
	int ok;
	cout<<"точно хотите удалить последнюю запись?(0-нет, 1-да): ";
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


	//варианты
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
    fontInfo.dwFontSize.X = 12;  // Увеличиваем ширину шрифта в два раза
    fontInfo.dwFontSize.Y = 18;  // Увеличиваем высоту шрифта в два раза

    SetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);*/

	/*HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Получить консоль

CONSOLE_FONT_INFOEX fontInfo;
GetCurrentConsoleFontEx(hConsole, TRUE, &fontInfo); // Получить текущий шрифт

// Поменять какие-то параметры
//wcsncpy(L"Arial Cyr", fontInfo.FaceName, LF_FACESIZE);  // Имя
fontInfo.dwFontSize.X = 10; // Размер (в логических единицах)

SetCurrentConsoleFontEx(hConsole, TRUE, &fontInfo); // Установить новый

*/
}

void Run()
{
	cout<<"Программа расчёта услуг\n";
    //дробные числа можно вводить и через точку, и через запятую, без разницы
	
	// загрузить данные из файла данных
	RecordsFile r;
	r.LoadFromFile();

	int choise;
	do
	{
		cout<<"\n0-выход, 1-ввести данные за новый месяц, 2-вывести последний отчёт\n"
		<< "3-удалить последнюю запись, 4-вывести последние N отчётов\n";

	   cin>>choise;
	   if (choise == 1)
       {
		   r.InputNewRecord();
       }
	   else if (choise ==2) 
	   {
	        r.PrintReport(0);//пока последний   
	   }
	   else if (choise ==3) 
	   {
	        r.DeleteLastRecord();	   
	   }
	   else if (choise ==4) 
	   {
		   cout<<"введи N: ";
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
	SystemInitialisation(); //локаль
	RestoreDataFilesAfterCrash();//при каждом запуске программы работающей с файлами по хорошему нужно вызывать подобную фунцию
	
	try
	{
		Run();
	}
	catch (...)
	{
		int exit;
		cout<<"\nвозникла ошибка в программе, введите любое число для выхода";
		cin>>exit;
	}

	return 0;
}

