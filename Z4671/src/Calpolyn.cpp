// Calpolyn.cpp: implementation of the CCalpolyn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "NTGraph_Test.h"
#include "Calpolyn.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCalpolyn::CCalpolyn()
{

}

CCalpolyn::~CCalpolyn()
{

}

double CCalpolyn::sum(double *dNumarry, int n)
{

	double *dTemp= new double[n];

    double dSum=0;

	dTemp=dNumarry;
	for (int i=0;i<n;i++) 
	{
		dSum+=dTemp[i];
	}
	
	return dSum;


}

double CCalpolyn::MutilSum(double *dX, double *dY, int n)
{
	double * dXTemp= new double [n];
	double * dYTemp= new double [n];
    double dMultiSum=0;

	dXTemp=dX;
	dYTemp=dY;

	for (int i=0;i<n;i++)
	{
       dMultiSum += dX[i]*dY[i];
	}

	return dMultiSum;

}

double CCalpolyn::RelatePow(double *dx, int n, int ex)
{
	double * dTemp =new double[n];
	double ReSum=0;
    dTemp=dx;
	for (int j=0;j<n;j++) 
	{
        ReSum+=pow(dTemp[j],ex);
	}
    return ReSum;

}

double CCalpolyn::RelateMutiXY(double *dx, double *dy, int n, int ex)
{
	double * dXTemp= new double [n];
	double * dYTemp= new double [n];
    double dReMultiSum=0;

	dXTemp=dx;
	dYTemp=dy;

	for (int i=0;i<n;i++) 
	{
		 dReMultiSum+=pow(dXTemp[i],ex)*dYTemp[i];
	}
	return dReMultiSum;

}
//------------------------------------------------------------------
//函数名：EMatrix
//输入：double *dx 输入点的x坐标 double *dy 输入点的y坐标
//      int n  输入点的个数  int ex 拟合的阶数
//      double coefficient[]  拟合后获得的系数数组
//注意：
//     拟合的阶数输入的时候需要加一，如果要求4阶那么需要输入5
//     拟合后的系数数组不是从0下标开始而是从1下标开始
//作者：wanxin wang
//时间：2010.2.20
//-------------------------------------------------------------------
void CCalpolyn::EMatrix(double *dx, double *dy, int n, int ex, double coefficient[])
{
    double * dXTemp= new double [n];
	double * dYTemp= new double [n];
	dXTemp=dx;
	dYTemp=dy;

	for(int i=1;i<=ex;i++)
	{

		for(int j=1;j<=ex;j++)
		{
			Em[i][j]=RelatePow(dXTemp,n,i+j-2);
		}
		Em[i][ex+1]=RelateMutiXY(dXTemp,dYTemp,n,i-1);
	}
	Em[1][1]=n;
    CalEquation(ex,coefficient);	

}

void CCalpolyn::CalEquation(int exp, double coefficient[])
{
	for(int k=1;k<exp;k++) //消元过程
	{
		for(int i=k+1;i<exp+1;i++)
		{
			double p1=0;

			if(Em[k][k]!=0)
				p1=Em[i][k]/Em[k][k];

			for(int j=k;j<exp+2;j++) 
				Em[i][j]=Em[i][j]-Em[k][j]*p1;
		}
	}

	coefficient[exp]=Em[exp][exp+1]/Em[exp][exp];
	for(int l=exp-1;l>=1;l--)   //回代求解
		coefficient[l]=(Em[l][exp+1]-F(coefficient,l+1,exp))/Em[l][l];
}

double CCalpolyn::F(double c[], int l, int m)
{
	double sum=0;
	for(int i=l;i<=m;i++)
		sum+=Em[l-1][i]*c[i];
	return sum; 
}
