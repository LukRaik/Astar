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


using namespace std;

class Field {
public:
	Field(int x, int y, int fieldCost);
	Field(int x, int y, int fieldCost, Field* parent);
	void CountHeuristic() {
		double value;
		value = this->fieldCost + sqrt(pow((this->x - xLen - 1), 2) + pow((this->y - yLen - 1), 2));
		this->heuristicValue = value;
	};
	double heuristicValue;
	int x;
	int y;
	int fieldCost;
	Field* parent;
};

//To jest konstruktor !!! :D
Field::Field(int x, int y, int fieldCost) {
	this->x = x;
	this->y = y;
	this->fieldCost = fieldCost;
	this->parent = nullptr;
};
//TO JEST DRUGI KONSTRUKTOR :)
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

Field* FindField(int x, int y, vector<Field*> list) {
	for (Field* field : list) {
		if (field->x == x && field->y == y) {
			return field;
		}
	}
	return nullptr;
};

//Pobiera komórkê w stosunku do aktualnego po³o¿enia
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
	if (map[x][y] == 5 || map[x][y] == 2) return nullptr;
	if (FindField(x, y, added) != nullptr) return nullptr;

	return new Field(x, y, cur->fieldCost + 1, cur);
};

void RefreshMap(vector<vector<int>>* map) {
	for (int i = 0; i < map->size(); i++) {
		for (int j = 0; j < (*map)[i].size(); j++) {
			if((*map)[i][j]==7)(*map)[i][j] = 0;
		}
	}
}

//Rysuje mapê
void DrawMap(vector<vector<int>> map) {
	system("cls");
	HANDLE hOut;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int i = map.size() - 1; i >= 0; i--) {
		for (auto b : map[i]) {
			switch (b) {
			case 9:
				SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
				cout << " " << "R";
				break;
			case 7:
				SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
				cout << " " << "M";
				break;
			case 5:
				SetConsoleTextAttribute(hOut, FOREGROUND_RED);
				cout << " " << "X";
				break;
			case 0:
				cout << " " << " ";
				break;
			}
		}
		cout << "\n";
		SetConsoleTextAttribute(hOut, FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN);
	}
};


void TryAddCell(Field* cur, Direction dir, vector<vector<int>> map, vector<Field*> closedList, vector<Field*>* openList) {
	auto field = GetCell(cur, dir, map, closedList);
	if (field != nullptr) {
		auto oldField = FindField(field->x, field->y, *openList);
		if (oldField != nullptr) {
			if (oldField->heuristicValue > field->heuristicValue) {
				oldField->heuristicValue = field->heuristicValue;
				oldField->parent = field->parent;
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

	plik.close();

	map[0][0] = 7;

	while (closedList[closedList.size()-1]->x != 19 || closedList[closedList.size() - 1]->y != 19) {

		//gora dol lewo prawo
		int index = closedList.size() - 1;

		TryAddCell(closedList[index], Top, map, closedList, &openList);
		TryAddCell(closedList[index], Down, map, closedList, &openList);
		TryAddCell(closedList[index], Left, map, closedList, &openList);
		TryAddCell(closedList[index], Right, map, closedList, &openList);

		int elementIndex = 0;
		Field* lowest = nullptr;
		for (int i = 0; i < openList.size(); i++) {
			if (openList[i] == nullptr) continue;
			if (lowest == nullptr) {
				lowest = openList[i];
				elementIndex = i;
			}
			else if (lowest->heuristicValue > openList[i]->heuristicValue) {
				lowest = openList[i];
				elementIndex = i;
			}
		}
		openList.erase(openList.begin() + elementIndex);
		closedList.push_back(lowest);
		map[lowest->x][lowest->y] = 7;
		DrawMap(map);
		Sleep(150);
	}

	Field* field = closedList[closedList.size() - 1];

	//RefreshMap(&map);

	while(field!=nullptr){
		map[field->x][field->y] = 9;
		field = field->parent;
		DrawMap(map);
		Sleep(150);
	}

	DrawMap(map);
	system("pause");




}
