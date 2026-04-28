#include <iostream>
using namespace std;

const int SIZE = 10;
const int MAX = 10000;

// ================= STATE =================
class State
{
public:
    int x, y, fuel;
    char c1, c2, c3, c4;
    int parent;
};

// ================= GAME =================
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

// ================= SEARCH =================
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

    // h1 = nearest neighbor sum
    int h1(State s,Game &g)
    {
        int total=0;
        int cx=s.x, cy=s.y;

        char a=s.c1,b=s.c2,c=s.c3,d=s.c4;

        while(true)
        {
            int best=9999;
            int choose=0;

            if(a=='f')
            {
                int t=dist(cx,cy,g.coinX[0],g.coinY[0]);
                if(t<best){best=t; choose=1;}
            }

            if(b=='f')
            {
                int t=dist(cx,cy,g.coinX[1],g.coinY[1]);
                if(t<best){best=t; choose=2;}
            }

            if(c=='f')
            {
                int t=dist(cx,cy,g.coinX[2],g.coinY[2]);
                if(t<best){best=t; choose=3;}
            }

            if(d=='f')
            {
                int t=dist(cx,cy,g.coinX[3],g.coinY[3]);
                if(t<best){best=t; choose=4;}
            }

            if(choose==0) break;

            total += best;

            if(choose==1){cx=g.coinX[0]; cy=g.coinY[0]; a='t';}
            if(choose==2){cx=g.coinX[1]; cy=g.coinY[1]; b='t';}
            if(choose==3){cx=g.coinX[2]; cy=g.coinY[2]; c='t';}
            if(choose==4){cx=g.coinX[3]; cy=g.coinY[3]; d='t';}
        }

        total += dist(cx,cy,1,1);
        return total;
    }

    // h2 = max distance
    int h2(State s,Game &g)
    {
        int best=0;

        if(s.c1=='f')
        {
            int t=dist(s.x,s.y,g.coinX[0],g.coinY[0]);
            if(t>best) best=t;
        }

        if(s.c2=='f')
        {
            int t=dist(s.x,s.y,g.coinX[1],g.coinY[1]);
            if(t>best) best=t;
        }

        if(s.c3=='f')
        {
            int t=dist(s.x,s.y,g.coinX[2],g.coinY[2]);
            if(t>best) best=t;
        }

        if(s.c4=='f')
        {
            int t=dist(s.x,s.y,g.coinX[3],g.coinY[3]);
            if(t>best) best=t;
        }

        return best;
    }

    // ================= GREEDY =================
    void Greedy(Game &g,State start,int type)
    {
        State open[MAX], close[MAX];
        int parent[MAX], h[MAX];

        int openCount=1, closeCount=0;

        start.parent=-1;
        open[0]=start;
        parent[0]=-1;
        h[0]=(type==1)?h1(start,g):h2(start,g);

        while(openCount>0)
        {
            int best=0;

            for(int i=1;i<openCount;i++)
                if(h[i]<h[best]) best=i;

            State cur=open[best];
            cur.parent=parent[best];

            for(int i=best;i<openCount-1;i++)
            {
                open[i]=open[i+1];
                parent[i]=parent[i+1];
                h[i]=h[i+1];
            }

            openCount--;

            if(exists(close,closeCount,cur))
                continue;

            close[closeCount]=cur;
            int idx=closeCount++;
            
            cout<<"Visited: ";
            g.print(cur);
            cout<<endl;

            if(g.goal(cur))
            {
                cout<<"Goal Found\n";
                cout<<"Visited Nodes = "<<closeCount<<endl;
                cout<<"Path:\n";
                printPath(close,idx,g);
                cout<<"Cost = "<<idx<<endl;
                return;
            }

            if(cur.fuel<=0) continue;

            int dx[4]={1,-1,0,0};
            int dy[4]={0,0,1,-1};

            for(int i=0;i<4;i++)
            {
                int nx=cur.x+dx[i];
                int ny=cur.y+dy[i];

                if(g.valid(nx,ny))
                {
                    State next=g.move(cur,dx[i],dy[i]);
                    next.parent=idx;

                    if(!exists(close,closeCount,next))
                    {
                        open[openCount]=next;
                        parent[openCount]=idx;
                        h[openCount]=(type==1)?h1(next,g):h2(next,g);
                        openCount++;
                    }
                }
            }
        }

        cout<<"No Solution\n";
    }

    // ================= A* =================
    void AStar(Game &g,State start,int type)
    {
        State open[MAX], close[MAX];
        int parent[MAX], gCost[MAX], f[MAX];

        int openCount=1, closeCount=0;

        start.parent=-1;
        open[0]=start;
        parent[0]=-1;
        gCost[0]=0;
        f[0]=(type==1)?h1(start,g):h2(start,g);

        while(openCount>0)
        {
            int best=0;

            for(int i=1;i<openCount;i++)
                if(f[i]<f[best]) best=i;

            State cur=open[best];
            cur.parent=parent[best];
            int cost=gCost[best];

            for(int i=best;i<openCount-1;i++)
            {
                open[i]=open[i+1];
                parent[i]=parent[i+1];
                gCost[i]=gCost[i+1];
                f[i]=f[i+1];
            }

            openCount--;

            if(exists(close,closeCount,cur))
                continue;

            close[closeCount]=cur;
            int idx=closeCount++;

            cout<<"Visited: ";
            g.print(cur);
            cout<<endl;

            if(g.goal(cur))
            {
                cout<<"Goal Found\n";
                cout<<"Visited Nodes = "<<closeCount<<endl;
                cout<<"Path:\n";
                printPath(close,idx,g);
                cout<<"Cost = "<<cost<<endl;
                return;
            }

            if(cur.fuel<=0) continue;

            int dx[4]={1,-1,0,0};
            int dy[4]={0,0,1,-1};

            for(int i=0;i<4;i++)
            {
                int nx=cur.x+dx[i];
                int ny=cur.y+dy[i];

                if(g.valid(nx,ny))
                {
                    State next=g.move(cur,dx[i],dy[i]);
                    next.parent=idx;

                    if(!exists(close,closeCount,next))
                    {
                        open[openCount]=next;
                        parent[openCount]=idx;
                        gCost[openCount]=cost+1;

                        int hh=(type==1)?h1(next,g):h2(next,g);
                        f[openCount]=gCost[openCount]+hh;

                        openCount++;
                    }
                }
            }
        }

        cout<<"No Solution\n";
    }
};

// ================= MAIN =================
int main()
{
    Game g;
    Search s;
    State start;

    cout<<"Enter start x y fuel: ";
    cin>>start.x>>start.y>>start.fuel;

    if(!g.valid(start.x,start.y))
    {
        cout<<"Invalid Start";
        return 0;
    }

    start.c1='f';
    start.c2='f';
    start.c3='f';
    start.c4='f';

    g.collectCoin(start);
    g.refill(start);

    cout<<"\nGreedy using h1\n";
    s.Greedy(g,start,1);

    cout<<"\nGreedy using h2\n";
    s.Greedy(g,start,2);

    cout<<"\nA* using h1\n";
    s.AStar(g,start,1);

    cout<<"\nA* using h2\n";
    s.AStar(g,start,2);

    return 0;
}