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
    int i,j,mindist[N][N],mincost[N][N],m1[N][N],m2[N][N];    //m1��m2�ֱ��ʾ�������ʹ��۾���
    FILE *fp;
    for(i=0;i<N;i++)
    {
        visited[i]=0;
        bestPath[i]=0;
    }
    fp=fopen("m1.txt","r");    //���ļ��еľ������m1��������mindist[N][N]
    if(fp==NULL)
    {
        printf("can not open file\n");
        return 1;
    }
    for(i=0;i<N;i++)
        for(j=0;j<N;j++)
            fscanf(fp,"%d",&mindist[i][j]);
    fclose(fp);     //�������m1�������
    fp=fopen("m2.txt","r");     //���ļ��еľ������m2��������mincost[N][N]
    if(fp==NULL)
    {
        printf("can not open file\n");
        return 1;
    }
    for(i=0;i<N;i++)
        for(j=0;j<N;j++)
            fscanf(fp,"%d",&mincost[i][j]);
    fclose(fp);     //���۾���m2�������
    copy(m1,mindist);      //mindist[N][N]��ֵ��m1[N][N]
    copy(m2,mincost);      //mincost[N][N]��ֵ��m2[N][N]
    for(i=0;i<N;i++)        //��ʼ������Խ�Ԫ�أ����е�������ͨ������Ϊ0
    {
        mindist[i][i]=9999;
        mincost[i][i]=0;
    }
    Floyd(mindist);     //�ø��������㷨�����������м����̾��룬�洢��mindist[N][N]
    Floyd(mincost);     //�ø��������㷨�����������м����С���ۣ��洢��mincost[N][N]
    BranchAndBound(m1,m2,mindist,mincost);      //�÷�֧���編�ҳ����п���·�������
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

void copy(int a[N][N],int b[N][N])     //�Ѿ���b��ֵ������a
{
    int i,j;
    for(i=0;i<N;i++)
        for(j=0;j<N;j++)
            a[i][j]=b[i][j];
}

void BranchAndBound(int m1[N][N],int m2[N][N],int mindist[N][N],int mincost[N][N])
{
    int stack[MAX],depth=0,next,i,j;        //����ջ��depth��ʾջ��ָ�룬nextָ��ÿ�α���ʱ��ǰ�������е���һ���Ѿ������ĳ���
    int bestLength,shortestDist,minimumCost,distBound=9999,costBound=1500;
    int cur,currentDist=0,currentCost=0;        //curָ��ǰ�������У�currentDist��currentCost�ֱ��ʾ�Ӽ׳��е���ǰ���е���̾������С����
    stack[depth]=0;     //ջ�ĳ�ʼ��
    stack[depth+1]=0;
    visited[0]=1;       //visited[0]=1��ʶ�Ӽ׳��г������б������׳����ѱ�����
    while(depth>=0)     //��ʾ������ʼ�ͽ�����������ʼʱ�Ӽ׳��г�����ջ��depth=0;����ʱ������ϣ����н�����ջ��depth=0
    {           //�ӵ�ǰ������Ѱ��һ���ڽ��ĳ���
        cur=stack[depth];       //ȡջ���ڵ㸳ֵ��cur����ʾ��ǰ���ʵ�cur�ų���
        next=stack[depth+1];    //nextָ��ǰ�������е���һ���Ѿ������ĳ���
        for(i=next+1;i<N;i++)       //��̽��ǰ�������е�ÿһ�����ڳ���
        {
            if((currentCost+mincost[cur][N-1]>costBound)||(currentDist+mindist[cur][N-1]>=distBound))       //��̽���������֦���������м�֦
            {
                continue;
            }
            if(m1[cur][i]==9999)
                continue;       //����̽���в���ͨ
            if(visited[i]==1)
                continue;       //����̽�����ѱ�����
            if(i<N)
                break;      //����̽�������������������ֹforѭ��
        }
        if(i==N)        //������������г��У����л���
        {
            depth--;
            currentDist-=m1[stack[depth]][stack[depth+1]];
            currentCost-=m2[stack[depth]][stack[depth+1]];
            visited[stack[depth+1]]=0;
        }
        else        //�ҵ���һ�����е��ڽ�����
        {
            currentDist+=m1[cur][i];       //�ѿ��г��еľ������currentDist
            currentCost+=m2[cur][i];       //�ѿ��г��еĴ��ۼ���currentCost
            depth++;        //�ҵ��Ŀ��г�����ջ
            stack[depth]=i;     //����ջ��ָ��ָ���ҵ��Ŀ��г���
            stack[depth+1]=0;
            visited[i]=1;       //�޸��ҵ��ĳ��еķ��ʱ�־
            if(i==N-1)      //���ʵ����ҳ��У���������г��е�һ���������ҵ���һ��ͨ·
            {
                if(currentCost>1500)        //�����ǰͨ·���ܴ���>1500�������ͨ·
                    continue;
                distBound=currentDist;      //���¼�֦��·���߽磬����Ժ��ҵ���ͨ·������ڱ߽�ͼ�֦
                shortestDist=currentDist;       //����ͨ·�ܾ���
                minimumCost=currentCost;        //����ͨ·�ܴ���
                for(j=0;j<=depth;j++)       //���浱ǰ�ҵ���ͨ·�����������н��
                    bestPath[j]=stack[j];
                bestLength=depth;       //���浱ǰ�ҵ���ͨ·�Ľڵ���
                depth--;        //��������ջ��������ֵ�����л��ݣ�Ѱ���µĿ���ͨ·
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
    printf("��̾���:%d,��·���ܶ�:%d,·��Ϊ:\n",shortestDist,minimumCost);
    for(i=0;i<bestLength;i++)      //������н��
        printf("%d-->",bestPath[i]+1);
    printf("%d",bestPath[bestLength]+1);
}
