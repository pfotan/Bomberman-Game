#include <bits/stdc++.h>
#include "raylib.h"
#include "bomberman.h"

using namespace std;


int main()
{
    srand(time(NULL));
    Game game;
    Result result;
    while(game.Start());
    CloseWindow();
    return 0;
}
