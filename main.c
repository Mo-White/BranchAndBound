#include <stdlib.h>
#include <stdio.h>
#define N 50
#define MAX 52
void copy(int a[N][N],int b[N][N]);
void Floyd(int d[N][N]);
void BranchAndBound(int m1[N][N],int m2[N][N],int mindist[N][N],int mincost[N][N]);
int visited[N],bestPath[N];
int main()
{
    int i,j,mindist[N][N],mincost[N][N],m1[N][N],m2[N][N];    //m1、m2分别表示距离矩阵和代价矩阵
    FILE *fp;
    for(i=0;i<N;i++)
    {
        visited[i]=0;
        bestPath[i]=0;
    }
    fp=fopen("m1.txt","r");    //把文件中的距离矩阵m1读入数组mindist[N][N]
    if(fp==NULL)
    {
        printf("can not open file\n");
        return 1;
    }
    for(i=0;i<N;i++)
        for(j=0;j<N;j++)
            fscanf(fp,"%d",&mindist[i][j]);
    fclose(fp);     //距离矩阵m1读入完毕
    fp=fopen("m2.txt","r");     //把文件中的距离矩阵m2读入数组mincost[N][N]
    if(fp==NULL)
    {
        printf("can not open file\n");
        return 1;
    }
    for(i=0;i<N;i++)
        for(j=0;j<N;j++)
            fscanf(fp,"%d",&mincost[i][j]);
    fclose(fp);     //代价矩阵m2读入完毕
    copy(m1,mindist);      //mindist[N][N]赋值给m1[N][N]
    copy(m2,mincost);      //mincost[N][N]赋值给m2[N][N]
    for(i=0;i<N;i++)        //初始化矩阵对角元素，城市到自身不连通，代价为0
    {
        mindist[i][i]=9999;
        mincost[i][i]=0;
    }
    Floyd(mindist);     //用弗洛伊德算法求任意两城市间的最短距离，存储在mindist[N][N]
    Floyd(mincost);     //用弗洛伊德算法求任意两城市间的最小代价，存储在mincost[N][N]
    BranchAndBound(m1,m2,mindist,mincost);      //用分支定界法找出所有可行路径并输出
    return 0;
}

void Floyd(int d[N][N])
{
    int v,w,u;
    for(u=0;u<N;u++)
    {
        for(v=0;v<N;v++)
        {
            for(w=0;w<N;w++)
                if(d[v][u]+d[u][w]<d[v][w])
                {
                    d[v][w]=d[v][u]+d[u][w];
                }
        }
    }
}

void copy(int a[N][N],int b[N][N])     //把矩阵b赋值给矩阵a
{
    int i,j;
    for(i=0;i<N;i++)
        for(j=0;j<N;j++)
            a[i][j]=b[i][j];
}

void BranchAndBound(int m1[N][N],int m2[N][N],int mindist[N][N],int mincost[N][N])
{
    int stack[MAX],depth=0,next,i,j;        //定义栈，depth表示栈顶指针，next指向每次遍历时当前所处城市的上一个已经遍历的城市
    int bestLength,shortestDist,minimumCost,distBound=9999,costBound=1500;
    int cur,currentDist=0,currentCost=0;        //cur指向当前所处城市，currentDist和currentCost分别表示从甲城市到当前城市的最短距离和最小代价
    stack[depth]=0;     //栈的初始化
    stack[depth+1]=0;
    visited[0]=1;       //visited[0]=1标识从甲城市出发进行遍历，甲城市已被访问
    while(depth>=0)     //表示遍历开始和结束条件，开始时从甲城市出发，栈空depth=0;结束时遍历完毕，所有结点均出栈，depth=0
    {           //从当前城市中寻找一个邻近的城市
        cur=stack[depth];       //取栈顶节点赋值给cur，表示当前访问第cur号城市
        next=stack[depth+1];    //next指向当前所处城市的上一个已经遍历的城市
        for(i=next+1;i<N;i++)       //试探当前所处城市的每一个相邻城市
        {
            if((currentCost+mincost[cur][N-1]>costBound)||(currentDist+mindist[cur][N-1]>=distBound))       //试探城市满足剪枝条件，进行剪枝
            {
                continue;
            }
            if(m1[cur][i]==9999)
                continue;       //所试探城市不连通
            if(visited[i]==1)
                continue;       //所试探城市已被访问
            if(i<N)
                break;      //所试探城市满足访问条件，终止for循环
        }
        if(i==N)        //如果搜索完所有城市，进行回溯
        {
            depth--;
            currentDist-=m1[stack[depth]][stack[depth+1]];
            currentCost-=m2[stack[depth]][stack[depth+1]];
            visited[stack[depth+1]]=0;
        }
        else        //找到了一个可行的邻近城市
        {
            currentDist+=m1[cur][i];       //把可行城市的距离加入currentDist
            currentCost+=m2[cur][i];       //把可行城市的代价加入currentCost
            depth++;        //找到的可行城市入栈
            stack[depth]=i;     //更新栈顶指针指向找到的可行城市
            stack[depth+1]=0;
            visited[i]=1;       //修改找到的城市的访问标志
            if(i==N-1)      //访问到了乙城市，完成了所有城市的一次搜索，找到了一条通路
            {
                if(currentCost>1500)        //如果当前通路的总代价>1500则放弃该通路
                    continue;
                distBound=currentDist;      //更新剪枝的路径边界，如果以后找到的通路距离大于边界就剪枝
                shortestDist=currentDist;       //保存通路总距离
                minimumCost=currentCost;        //保存通路总代价
                for(j=0;j<=depth;j++)       //保存当前找到的通路所经过的所有结点
                    bestPath[j]=stack[j];
                bestLength=depth;       //保存当前找到的通路的节点数
                depth--;        //连续弹出栈顶的两个值，进行回溯，寻找新的可行通路
                currentDist-=m1[stack[depth]][stack[depth+1]];
                currentCost-=m2[stack[depth]][stack[depth+1]];
                visited[stack[depth+1]]=0;
                depth--;
                currentDist-=m1[stack[depth]][stack[depth+1]];
                currentCost-=m2[stack[depth]][stack[depth+1]];
                visited[stack[depth+1]]=0;
            }
        }
    }
    printf("最短距离:%d,养路费总额:%d,路径为:\n",shortestDist,minimumCost);
    for(i=0;i<bestLength;i++)      //输出所有结点
        printf("%d-->",bestPath[i]+1);
    printf("%d",bestPath[bestLength]+1);
}
