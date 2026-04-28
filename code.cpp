#include <iostream>
using namespace std;

const int SIZE = 10;
const int MAX = 10000;

// Class State
class State
{
public:
    int x, y, fuel;
    char c1, c2, c3, c4;
    int parent;
};

// Class Game
class Game
{
public:
    int grid[SIZE][SIZE];

    int coinX[4] = {2,6,5,8};
    int coinY[4] = {3,2,7,8};

    int fuelX = 4;
    int fuelY = 9;

    Game()
    {
        for(int i=0;i<SIZE;i++)
            for(int j=0;j<SIZE;j++)
                grid[i][j]=0;

        grid[3][7]=1;
        grid[3][8]=1;
        grid[4][7]=1;
        grid[4][8]=1;
    }

    bool valid(int x,int y)
    {
        return (x>=0 && x<SIZE && y>=0 && y<SIZE && grid[x][y]==0);
    }

    bool goal(State s)
    {
        return (s.x==1 && s.y==1 &&
                s.c1=='t' && s.c2=='t' &&
                s.c3=='t' && s.c4=='t');
    }

    void collectCoin(State &s)
    {
        if(s.x==coinX[0] && s.y==coinY[0]) s.c1='t';
        if(s.x==coinX[1] && s.y==coinY[1]) s.c2='t';
        if(s.x==coinX[2] && s.y==coinY[2]) s.c3='t';
        if(s.x==coinX[3] && s.y==coinY[3]) s.c4='t';
    }

    void refill(State &s)
    {
        if(s.x==fuelX && s.y==fuelY)
            s.fuel=20;
    }

    State move(State s,int dx,int dy)
    {
        s.x += dx;
        s.y += dy;
        s.fuel--;

        collectCoin(s);
        refill(s);

        return s;
    }

    void print(State s)
    {
        cout<<"("<<s.x<<","<<s.y<<","<<s.fuel<<","
            <<s.c1<<","<<s.c2<<","<<s.c3<<","<<s.c4<<")";
    }
};

// Class Search
class Search
{
public:

    int absNum(int x)
    {
        if(x<0) return -x;
        return x;
    }

    int dist(int x1,int y1,int x2,int y2)
    {
        return absNum(x1-x2)+absNum(y1-y2);
    }

    bool same(State a,State b)
    {
        return (a.x==b.x && a.y==b.y &&
                a.fuel==b.fuel &&
                a.c1==b.c1 && a.c2==b.c2 &&
                a.c3==b.c3 && a.c4==b.c4);
    }

    bool exists(State arr[],int n,State s)
    {
        for(int i=0;i<n;i++)
            if(same(arr[i],s))
                return true;

        return false;
    }

    void printPath(State arr[],int i,Game &g) 
    {
        if(i==-1) return;

        printPath(arr,arr[i].parent,g);
        g.print(arr[i]);
        cout<<endl;
    }
};