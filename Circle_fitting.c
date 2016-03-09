#include "stm32f4xx.h"
#include "Circle_fitting.h"

double absxx(double x){
	if(x<0) return -x;
	return x;
}

struct Circle{
	double s,g,a,b,r;
	int i,j;
}Old,New;

void CircleFitByTaubin (int datan, int16_t* dataX, int16_t* dataY,double* rDCx, double* rDCy, double* rDCr)
/*
      Circle fit to a given set of data points (in 2D)

      This is an algebraic fit, due to Taubin, based on the journal article

      G. Taubin, "Estimation Of Planar Curves, Surfaces And Nonplanar
                  Space Curves Defined By Implicit Equations, With
                  Applications To Edge And Range Image Segmentation",
                  IEEE Trans. PAMI, Vol. 13, pages 1115-1138, (1991)

      Input:  data     - the class of data (contains the given points):

	      data.n   - the number of data points
	      data.X[] - the array of X-coordinates
	      data.Y[] - the array of Y-coordinates

     Output:
               circle - parameters of the fitting circle:

	       circle.a - the X-coordinate of the center of the fitting circle
	       circle.b - the Y-coordinate of the center of the fitting circle
 	       circle.r - the radius of the fitting circle
 	       circle.s - the root mean square error (the estimate of sigma)
 	       circle.j - the total number of iterations

     The method is based on the minimization of the function

                  sum [(x-a)^2 + (y-b)^2 - R^2]^2
              F = -------------------------------
                      sum [(x-a)^2 + (y-b)^2]

     This method is more balanced than the simple Kasa fit.

     It works well whether data points are sampled along an entire circle or
     along a small arc.

     It still has a small bias and its statistical accuracy is slightly
     lower than that of the geometric fit (minimizing geometric distances),
     but slightly higher than that of the very similar Pratt fit.
     Besides, the Taubin fit is slightly simpler than the Pratt fit

     It provides a very good initial guess for a subsequent geometric fit.

       Nikolai Chernov  (September 2012)

*/
{
    int i,iter,IterMAX=20;

    double Xi,Yi,Zi;
    double Mz,Mxy,Mxx,Myy,Mxz,Myz,Mzz,Cov_xy,Var_z;
    double A0,A1,A2,A22,A3,A33;
    double Dy,xnew,x,ynew,y;
    double DET,Xcenter,Ycenter;



    // Compute x- and y- sample means (via a function in the class "data")

//     computing moments

	Mxx=Myy=Mxy=Mxz=Myz=Mzz=0.;

    for (i=0; i<datan; i++)
    {
        Xi = dataX[i];// - data.meanX;   //  centered x-coordinates
        Yi = dataY[i];// - data.meanY;   //  centered y-coordinates
        Zi = Xi*Xi + Yi*Yi;

        Mxy += Xi*Yi;
        Mxx += Xi*Xi;
        Myy += Yi*Yi;
        Mxz += Xi*Zi;
        Myz += Yi*Zi;
        Mzz += Zi*Zi;
    }
    Mxx /= datan;
    Myy /= datan;
    Mxy /= datan;
    Mxz /= datan;
    Myz /= datan;
    Mzz /= datan;

//      computing coefficients of the characteristic polynomial

    Mz = Mxx + Myy;
    Cov_xy = Mxx*Myy - Mxy*Mxy;
    Var_z = Mzz - Mz*Mz;
    A3 = 4*Mz;
    A2 = -3*Mz*Mz - Mzz;
    A1 = Var_z*Mz + 4*Cov_xy*Mz - Mxz*Mxz - Myz*Myz;
    A0 = Mxz*(Mxz*Myy - Myz*Mxy) + Myz*(Myz*Mxx - Mxz*Mxy) - Var_z*Cov_xy;
    //A0=Mxz*Mxz*Myy + Myz*Myz*Mxx - Mzz*Cov_xy - 2*Mxz*Myz*Mxy + Mz*Mz*Cov_xy;
    A22 = A2 + A2;
    A33 = A3 + A3 + A3;

//    finding the root of the characteristic polynomial
//    using Newton's method starting at x=0
//     (it is guaranteed to converge to the right root)

    for (x=0.,y=A0,iter=0; iter<IterMAX; iter++)  // usually, 4-6 iterations are enough
    {
    	Dy = A1 + x*(A22 + A33*x);
        xnew = x - y/Dy;
        if ((xnew == x)||((xnew>999999)||(xnew<-999999))) break;
        ynew = A0 + xnew*(A1 + xnew*(A2 + xnew*A3));
        if (absxx(ynew)>=absxx(y))  break;
        x = xnew;  y = ynew;
    }

//       computing paramters of the fitting circle

    DET = x*x - x*Mz + Cov_xy;
    Xcenter = (Mxz*(Myy - x) - Myz*Mxy)/DET/2;
    Ycenter = (Myz*(Mxx - x) - Mxz*Mxy)/DET/2;

//       assembling the output

    *rDCx = Xcenter;// + data.meanX;
    *rDCy = Ycenter;// + data.meanY;
    *rDCr = sqrt(Xcenter*Xcenter + Ycenter*Ycenter + Mz);
    //circle.r = sqrt(Xcenter*Xcenter + Ycenter*Ycenter + Mz);
    //circle.s = Sigma(data,circle);
    //circle.i = 0;
    //circle.j = iter;  //  return the number of iterations, too

    //return circle;
}


//****************** Sigma ************************************
//
//   estimate of Sigma = square root of RSS divided by N
//   gives the root-mean-square error of the geometric circle fit

double Sigma (int n, int16_t* dataX, int16_t* dataY, double Nx, double Ny, double Nr)
{
    double sum=0.0,dx,dy;

    for (int i=0; i<n; i++)
    {
        dx = dataX[i] - Nx;
        dy = dataY[i] - Ny;
        sum += (sqrt(dx*dx+dy*dy) - Nr) * (sqrt(dx*dx+dy*dy) - Nr);
    }
    return sqrt(sum/n);
}



//int CircleFitByLevenbergMarquardtFull (Data& data, Circle& circleIni, reals LambdaIni, Circle& circle)
int CircleFitByLevenbergMarquardtFull(int datan, int16_t* dataX, int16_t* dataY,double circleinix,double circleiniy,double circleinir, double LambdaIni, double *circlex,double *circley,double *circler)
/*                                     <------------------ Input ------------------->  <-- Output -->

       Geometric circle fit to a given set of data points (in 2D)

       Input:  data     - the class of data (contains the given points):

	       data.n   - the number of data points
	       data.X[] - the array of X-coordinates
	       data.Y[] - the array of Y-coordinates

               circleIni - parameters of the initial circle ("initial guess")

	       circleIni.a - the X-coordinate of the center of the initial circle
	       circleIni.b - the Y-coordinate of the center of the initial circle
	       circleIni.r - the radius of the initial circle

	       LambdaIni - the initial value of the control parameter "lambda"
	                   for the Levenberg-Marquardt procedure
	                   (common choice is a small positive number, e.g. 0.001)

       Output:
	       integer function value is a code:
	                  0:  normal termination, the best fitting circle is
	                      successfully found
	                  1:  the number of outer iterations exceeds the limit (99)
	                      (indicator of a possible divergence)
	                  2:  the number of inner iterations exceeds the limit (99)
	                      (another indicator of a possible divergence)
	                  3:  the coordinates of the center are too large
	                      (a strong indicator of divergence)

	       circle - parameters of the fitting circle ("best fit")

	       circle.a - the X-coordinate of the center of the fitting circle
	       circle.b - the Y-coordinate of the center of the fitting circle
 	       circle.r - the radius of the fitting circle
 	       circle.s - the root mean square error (the estimate of sigma)
 	       circle.i - the total number of outer iterations (updating the parameters)
 	       circle.j - the total number of inner iterations (adjusting lambda)

       Algorithm:  Levenberg-Marquardt running over the full parameter space (a,b,r)

       See a detailed description in Section 4.5 of the book by Nikolai Chernov:
       "Circular and linear regression: Fitting circles and lines by least squares"
       Chapman & Hall/CRC, Monographs on Statistics and Applied Probability, volume 117, 2010.

		Nikolai Chernov,  February 2014
*/
{
	 int code,i,iter,inner,IterMAX=20;

	    double factorUp=10.,factorDown=0.04,lambda,ParLimit=1.e+6;
	    double dx,dy,ri,u,v;
	    double Mu,Mv,Muu,Mvv,Muv,Mr,UUl,VVl,Nl,F1,F2,F3,dX,dY,dR;
	    double epsilon=3.e-8;
	    double G11,G22,G33,G12,G13,G23,D1,D2,D3;



	//       starting with the given initial circle (initial guess)

	    New.a = circleinix;
	    New.b = circleiniy;
	    New.r = circleinir;

	//       compute the root-mean-square error via function Sigma; see Utilities.cpp

	    New.s = Sigma(datan,dataX,dataY,New.a,New.b,New.r);

	//       initializing lambda, iteration counters, and the exit code

	    lambda = LambdaIni;
	    iter = inner = code = 0;

	NextIteration:

	    Old = New;
	    if (++iter > IterMAX) {code = 1;  goto enough;}

	//       computing moments

	    Mu=Mv=Muu=Mvv=Muv=Mr=0.;

	    for (i=0; i<datan; i++)
	    {
	        dx = dataX[i] - Old.a;
	        dy = dataY[i] - Old.b;
	        ri = sqrt(dx*dx + dy*dy);
	        u = dx/ri;
	        v = dy/ri;
	        Mu += u;
	        Mv += v;
	        Muu += u*u;
	        Mvv += v*v;
	        Muv += u*v;
	        Mr += ri;
	    }
	    Mu /= datan;
	    Mv /= datan;
	    Muu /= datan;
	    Mvv /= datan;
	    Muv /= datan;
	    Mr /= datan;

	//       computing matrices

	    F1 = Old.a + Old.r*Mu ;//- data.meanX;
	    F2 = Old.b + Old.r*Mv ;//- data.meanY;
	    F3 = Old.r - Mr;

	    Old.g = New.g = sqrt(F1*F1 + F2*F2 + F3*F3);

	try_again:

	    UUl = Muu + lambda;
	    VVl = Mvv + lambda;
	    Nl = 1 + lambda;

	//         Cholesly decomposition

	    G11 = sqrt(UUl);
	    G12 = Muv/G11;
	    G13 = Mu/G11;
	    G22 = sqrt(VVl - G12*G12);
	    G23 = (Mv - G12*G13)/G22;
	    G33 = sqrt(Nl - G13*G13 - G23*G23);

	    D1 = F1/G11;
	    D2 = (F2 - G12*D1)/G22;
	    D3 = (F3 - G13*D1 - G23*D2)/G33;

	    dR = D3/G33;
	    dY = (D2 - G23*dR)/G22;
	    dX = (D1 - G12*dY - G13*dR)/G11;

	    if ((absxx(dR)+absxx(dX)+absxx(dY))/(1+Old.r) < epsilon) goto enough;

	//       updating the parameters

	    New.a = Old.a - dX;
	    New.b = Old.b - dY;

	    if ((absxx(New.a)>ParLimit) || (absxx(New.b)>ParLimit)) {code = 3; goto enough;}


	    New.r = Old.r - dR;

	    if (New.r <= 0.)
	    {
	        lambda *= factorUp;
	        if (++inner > IterMAX) {code = 2;  goto enough;}
	        goto try_again;
	    }

	//       compute the root-mean-square error via function Sigma; see Utilities.cpp

	    New.s = Sigma(datan,dataX,dataY,New.a,New.b,New.r);

	//       check if improvement is gained

	    if (New.s < Old.s)    //   yes, improvement
	    {
	        lambda *= factorDown;
	        goto NextIteration;
	    }
	    else                       //   no improvement
	    {
	        if (++inner > IterMAX) {code = 2;  goto enough;}
	        lambda *= factorUp;
	        goto try_again;
	    }

	    //       exit

	enough:

	    Old.i = iter;    // total number of outer iterations (updating the parameters)
	    Old.j = inner;   // total number of inner iterations (adjusting lambda)

	    *circlex = Old.a;
	    *circley = Old.b;
	    *circler = Old.r;

	    return code;
}

