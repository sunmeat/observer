#include <iostream>
#include <windows.h>
#include <ctime>
#include <string>
#include <vector>
#include <regex> 
#include <fstream> 
#pragma comment(lib, "urlmon.lib")   
#pragma warning(disable : 4996)   
using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Model abstract
{
public:
	virtual void Logic() = 0;
};

class View abstract
{
public:
	virtual void ShowInfo() = 0;
};

class Controller abstract
{
public:
	virtual void GetInfo() = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Observer
{
public:
	virtual void Update() = 0;
};

class Observable
{
	vector<Observer*> observers;

public:
	void AddObserver(Observer* observer)
	{
		observers.push_back(observer);
	}

	void NotifyUpdate()
	{
		for (auto& observer : observers)
			observer->Update();
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WeatherForecastModel : public Model, public Observable
{
	double temperature = 0;
	string city = "";

public:
	void Logic()
	{
		GetForecast();
	}

private:
	void GetForecast()
	{
		const char* srcURL = "https://www.gismeteo.ua/weather-odessa-4982/";
		const char* destFile = "weather.txt"; // the destination file   
		URLDownloadToFileA(NULL, srcURL, destFile, 0, NULL);

		ifstream file(destFile);
		string text;
		string temp;
		while (!file.eof())
		{
			getline(file, temp);
			text += temp;
		}

		// показ температуры по ощущению
		string search = "<span class=\"measure\"><span class=\"unit unit_temperature_c\">";
	
		size_t pos = text.find(search);
		int index = pos + search.length();
		string result = text.substr(index, 3);
		SetTemperature(stoi(result));
	}

private:
	void SetTemperature(double temp)
	{
		if (temp < -91.2 || temp > 58.2) return;
		temperature = temp;
		NotifyUpdate();
	}

public:
	double GetTemperature()
	{
		return temperature;
	}

	void SetCity(string city)
	{
		this->city = city;
		GetForecast();
		NotifyUpdate();
	}

	string GetCity()
	{
		return city;
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ConsoleView : public View, public Observer
{
	WeatherForecastModel* model;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
public:
	ConsoleView(WeatherForecastModel* model)
	{
		Options();
		this->model = model;
		this->model->AddObserver(this);
	}

	virtual void Update() override
	{
		system("cls");

		cout << "температура воздуха в городе ";
		setlocale(0, "C");
		printf("%s: %d", model->GetCity().c_str(), (int)model->GetTemperature());
		cout << "C.\n\n";
		setlocale(0, "");
	}

	virtual void ShowInfo() override
	{
		Update();
	}

private:
	void Options()
	{
		SetTitle("MVC architectural pattern for C++ console application");
		SetWindowSize();
		SetBackgroundColor();
		StartRandomize();
		ShowCursor(true);
		system("chcp 1251>0");
	}

	void SetTitle(const char* title)
	{
		char name[200];
		sprintf_s(name, 200, "title %s", title);
		system(name);
	}

	void SetWindowSize(int width = 80, int height = 20)
	{
		//char mode[200];
		//sprintf_s(mode, 200, "mode con cols=%d lines=%d", width, height);
		//system(mode);
	}

	void SetBackgroundColor()
	{
		SetConsoleTextAttribute(h, 16 * 1 + 15);
	}

	void StartRandomize()
	{
		srand(unsigned(time(0)));
		rand();
	}

	void ShowCursor(bool show, int size = 100)
	{
		CONSOLE_CURSOR_INFO info;
		info.bVisible = show;
		info.dwSize = size;
		SetConsoleCursorInfo(h, &info);
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ConsoleController : public Controller
{
	WeatherForecastModel* model;

public:
	ConsoleController(WeatherForecastModel* model)
	{
		this->model = model;
	}

	void GetInfo()
	{
		model->SetCity("odessa");
		string city;
		do
		{
			cin >> city;
			model->SetCity(city);
		} while (city == "odessa" || city == "kiev" || city == "lvov");
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	WeatherForecastModel model;
	ConsoleView view(&model);
	ConsoleController controller(&model);
	controller.GetInfo();
}