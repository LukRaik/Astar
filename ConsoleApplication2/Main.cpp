#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <Windows.h>
#include <cstdlib>

#define ECV 0;
const int yLen = 20;
const int xLen = 20;
const int ANIMSPEED = 35;


using namespace std;

//Metryka euklidesa
double CountEuclides(int x, int y) {
	return sqrt(pow((x - xLen - 1), 2) + pow((y - yLen - 1), 2));
}

class Field {
public:
	Field(int x, int y, int fieldCost);
	Field(int x, int y, int fieldCost, Field* parent);
	void CountHeuristic() {
		this->compareFunctionValue = this->fieldCost + CountEuclides(this->x, this->y);
	};
	double compareFunctionValue;
	int x;
	int y;
	int fieldCost;
	Field* parent;
};

Field::Field(int x, int y, int fieldCost) {
	this->x = x;
	this->y = y;
	this->fieldCost = fieldCost;
	this->parent = nullptr;
};

Field::Field(int x, int y, int fieldCost, Field* parent) {
	this->x = x;
	this->y = y;
	this->fieldCost = fieldCost;
	this->parent = parent;
	CountHeuristic();
};


//Okreœla kierunek
enum Direction {
	Left,
	Right,
	Top,
	Down
};

//Szukaj w vectorze pola o odpowiednich kordynatach
Field* FindField(int x, int y, vector<Field*> list) {
	for (Field* field : list) {
		if (field->x == x && field->y == y) {
			return field;
		}
	}
	return nullptr;
};

//Pobiera komórkê w stosunku do aktualnego po³o¿enia zwracaj¹c uwagê na kolizjê, i ju¿ istniej¹ce obiekty
Field* GetCell(Field* cur, Direction dir, vector<vector<int>> map, vector<Field*> added) {
	int x = cur->x;
	int y = cur->y;
	switch (dir) {
	case Left:
		x--;
		break;
	case Right:
		x++;
		break;
	case Top:
		y++;
		break;
	case Down:
		y--;
		break;
	}
	if (!(x >= 0 && x < xLen)) return nullptr;
	if (!(y >= 0 && y < xLen)) return nullptr;
	if (map[x][y] == 5) return nullptr;
	if (FindField(x, y, added) != nullptr) return nullptr;

	return new Field(x, y, cur->fieldCost + 1, cur);
};

//Rysuje mapê
void DrawMap(vector<vector<int>> map) {
	system("cls");
	HANDLE hOut;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int i = map.size() - 1; i >= 0; i--) {
		for (auto b : map[i]) {
			switch (b) {
			case 9: //Pole trasy
				SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
				cout << " " << "R";
				break;
			case 7: //Pole sprawdzane
				SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
				cout << " " << "M";
				break;
			case 5: //Œciana
				SetConsoleTextAttribute(hOut, FOREGROUND_RED);
				cout << " " << "X";
				break;
			case 0: //Nic
				cout << " " << " ";
				break;
			}
		}
		cout << "\n";
		SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
	}
};

//Spróbuj dodaæ komorkê do listy otwartej
void TryAddCell(Field* cur, Direction dir, vector<vector<int>> map, vector<Field*> closedList, vector<Field*>* openList) {
	auto field = GetCell(cur, dir, map, closedList);//Spróbuj pobraæ komórkê
	if (field != nullptr) {
		auto oldField = FindField(field->x, field->y, *openList); //ZnajdŸ takie same pole
		if (oldField != nullptr) { //Jeœli istnieje
			if (oldField->compareFunctionValue > field->compareFunctionValue) {//Gdy wartoœæ funkcji jest mniejsza
				oldField->compareFunctionValue = field->compareFunctionValue; //Zmieñ wartoœæ funkcji dla starego obiektu
				oldField->parent = field->parent;	// I rodzica
			}
		}
		else {
			openList->push_back(field);
		}
	}
}


int main() {
	string nazwa = "grid.txt";

	vector<vector<int>> map;
	vector<Field*> openList;
	vector<Field*> closedList;

	cout << "A* Lukasz Zimnoch" << '\n';
	cout << "Podaj sciezke do pliku:";

	cin >> nazwa;

	closedList.push_back(new Field(0, 0, 0));
	ifstream plik(nazwa.c_str());

	for (unsigned int i = 1; i < yLen + 1; i++)
	{
		vector<int> tempVector;
		for (unsigned int j = 1; j < xLen + 1; j++)
		{
			int value;
			plik >> value;
			tempVector.push_back(value);
		}
		map.insert(map.begin(), tempVector);
	}

	map[0][0] = 7;

	while (closedList[closedList.size() - 1]->x != xLen-1 || closedList[closedList.size() - 1]->y != yLen-1) {

		//gora dol lewo prawo
		int index = closedList.size() - 1; //Index ostatnio dodanego pola

		if (openList.size() == 0&&closedList.size()>1) break;

		TryAddCell(closedList[index], Top, map, closedList, &openList);
		TryAddCell(closedList[index], Down, map, closedList, &openList);
		TryAddCell(closedList[index], Left, map, closedList, &openList);
		TryAddCell(closedList[index], Right, map, closedList, &openList);

		//Szukaj pola z najmniejsza wartoœcia funkcji
		int elementIndex = 0;
		Field* lowest = nullptr;
		for (int i = 0; i < openList.size(); i++) {
			if (openList[i] == nullptr) continue;
			if (lowest == nullptr) {
				lowest = openList[i];
				elementIndex = i;
			}
			else if (lowest->compareFunctionValue > openList[i]->compareFunctionValue) {
				lowest = openList[i];
				elementIndex = i;
			}
		}
		openList.erase(openList.begin() + elementIndex);//Usuñ z otwartej listy ten element
		closedList.push_back(lowest); // Dodaj do zamkniêtej
		map[lowest->x][lowest->y] = 7; //Narysuj na mapie
		DrawMap(map);
		Sleep(ANIMSPEED);
	}

	Field* field = closedList[closedList.size() - 1]; // Osatnie pole

	if (field->x != 19 || field->y != 19) {
		cout << "Szukane sciezki nieudane\n";
	}
	else {
		while (field != nullptr) {
			map[field->x][field->y] = 9;
			field = field->parent;
			DrawMap(map);
			Sleep(ANIMSPEED * 2);
		}

		DrawMap(map);
	}
	//RefreshMap(&map);

	
	plik.close();
	system("pause");




}