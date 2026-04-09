// Calpolyn.h: interface for the CCalpolyn class.
// this document is  the mothed of least squaeres 's high-ranking culve 
// author :wanxin wang
// Date :  2010.5.20
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALPOLYN_H__1FAC3912_F7A0_4974_9C8D_169C934947CB__INCLUDED_)
#define AFX_CALPOLYN_H__1FAC3912_F7A0_4974_9C8D_169C934947CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCalpolyn  
{
public:
	double *dArryX;
	double *dArryY;
	double F(double c[],int l,int m);
	void CalEquation(int exp,double coefficient[] );
	void EMatrix(double *dx,double*dy,int n,int ex,double coefficient[] );
	double RelateMutiXY(double *dx,double*dy,int n,int ex);
	double RelatePow(double *dx,int n,int ex);
	double MutilSum(double* dX,double *dY,int n);
	double sum(double * dNumarry,int n);
	CCalpolyn();
	virtual ~CCalpolyn();
private:
	double Em[10][10];

};

#endif // !defined(AFX_CALPOLYN_H__1FAC3912_F7A0_4974_9C8D_169C934947CB__INCLUDED_)
