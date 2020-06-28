#include <bits/stdc++.h>
#include "bomberman.h"
#include "raylib.h"


using namespace std;

bool Game::Start()
{
    Board * board = new Board();
    board->Initialize();
    Player player1(1,1,1,GREEN,KEY_A ,KEY_D, KEY_W ,KEY_S,KEY_SPACE);
    Player player2(2,board->Get_Width() * board->Get_Square_Size() - Player::sz , board->Get_Height() * board->Get_Square_Size() - Player::sz , BLUE, KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN,KEY_ENTER);

    InitWindow(board->Get_Board_Width(),board->Get_Board_Height(),"Bomberman");

    SetTargetFPS(60);
    int k = 0;
    while (!WindowShouldClose())
    {
        BeginDrawing();
        board->Draw_Game_State();
        player1.Draw(board);
        player2.Draw(board);
        player1.Move(board);
        player2.Move(board);
        player1.Plant_Bomb(board);
        player2.Plant_Bomb(board);
        board->Check_Boost(&player1);
        board->Check_Boost(&player2);
        int ans = board->Detonate_Bombs(player1,player2);
        EndDrawing();
        if(IsKeyPressed(KEY_P) && board->Get_Bombs_Size() == 0) Pause();
        if(ans)
        {
            Result result;
            result.Draw_Winner(ans);
            delete board;
            return result.Ask_If_Again();
        }
    }
    delete board;
    return false;
}

void Game::Pause()
{
    int cnt = 0;
    while (!WindowShouldClose())
    {
        cnt++;
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("GAME PAUSED", 300, 220, 80, BLACK);
        DrawText("PRESS P TO RESUME", 160, 320, 80, BLACK);

        EndDrawing();

        if(IsKeyPressed(KEY_P) && cnt>=5)
        {
            double time_elapsed = GetTime();
            while(GetTime()-3<time_elapsed)
            {
                double akt = GetTime() - time_elapsed;
                BeginDrawing();
                ClearBackground(RAYWHITE);
                if(akt<1)
                {
                    DrawText("3", 550, 200, 260, RED);
                }
                else if(akt<2)
                {
                    DrawText("2", 550, 200, 260, RED);
                }
                else
                {
                    DrawText("1", 550, 200, 260, RED);
                }
                EndDrawing();
            }
            return;
        }
    }
}

Board::Board()
{
    bombs.clear();
    obstacle = new Obstacle();
}

void Board::Initialize()
{
    this->obstacle->Initialize(this->height,this->width);
}


void Board::Draw_Game_State()
{
    ClearBackground(RAYWHITE);
    for(int i=1;i<=height;i++)
    {
        for(int j=1;j<=width;j++)
        {
            Color col;
            if(i%2==0 && j%2==0) col = BLACK;
            else if(this->obstacle->Is_Obstacle(i,j)) col = DARKPURPLE;
            else col = LIGHTGRAY;
            DrawRectangle(width_marigin + (j-1) * square_size, height_marigin + (i-1) * square_size, square_size, square_size, col);
        }
    }
    this->Draw_Boost();
    this->Draw_Bombs();
}

bool Board::Is_Ok(int h,int w)
{
    if(h<=0 || w<=0) return false;
    if(h> this->height * this->square_size - Player::sz) return false;
    if(w> this->width * this->square_size - Player::sz) return false;
    int ph,pw;
    ph = (h/this->square_size) + 1;
    pw = (w/this->square_size) + 1;
    if(ph%2==0 && pw%2==0) return false;
    if(this->obstacle->Is_Obstacle(ph,pw)) return false;
    h += Player::sz - 1;
    ph = (h/this->square_size) + 1;
    pw = (w/this->square_size) + 1;
    if(ph%2==0 && pw%2==0) return false;
    if(this->obstacle->Is_Obstacle(ph,pw)) return false;
    w += Player::sz - 1;
    ph = (h/this->square_size) + 1;
    pw = (w/this->square_size) + 1;
    if(ph%2==0 && pw%2==0) return false;
    if(this->obstacle->Is_Obstacle(ph,pw)) return false;
    h -= Player::sz - 1;
    ph = (h/this->square_size) + 1;
    pw = (w/this->square_size) + 1;
    if(ph%2==0 && pw%2==0) return false;
    if(this->obstacle->Is_Obstacle(ph,pw)) return false;
    return true;
}

bool Board::Is_Free(int h,int w)
{
    if(h%2==0 && w%2==0) return false;
    return !this->obstacle->Is_Obstacle(h,w);
}

bool Board::Is_Boost(int h,int w)
{
    for(int i=0;i<boost.size();i++)
    {
        if(boost[i].Get_Y()==h && boost[i].Get_X()==w)
        {
            return true;
        }
    }
    return false;
}

int Board::Get_Board_Height()
{
    return board_height;
}

int Board::Get_Board_Width()
{
    return board_width;
}

int Board::Get_Square_Size()
{
    return square_size;
}

int Board::Get_Width()
{
    return width;
}

int Board::Get_Height()
{
    return height;
}

int Board::Get_Height_Marigin()
{
    return height_marigin;
}

int Board::Get_Width_Marigin()
{
    return width_marigin;
}

void Board::Erase_Obstacle(int i,int j)
{
    obstacle->Erase(i,j);
}

void Board::Draw_Bombs()
{
    for(int i=0;i<(int)this->bombs.size();i++)
    {
        bombs[i].Draw_Bomb(bombs[i],this);
    }
}

bool Board::Is_Bomb_Planted(int x,int y)
{
    for(int i=0;i<(int)bombs.size();i++)
    {
        if(bombs[i].Get_X() == x && bombs[i].Get_Y() == y) return true;
    }
    return false;
}

void Board::Add_Bomb(int i,int j,double time_added,int owner)
{
    Bomb bomb(i,j,time_added,owner);
    this->bombs.push_back(bomb);
}

int Board::Detonate_Bombs(Player& player1,Player& player2)
{
    int sz = this->bombs.size();
    double x = -1;
    set<pair<int,int> > detonated;
    for(int i=1;i<=sz+1;i++)
    {
        for(int i=0;i<sz;i++)
        {
            if(GetTime() - bombs[i].Get_Plant_Time() < 3)
            {
                if(detonated.count({bombs[i].Get_Y(),bombs[i].Get_X()}))
                {
                    if(x!=-1) bombs[i].Set_Plant_Time(x);
                }
            }
            if(GetTime() - bombs[i].Get_Plant_Time() >= 3)
            {
                if(x==-1) x = bombs[i].Get_Plant_Time();
                set<pair<int,int> > add_cells;
                if(bombs[i].Get_Owner()==1) add_cells = bombs[i].Cells_In_Range(player1,this);
                else add_cells = bombs[i].Cells_In_Range(player2,this);
                set<pair<int,int> >::iterator it = add_cells.begin();
                while(it!=add_cells.end())
                {
                    detonated.insert({it->first,it->second});
                    it++;
                }
            }
        }
    }
    set<pair<int,int> > destroy;
    for(int i=0;i<(int)this->bombs.size();i++)
    {
        set<pair<int,int> > cells;
        if(bombs[i].Get_Owner()==1) cells = bombs[i].Cells_In_Range(player1,this);
        else cells = bombs[i].Cells_In_Range(player2,this);
        double act_time = GetTime();
        if(act_time-3 >= bombs[i].Get_Plant_Time() && act_time-3.3<bombs[i].Get_Plant_Time())
        {
            bombs[i].Explosion(this,cells);
        }
        if(act_time-3.25>bombs[i].Get_Plant_Time())
        {
            set<pair<int,int> > ::iterator it = cells.begin();
            if(bombs[i].Get_Owner()==1) player1.Add_Bomb();
            else player2.Add_Bomb();
            while(it!=cells.end())
            {
                pair<int,int> cell = *it;
                destroy.insert(cell);
                it++;
            }
            bombs.erase(bombs.begin()+i);
            i--;
        }
    }
    set<pair<int,int> > ::iterator it = destroy.begin();
    while(it!=destroy.end())
    {
        pair<int,int> cell = *it;
        if(this->obstacle->Is_Obstacle(cell.first,cell.second))
        {
            this->obstacle->Erase(cell.first,cell.second);
            if(rand()%2)
            {
                this->Add_Boost(rand()%3+1,cell.first,cell.second);
            }
        }
        it++;
    }
    if(player1.Is_Dead(destroy,this) && player2.Is_Dead(destroy,this)) return 3;
    if(player1.Is_Dead(destroy,this)) return 2;
    if(player2.Is_Dead(destroy,this)) return 1;
    return 0;
}

int Board::Get_Bombs_Size()
{
    return bombs.size();
}

void Board::Draw_Boost()
{
    for(int i=0;i<(int)this->boost.size();i++)
    {
        boost[i].Draw(this);
    }
}

void Board::Add_Boost(int id,int y,int x)
{
    this->boost.push_back(Boost(id,y,x));
}

void Board::Check_Boost(Player * player)
{
    for(int i=0;i<(int)this->boost.size();i++)
    {
        if(this->boost[i].Check_Taken(player,this))
        {
            if(this->boost[i].Get_Type() == 1)
            {
                player->Add_Bomb();
            }
            else if(this->boost[i].Get_Type() == 2)
            {
                player->Increase_Speed();
            }
            else if(this->boost[i].Get_Type() == 3)
            {
                player->Increase_Range();
            }
            this->boost.erase(this->boost.begin()+i);
            i--;
        }
    }
}

Board::~Board()
{
    delete obstacle;
}

void Result::Draw_Winner(int index)
{
    while(!IsKeyPressed(KEY_C))
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if(index==1)
        {
            DrawText("GREEN PLAYER WINS!", 160, 220, 80, GREEN);
            DrawText("PRESS C TO CONTINUE", 120, 370, 80, DARKGRAY);
        }
        else if(index==2)
        {
            DrawText("BLUE PLAYER WINS!", 160, 220, 80, BLUE);
            DrawText("PRESS C TO CONTINUE", 120, 370, 80, DARKGRAY);
        }
        else
        {
            DrawText("DRAW!", 460, 220, 80, BLUE);
            DrawText("PRESS C TO CONTINUE", 120, 370, 80, DARKGRAY);
        }
        EndDrawing();
    }
    return;
}

bool Result::Ask_If_Again()
{
    while(!IsKeyPressed(KEY_Y) && !IsKeyPressed(KEY_N))
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("DO YOU WANT ONE MORE GAME? (Y/N)", 90, 280, 50, BLUE);
        EndDrawing();
        if(IsKeyPressed(KEY_Y)) return true;
        if(IsKeyPressed(KEY_N)) return false;
    }
    return false;
}

Player::Player(int id,int x,int y,Color col,int a,int b,int c,int d,int bomb_key)
{
    this->id = id;
    this->x = x;
    this->y = y;
    this->col = col;
    this->bomb_key = bomb_key;
    left = a;
    right = b;
    up = c;
    down = d;
    bombs_cnt = 1;
    bomb_range = 1;
    last_plant_time = 0.0;
    speed = 3;
}

void Player::Draw(Board * board)
{
    DrawRectangle(board->Get_Width_Marigin() + x,board->Get_Height_Marigin() + y, sz, sz, col);
}

void Player::Move(Board * board)
{
    int move_length = this->speed;
    for(int i=0;i<move_length;i++)
    {
        if(IsKeyDown(left) && board->Is_Ok(y,x-1)) x--;
    }
    for(int i=0;i<move_length;i++)
    {
        if(IsKeyDown(right) && board->Is_Ok(y,x+1)) x++;
    }
    for(int i=0;i<move_length;i++)
    {
        if(IsKeyDown(up) && board->Is_Ok(y-1,x)) y--;
    }
    for(int i=0;i<move_length;i++)
    {
        if(IsKeyDown(down) && board->Is_Ok(y+1,x)) y++;
    }
    return;
}

void Player::Plant_Bomb(Board * board)
{
    if(bombs_cnt && IsKeyDown(this->bomb_key))
    {
        int x = this->x;
        int y = this->y;
        x += Player::sz /2;
        y += Player::sz /2;
        x /= board->Get_Square_Size();
        y /= board->Get_Square_Size();
        x++; y++;
        if(GetTime() - this->Get_Last_Plant_Time() <0.1) return;
        if(board->Is_Bomb_Planted(x,y)) return;
        int owner = this->id;
        this->last_plant_time = GetTime();
        board->Add_Bomb(x,y,GetTime(),owner);
        bombs_cnt--;
    }
}

double Player::Get_Last_Plant_Time()
{
    return last_plant_time;
}

int Player::Get_Bomb_Range()
{
    return bomb_range;
}

void Player::Add_Bomb()
{
    bombs_cnt++;
}

void Player::Increase_Range()
{
    bomb_range++;
}

void Player::Increase_Speed()
{
    speed = min(speed+1,6);
}

bool Player::Is_Dead(set<pair<int,int> > s,Board * board)
{
    int w = this->x;
    int h = this->y;
    int ph,pw;
    ph = (h/board->Get_Square_Size()) + 1;
    pw = (w/board->Get_Square_Size()) + 1;
    if(s.count({ph,pw})) return true;
    h += Player::sz - 1;
    ph = (h/board->Get_Square_Size()) + 1;
    pw = (w/board->Get_Square_Size()) + 1;
    if(s.count({ph,pw})) return true;
    w += Player::sz - 1;
    ph = (h/board->Get_Square_Size()) + 1;
    pw = (w/board->Get_Square_Size()) + 1;
    if(s.count({ph,pw})) return true;
    h -= Player::sz - 1;
    ph = (h/board->Get_Square_Size()) + 1;
    pw = (w/board->Get_Square_Size()) + 1;
    if(s.count({ph,pw})) return true;
    return false;
}

int Player::Get_X()
{
    return x;
}

int Player::Get_Y()
{
    return y;
}

Obstacle::Obstacle()
{
    obs_list.clear();
}

void Obstacle::Initialize(int height,int width)
{
    obs_list.clear();
    for(int i=1;i<=height;i++)
    {
        for(int j=1;j<=width;j++)
        {
            if(i%2!=0 || j%2!=0)
            {
                int len1 = abs(i-1) + abs(j-1);
                int len2 = abs(height-i) + abs(width-j);
                if(rand()%3 && len1>=3 && len2>=3) obs_list.insert({i,j});
            }
        }
    }
}

bool Obstacle::Is_Obstacle(int h,int w)
{
    if(obs_list.count({h,w})) return true;
    return false;
}


void Obstacle::Erase(int i,int j)
{
    obs_list.erase({i,j});
}

Bomb::Bomb(int i,int j,double plant_time,int owner)
{
    x = i;
    y = j;
    this->plant_time = plant_time;
    this->owner = owner;
}

void Bomb::Draw_Bomb(Bomb bomb,Board * board)
{
    Color kolor;
    kolor.r = 255;
    kolor.g = 0;
    kolor.b = 0;
    kolor.a = 255;
    double time = GetTime();
    time -= this->Get_Plant_Time();
    time = min(time,(double)3);
    double dif = (time/(double)3)*255;
    kolor.g += dif;
    kolor.b += dif;
    int x = bomb.x;
    int y = bomb.y;
    DrawCircle(board->Get_Width_Marigin() + board->Get_Square_Size()/2 + (x-1) * board->Get_Square_Size(),board->Get_Height_Marigin() + board->Get_Square_Size()/2 + (y-1) * board->Get_Square_Size(),17, BLACK);
    DrawCircle(board->Get_Width_Marigin() + board->Get_Square_Size()/2 + (x-1) * board->Get_Square_Size(),board->Get_Height_Marigin() + board->Get_Square_Size()/2 + (y-1) * board->Get_Square_Size(),15, kolor);
}

int Bomb::Get_X()
{
    return x;
}

int Bomb::Get_Y()
{
    return y;
}

double Bomb::Get_Plant_Time()
{
    return plant_time;
}

int Bomb::Get_Owner()
{
    return owner;

}
void Bomb::Explosion(Board * board,set<pair<int,int> > s)
{
   set<pair<int,int> >::iterator it = s.begin();
   while(it!=s.end())
   {
       int y = it->first;
       int x = it->second;
       DrawRectangle(board->Get_Width_Marigin() + (x-1) * board->Get_Square_Size(), board->Get_Height_Marigin() + (y-1) * board->Get_Square_Size(), board->Get_Square_Size(), board->Get_Square_Size(), YELLOW);
       it++;
   }
}

set<pair<int,int> > Bomb::Cells_In_Range(Player player,Board * board)
{
    int range = player.Get_Bomb_Range();
    int x = this->x;
    int y = this->y;
    set<pair<int,int> > res;
    res.insert({y,x});
    for(int i=1;i<=range;i++)
    {
        int xp = x + i;
        int yp = y;
        if(xp<1 || xp>board->Get_Width() || yp<1 || yp>board->Get_Height()) break;
        if(xp%2==1 || yp%2==1) res.insert({yp,xp});
        bool is_free = board->Is_Free(yp,xp);
        if(!is_free) break;
    }
    for(int i=1;i<=range;i++)
    {
        int xp = x - i;
        int yp = y;
        if(xp<1 || xp>board->Get_Width() || yp<1 || yp>board->Get_Height()) break;
        if(xp%2==1 || yp%2==1) res.insert({yp,xp});
        bool is_free = board->Is_Free(yp,xp);
        if(!is_free) break;
    }
    for(int i=1;i<=range;i++)
    {
        int xp = x;
        int yp = y - i;
        if(xp<1 || xp>board->Get_Width() || yp<1 || yp>board->Get_Height()) break;
        if(xp%2==1 || yp%2==1) res.insert({yp,xp});
        bool is_free = board->Is_Free(yp,xp);
        if(!is_free) break;
    }
    for(int i=1;i<=range;i++)
    {
        int xp = x;
        int yp = y + i ;
        if(xp<1 || xp>board->Get_Width() || yp<1 || yp>board->Get_Height()) break;
        if(xp%2==1 || yp%2==1) res.insert({yp,xp});
        bool is_free = board->Is_Free(yp,xp);
        if(!is_free) break;
    }
    return res;
}

void Bomb::Set_Plant_Time(double p_time)
{
    plant_time = p_time;
}

Boost::Boost(int id,int y,int x)
{
    type = id;
    this->x = x;
    this->y = y;
}

void Boost::Draw(Board * board)
{
    if(type==1)
    {
        DrawRectangle(board->Get_Width_Marigin() + (x-1) * board->Get_Square_Size(), board->Get_Height_Marigin() + (y-1) * board->Get_Square_Size(), board->Get_Square_Size(), board->Get_Square_Size(), LIME);
        DrawCircle(board->Get_Width_Marigin() + board->Get_Square_Size()/2 + (x-1) * board->Get_Square_Size(),board->Get_Height_Marigin() + board->Get_Square_Size()/2 + (y-1) * board->Get_Square_Size(),15, BLACK);
    }
    else if(type==2)
    {
        DrawRectangle(board->Get_Width_Marigin() + (x-1) * board->Get_Square_Size(), board->Get_Height_Marigin() + (y-1) * board->Get_Square_Size(), board->Get_Square_Size(), board->Get_Square_Size(), LIME);
        Vector2 x1,x2,x3;
        x1.x = board->Get_Width_Marigin() + (x-1) * board->Get_Square_Size() + board->Get_Square_Size()/4;
        x2.x = x1.x + board->Get_Square_Size()/2;
        x1.y = x2.y = board->Get_Height_Marigin() + (y-1) * board->Get_Square_Size() + board->Get_Square_Size()*3/4;
        x3.x = (x1.x+x2.x) / 2;
        x3.y = x1.y - board->Get_Square_Size()/2;
        DrawTriangle(x1,x2,x3,GOLD);
    }
    else if(type==3)
    {
        DrawRectangle(board->Get_Width_Marigin() + (x-1) * board->Get_Square_Size(), board->Get_Height_Marigin() + (y-1) * board->Get_Square_Size(), board->Get_Square_Size(), board->Get_Square_Size(), LIME);
        DrawCircle(board->Get_Width_Marigin() + board->Get_Square_Size()/2 + (x-1) * board->Get_Square_Size() - board->Get_Square_Size()/4,board->Get_Height_Marigin() + board->Get_Square_Size()/2 + (y-1) * board->Get_Square_Size() + board->Get_Square_Size()/8,board->Get_Square_Size()/6, YELLOW);
        DrawCircle(board->Get_Width_Marigin() + board->Get_Square_Size()/2 + (x-1) * board->Get_Square_Size() + board->Get_Square_Size()/4,board->Get_Height_Marigin() + board->Get_Square_Size()/2 + (y-1) * board->Get_Square_Size() + board->Get_Square_Size()/8,board->Get_Square_Size()/6, YELLOW);
        DrawCircle(board->Get_Width_Marigin() + board->Get_Square_Size()/2 + (x-1) * board->Get_Square_Size() , board->Get_Height_Marigin() + board->Get_Square_Size()/2 + (y-1) * board->Get_Square_Size() - board->Get_Square_Size()/4,board->Get_Square_Size()/6, YELLOW);
    }
}

int Boost::Get_X()
{
    return x;
}

int Boost::Get_Y()
{
    return y;
}

bool Boost::Check_Taken(Player * player,Board * board)
{
    int w = player->Get_X();
    int h = player->Get_Y();
    int ph,pw;
    int x = this->Get_X();
    int y = this->Get_Y();
    ph = (h/board->Get_Square_Size()) + 1;
    pw = (w/board->Get_Square_Size()) + 1;
    if(x==pw && y==ph) return true;
    h += Player::sz - 1;
    ph = (h/board->Get_Square_Size()) + 1;
    pw = (w/board->Get_Square_Size()) + 1;
    if(x==pw && y==ph) return true;
    w += Player::sz - 1;
    ph = (h/board->Get_Square_Size()) + 1;
    pw = (w/board->Get_Square_Size()) + 1;
    if(x==pw && y==ph) return true;
    h -= Player::sz - 1;
    ph = (h/board->Get_Square_Size()) + 1;
    pw = (w/board->Get_Square_Size()) + 1;
    if(x==pw && y==ph) return true;
    return false;
}

int Boost::Get_Type()
{
    return type;
}


