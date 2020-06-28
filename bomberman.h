#include "raylib.h"
#include <bits/stdc++.h>

using namespace std;

class Game
{
public:
    bool Start();
    void Pause();
};

class Board;
class Bomb;
class Player;
class Boost;
class Obstacle;



class Board
{
private:
    const int board_width = 1200;
    const int board_height = 800;
    const int square_size = 65;
    const int height_marigin = (board_height-height*square_size) / 2;
    const int width_marigin = (board_width-width*square_size) / 2;
    static const int width = 11;
    static const int height = 11;
    Obstacle * obstacle;
    vector<Boost> boost;
    vector<Bomb> bombs;
public:
    Board();
    void Initialize();
    void Draw_Game_State();
    bool Is_Ok(int h,int w);
    bool Is_Free(int h,int w);
    bool Is_Boost(int h,int w);
    int Get_Board_Width();
    int Get_Board_Height();
    int Get_Square_Size();
    int Get_Width();
    int Get_Height();
    int Get_Height_Marigin();
    int Get_Width_Marigin();
    void Erase_Obstacle(int,int);
    void Add_Bomb(int,int,double,int);
    void Draw_Bombs();
    int Detonate_Bombs(Player&,Player&);
    bool Is_Bomb_Planted(int i,int j);
    int Get_Bombs_Size();
    void Draw_Boost();
    void Add_Boost(int id,int y,int x);
    void Check_Boost(Player*);
    ~Board();
};

class Result
{
public:
    void Draw_Winner(int index);
    bool Ask_If_Again();
};

class Player
{
private:
    int speed;
    int x,y;
    Color col;
    int left,right,up,down;
    int bombs_cnt;
    int bomb_range;
    int bomb_key;
    int id;
    double last_plant_time;
public:
    static const int sz = 35;
    Player(int,int x,int y,Color col,int,int,int,int,int);
    int Get_X();
    int Get_Y();
    void Draw(Board * board);
    void Move(Board * board);
    void Plant_Bomb(Board * board);
    void Add_Bomb();
    void Increase_Range();
    void Increase_Speed();
    double Get_Last_Plant_Time();
    int Get_Bomb_Range();
    bool Is_Dead(set<pair<int,int> >,Board * board);
};

class Obstacle
{
private:
    set<pair<int,int> > obs_list;
public:
    Obstacle();
    void Initialize(int h,int w);
    bool Is_Obstacle(int i,int j);
    void Erase(int i,int j);
};

class Bomb
{
private:
    int x,y;
    double plant_time;
    int owner;
public:
    Bomb(int,int,double,int);
    void Draw_Bomb(Bomb bomb,Board * board);
    int Get_X();
    int Get_Y();
    double Get_Plant_Time();
    void Set_Plant_Time(double);
    int Get_Owner();
    void Explosion(Board * board,set<pair<int,int> >);
    set<pair<int,int> > Cells_In_Range(Player player,Board * board);
};

class Boost
{
private:
    int type;
    int x,y;
public:
    Boost(int,int,int);
    void Draw(Board * board);
    bool Check_Taken(Player*,Board*);
    int Get_X();
    int Get_Y();
    int Get_Type();
};


