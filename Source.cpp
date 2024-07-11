#include<iostream>
#include<vector>
#include<iomanip>
#include<string>
#include <windows.h>
#include<conio.h>
#include<fstream>
#include<cctype>
//#include<mmsystem.h>
using namespace std;
#define int long long 
#define loop(x, n) for(int i = x; i < n; i++)

const string PLAYER_ICON = "🧙";
const string ENEMY_ICON = "👻";
const string PLANT_ICON = "🌲";
const string TREE_ICON = "🌵";
const string HEART_ICON = "🤍";
const string WALL_ICON = "🧱";


struct Position {
	int X = 0, Y = 0;
};

void Color(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

int height, width;
class Character {

public:
	//static Player* hero;
	//static int w, h;
	string name;
	int health = 101, kills = 0, XP = 0;
	int heart = 20, color = 15;
	static int enemies;
	bool alive = true;
	string icon = ENEMY_ICON;
	bool enemy = false;
	Position pos;
	static Position playerPos;
	static vector<vector<Character*> > characters;
	virtual void Attack() {}

	//Character(int w, int h) { this->w = w; this->h = h; }
	Character() {}
	Character(string n) { name = n; }
	//virtual void Walk(char dir) {}
	virtual void Move(int x, int y) {
		bool move = false;
		if (x < width && x >= 0 && y < height && y >= 0) {
			if (characters[y][x] == NULL) {
				characters[playerPos.Y][playerPos.X]->health--;
				move = true;
			}
			else if (characters[y][x]->icon == HEART_ICON) {
				move = true;
				characters[playerPos.Y][playerPos.X]->health += heart + 5;
			}
			else if (characters[y][x]->name == "barrier") {
				if (_getch() == ' ') {
					if (characters[y][x]->icon == TREE_ICON) characters[y][x]->icon = PLANT_ICON;
					else {
						move = true;
						characters[playerPos.Y][playerPos.X]->health -= 2;
					}
				}
			}
			else if (characters[y][x]->enemy) {
				move = true;
				characters[playerPos.Y][playerPos.X]->kills++;
				characters[playerPos.Y][playerPos.X]->health -= 5;
				enemies--;
			}


			if (move) {
				Beep(1100, 100);
				characters[y][x] = characters[playerPos.Y][playerPos.X];
				characters[playerPos.Y][playerPos.X] = NULL;
				playerPos.Y = y; playerPos.X = x;
			}

		}
	}

	virtual void printStats() {/* characters[playerPos.Y][playerPos.X]->printStats(); */ }

};
class Player : public Character {

	int hearts = 5;
public:
	//static Player *hero;
	//int gold = 0; 
	//Player() { pos.X = 0; pos.Y = 0; }
	Player(string n) :Character(n) { playerPos.X = 0; playerPos.Y = 0; icon = PLAYER_ICON; }

	bool Walk(char dir) {
		switch (dir) {
		case 'w': case'H': Move(playerPos.X, playerPos.Y - 1); return true;
		case 'a': case'K': Move(playerPos.X - 1, playerPos.Y); return true;
		case 'd': case'M': Move(playerPos.X + 1, playerPos.Y); return true;
		case 's': case'P': Move(playerPos.X, playerPos.Y + 1); return true;
		case 'x': return false;
			//default: return true;
		}

		if (characters[playerPos.Y][playerPos.X]->health <= 1) {
			characters[playerPos.Y][playerPos.X]->alive = false;
			return false;
		}

		return true;
	}

	void printStats() {
		//cout << (char)3 << "HEALTH" << (char)3 << ": " << health << endl;
		hearts = health / heart + 1;
		if (hearts > 5) hearts = 5;
		Color(4);
		for (int i = 0; i < hearts; i++)
			cout << (char)3 << ' ';
		Color(15);
		//cout << "\n" << health << ' ' << hearts;
		cout << "\nKILLS: " << kills << endl;
		//cout << "Experiance: " << XP << endl;
		if (health < 25)
			cout << "Get ready to say your goodbyes. Death is near!!\n";
		if (health < 50)
			cout << "Be carefull not to die idiot!\n";
		if (kills == 1)
			cout << "Yay! You've killed someone. You're not useless afterall!! :D\n";
		if (kills > 7 && kills < 15)
			cout << "Okay you've officially become a murderer. Good for you! ;)\n";

	}

	void Restart() {
		health = 100;
		kills = 0;
		playerPos.X = playerPos.Y = 0;
		icon = PLAYER_ICON;
		hearts = 5;
	}

};

class Enemy : public Character {

public:
	bool moved;
	Enemy() {}

	void MoveE(int x, int y) {
		//moved = false;
		if (x < width && x >= 0 && y < height && y >= 0) {
			if (characters[y][x] != NULL && characters[y][x]->icon == PLAYER_ICON) {
				if (characters[y][x]->health <= 55) {
					characters[y][x]->alive = false;
					characters[y][x]->health = 0;
					characters[y][x]->icon = ENEMY_ICON;
					characters[pos.Y][pos.X] = NULL;
					pos.X = x; pos.Y = y;
					moved = true;

				}
				else characters[y][x]->health -= 10;
			}
			else if (characters[y][x] == NULL) {
				characters[y][x] = characters[pos.Y][pos.X];
				characters[pos.Y][pos.X] = NULL; pos.X = x; pos.Y = y;
				moved = true;
			}


		}

	}
	void Attack() {
		//int x = pos.X; int y = pos.Y;
		moved = false;
		if (playerPos.X > pos.X) MoveE(pos.X + 1, pos.Y);
		else if (playerPos.X < pos.X) MoveE(pos.X - 1, pos.Y);

		if (playerPos.Y > pos.Y && !moved) MoveE(pos.X, pos.Y + 1);
		else if (playerPos.Y < pos.Y) MoveE(pos.X, pos.Y - 1);
		///PROBLEMOOOOOO HEREEEE
		/*if (x != pos.X && y != pos.Y)
			return true;
		else return false;*/
	}


};

class gameBoard {

	int w, h;
	int Ebarriers = 0, Hbarriers = 0;
	//int hearts = 0;

public:

	static Player* hero;
	static int level;
	void setBoard(string n = " ", int w = 70, int h = 70) {
		width = this->w = w; height = this->h = h;
		vector<vector<Character*> > ch(height, vector<Character*>(width));
		Character::characters = ch;
		hero = new Player(n);
		hero->health = w * 20;
		hero->heart = (w * 20) / 5;
		Character::characters[0][0] = hero;
		if (n != " ")
			cout << "Welcome " << n << " to this stupid game! Yay! Have fun or whatever...\n";
		spawnEnemy();
	}

	void draw() {

		if (!hero->alive) {
			system("cls");
			for (int i = 0; i < 8; i++)
				cout << "GAME OVER! GAME OVER! GAME OVER! GAME OVER!\n";
			return;
		}
		if (Character::enemies == 0) {
			level++;
			for (int i = 0; i < level /*+ log(level * 3)*/; i++)
				spawnEnemy();
			if (level > 2 && level % 2)
				spawnHearts();
			for (int i = Ebarriers - 1; i < (level - 2) * 2; i++)
				spawnBarriers(PLANT_ICON, 2);
			if (level > 3) {
				for (int i = Hbarriers - 1; i < level - 4; i++)
					spawnBarriers(TREE_ICON, 10);
			}
		}

		if (level > 1) {
			enemyAttack();
		}

		system("cls");
		Color(11);
		cout << "LEVEL " << level << ":\n";
		Color(3);
		for (int i = -1; i < height + 1; i++) {
			cout << WALL_ICON << " ";
			for (int j = 0; j < width; j++) {
				if (i == height || i == -1)
					cout << WALL_ICON;
				else if (Character::characters[i][j] == NULL)
					cout << "  ";
				else {
					Color(Character::characters[i][j]->color);
					cout << Character::characters[i][j]->icon;
					Color(3);
				}
			}
			cout << " " << WALL_ICON << " \n";
		}
		cout << endl;
		Color(15);
		hero->printStats();

	}

	void enemyAttack() {
		bool stop;
		for (int i = 0; i < h; i++)
			for (int j = 0; j < w; j++) {
				if (Character::characters[i][j] != NULL && Character::characters[i][j]->enemy)
					Character::characters[i][j]->Attack();
			}
	}

	bool Again() {
		char c;
		int t = 1;
		while (t--) {
			cout << "Do you want to restart?(y/n) ";
			cin >> c;
			c = tolower(c);
			if (c == 'y') {
				level = 1;
				hero->alive = true;

				for (int i = 0; i < height; i++)
					for (int j = 0; j < width; j++)
						Character::characters[i][j] = NULL;

				Character::characters[0][0] = hero;
				hero->Restart();
				spawnEnemy();
				draw();
				return true;
			}
			else if (c == 'n') return false;

			else {
				t = 1;
				cout << "\aPlease enter y or n!\n";
			}
		}
	}

	void spawnEnemy() {

		bool cont = true;
		int posX, posY;
		do {
			posX = rand() % width;
			posY = rand() % height;
			if (Character::characters[posY][posX] == NULL) {
				Enemy* e = new Enemy;
				e->pos.X = posX;
				e->pos.Y = posY;
				e->enemy = true;
				Character::characters[posY][posX] = e;
				Character::enemies++;
				cont = false;
			}

		} while (cont);
	}

	void spawnHearts() {
		bool cont = true;
		int posX, posY;
		do {
			posX = rand() % width;
			posY = rand() % height;
			if (Character::characters[posY][posX] == NULL) {
				Character::characters[posY][posX] = new Character;
				Character::characters[posY][posX]->icon = HEART_ICON;
				Character::characters[posY][posX]->color = 4;
				Character::characters[posY][posX]->name = "heart";
				cont = false;
			}

		} while (cont);
	}

	void spawnBarriers(string i, int c) {
		bool cont = true;
		int posX, posY;
		do {
			posX = rand() % width;
			posY = rand() % height;
			if (Character::characters[posY][posX] == NULL) {
				Character::characters[posY][posX] = new Character;
				Character::characters[posY][posX]->icon = i;
				Character::characters[posY][posX]->color = c;
				Character::characters[posY][posX]->name = "barrier";
				if (i == PLANT_ICON) Ebarriers++;
				else Hbarriers++;
				cont = false;
			}

		} while (cont);
	}


	void SaveData() {

		Character* c = hero;
		ofstream out;
		out.open("C:\\Users\\Sarah\\Documents\\gameData.txt");
		out << level << " " << c->kills << " " << c->health << " " << c->name;
		out.close();
		cout << "Your data has been successfully saved! " << (char)1 << " \n";
	}

	void LoadData() {

		ifstream in;
		in.open("C:\\Users\\Sarah\\Documents\\gameData.txt");
		setBoard(" ", 10, 10);
		in >> level >> Character::characters[0][0]->kills >> Character::characters[0][0]->health;
		getline(in, Character::characters[0][0]->name);
		Character::enemies = 0;
		draw();
		in.close();
		cout << "Great to have you back" << Character::characters[0][0]->name << "! :)\n";

	}
};

int Character::enemies = 0;
Position Character::playerPos;
Player* gameBoard::hero = NULL;
vector<vector<Character*> > Character::characters = {};
int gameBoard::level = 1;
signed main() {

	/*for (int x = 1; x <= 20; x++)
	{
		Color(x);
		cout << x << endl;
	}*/
	//bool played = PlaySound(TEXT("C:\\Users\\Sarah\\source\\repos\\Project1\\Project1\\WIN.wav"), NULL, SND_ASYNC);
	srand((unsigned)time(0));
	string s; char input;
	gameBoard board;
	int t = 1;
	while (t--) {
		cout << "Welcome! Do you want to load a previous game?(y/n) ";
		cin >> input;
		input = tolower(input);
		if (input == 'y')
			board.LoadData();
		else if (input == 'n') {
			cout << "Hello Player! What's your name? ";
			cin.ignore();
			getline(cin, s);
			cout << "\n<><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n";
			cout << "<>                   INSTRUCTIONS:                    <>\n";
			cout << "<>   ->Use the arrow or WASD keys to move.            <>\n";
			cout << "<>   ->Kill enemy(s) to move to next level.           <>\n";
			cout << "<>   ->Press space to destroy barriers.               <>\n";
			cout << "<>   ->To exit or restart game press x.               <>\n";
			cout << "<>                                                    <>\n";
			cout << "<>                       KEY:                         <>\n";
			cout << "<>                                                    <>\n";
			cout << "<>   YOU: 🧙               ENEMY: 👻                  <>\n";
			cout << "<>   Easy Barrier: ";
			Color(2); cout << "🌲"; Color(15);
			cout << "\t   Hard Barrier: ";
			Color(10); cout << "🌵"; Color(15);
			cout << "           <>\n";
			cout << "<>                                                    <>\n";
			cout << "<><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n";
			cout << "Press ENTER to continue...\n";
			cin.get(input);
			board.setBoard(s, 15, 15);
			board.draw();
		}
		else { t = 1; cout << "\aPlease enter y or n!\n"; }
	}

	bool cont;

	do {
		do {
			input = _getch();
			cont = gameBoard::hero->Walk(input);
			board.draw();
		} while (cont);
	} while (board.Again());

	if (gameBoard::hero->alive) {
		cout << "\nDo you want to save your progress?(y/n) ";
		cin.ignore();
		cin.get(input);
		input = tolower(input);
		if (input == 'y')
			board.SaveData();
	}

	cout << "\nThanks for playing " << s << "!!Goodbye! :)\n";




	return 0;
}
