/* --------------------------------------------------------------------------------*\
==========================|
\\   /\ /\   // O pen     | OpenWAM: The Open Source 1D Gas-Dynamic Code
\\ |  X  | //  W ave     |
\\ \/_\/ //   A ction   | CMT-Motores Termicos / Universidad Politecnica Valencia
\\/   \//    M odel    |
----------------------------------------------------------------------------------
License

This file is part of OpenWAM.

OpenWAM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenWAM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OpenWAM.  If not, see <http://www.gnu.org/licenses/>.


\*-------------------------------------------------------------------------------- */

// ---------------------------------------------------------------------------

#ifndef Math_wamH
#define Math_wamH
// ---------------------------------------------------------------------------

// #include "nr3.h"
#include <cstdlib>
#include <vector>
#include <cmath>
#include <limits>
#include <iostream>

using namespace std;

typedef unsigned int Uint;
typedef std::vector<double>dVector; // !< Definition of vector double
typedef std::vector<std::vector<double> >dMatrix; // !< Definition of a 2-dimensional matrix double
typedef std::vector<int>iVector; // !< Definition of vector integer
typedef std::vector<std::vector<int> >iMatrix; // !< Definition of a 2-dimensional matrix integer
typedef std::vector<bool>bVector; // !< Definition of vector integer
typedef std::vector<std::vector<bool> >bMatrix; // !< Definition of a 2-dimensional matrix integer

inline double Interpola(double vizq, double vder, double axid, double xif) {
	return vizq + ((vder - vizq) / axid) * xif;
}


template<class T>
T pow2(T x) {
	return x*x;
}


template<class T>
T pow3(T x) {
	return x*x*x;
}


template<class T>
T pow4(T x) {
	return x*x*x*x;
}


template<class T, class U>
T poww(T x, U y) {
	return abs(x) > std::numeric_limits<T>::epsilon() ? pow(x, y) : 0;
}


template<class T>
T sqrtw(T x) {
	return abs(x) > std::numeric_limits<T>::epsilon() ? sqrt(x) : 0;
}


template<class T>
bool DoubEqZero(T x) {
	return abs(x) > std::numeric_limits<T>::epsilon() ? false : true;
}

inline double QuadraticEqP(double A, double B, double C) {
	return(-B + sqrt(B * B - 4 * A * C)) / 2 / A;
}

inline double QuadraticEqN(double A, double B, double C) {
	return(-B - sqrt(B * B - 4 * A * C)) / 2 / A;
}

template<class T>
inline T SQR(const T a) {
	return a*a;
}

template<class T>
inline const T &Max(const T &a, const T &b) {
	return b > a ? (b) : (a);
}

inline float Max(const double &a, const float &b) {
	return b > a ? (b) : float(a);
}

inline float Max(const float &a, const double &b) {
	return b > a ? float(b) : (a);
}

template<class T>
inline const T &Min(const T &a, const T &b) {
	return b < a ? (b) : (a);
}

inline float Min(const double &a, const float &b) {
	return b < a ? (b) : float(a);
}

inline float Min(const float &a, const double &b) {
	return b < a ? float(b) : (a);
}

template<class T>
inline T Sign(const T &a, const T &b) {
	return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);
}

inline float Sign(const float &a, const double &b) {
	return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);
}

inline float Sign(const double &a, const float &b) {
	return(float)(b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a));
}

template<class T>
inline void Swap(T &a, T &b) {
	T dum = a;
	a = b;
	b = dum;
}

template<class T>
T MaxComponent(std::vector<T>x) {
	T max = x[0];
	for (Uint i = 1; i < x.size(); i++) {
		if (x[i] > max)
			max = x[i];
	}
	return max;
}

template<class T>
T MinComponent(std::vector<T>x) {
	T min = x[0];
	for (Uint i = 1; i < x.size(); i++) {
		if (x[i] < min)
			min = x[i];
	}
	return min;
}
/* double MaxComponent(VecDoub Vect)
{
double Max=Vect[0];
for(int i=1;i<Vect.size();i++){
if(Vect[i]>Max){
Max=Vect[i];
}
}
return Max;
}

double MinComponent(VecDoub Vect)
{
double Min=Vect[0];
for(int i=1;i<Vect.size();i++){
if(Vect[i]<Min){
Min=Vect[i];
}
}
return Min;
} */

struct stPolar {
	double Ang;
	double Mod;

	stPolar() {
	}

	stPolar(double X, double Y) {
		Mod = sqrt(X * X + Y * Y);
		Ang = atan(Y / X);
	}
	void operator()(double X, double Y) {
		Mod = sqrt(X * X + Y * Y);
		Ang = atan(Y / X);
	}
};

struct stRectan {
	double X;
	double Y;

	stRectan() {
	}

	stRectan(double Ang, double Mod) {
		X = Mod * cos(Ang);
		Y = Mod * sin(Ang);
	};
	void operator()(double Ang, double Mod) {
		X = Mod * cos(Ang);
		Y = Mod * sin(Ang);
	};
};

struct Base_interp {
	int n, mm, jsav, cor, dj;
	const double *xx, *yy;

	Base_interp() : n(0), mm(0), jsav(0), cor(0), xx(), yy() {
	};

	Base_interp(dVector &x, const double *y, int m) : n(x.size()), mm(m), jsav(0), cor(0),
	xx(&x[0]), yy(y) {
		dj = Max(1, (int)pow((double)n, 0.25));
	}

	double interp(double x) {
		int jlo = cor ? hunt(x) : locate(x);
		return rawinterp(jlo, x);
	}
	int locate(const double x);
	int hunt(const double x);

	double virtual rawinterp(int jlo, double x) = 0;
};

inline int Base_interp::locate(const double x) {
	int ju, jm, jl;
	if (n < 2 || mm < 2 || mm > n)
		throw("locate size error");
	bool ascnd = (xx[n - 1] >= xx[0]);
	jl = 0;
	ju = n - 1;
	while (ju - jl > 1) {
		jm = (ju + jl) >> 1;
		if ((x >= xx[jm]) == ascnd) {
			jl = jm;
		}
		else {
			ju = jm;
		}
	}
	cor = abs(jl - jsav) > dj ? 0 : 1;
	jsav = jl;
	return Max(0, Min(n - mm, jl - ((mm - 2) >> 1)));

}

inline int Base_interp::hunt(const double x) {
	int jl = jsav, jm, ju, inc = 1;
	if (n < 2 || mm < 2 || mm > n)
		throw("locate size error");
	bool ascnd = (xx[n - 1] >= xx[0]);
	if (jl < 0 || jl > n - 1) {
		jl = 0;
		ju = n - 1;
	}
	else {
		if ((x >= xx[jl]) == ascnd) {
			for (; ; ) {
				ju = jl + inc;
				if (ju >= n) {
					ju = n - 1;
					break;
				}
				else if ((x < xx[ju]) == ascnd)
					break;
				else {
					jl = ju;
					inc += inc;
				}
			}
		}
		else {
			ju = jl;
			for (; ; ) {
				jl = jl - inc;
				if (jl <= 0) {
					jl = 0;
					break;
				}
				else if ((x >= xx[jl]) == ascnd)
					break;
				else {
					ju = jl;
					inc += inc;
				}
			}
		}
	}
	while (ju - jl > 1) {
		jm = (ju + jl) >> 1;
		if ((x >= xx[jm]) == ascnd) {
			jl = jm;
		}
		else {
			ju = jm;
		}
	}
	cor = abs(jl - jsav) > dj ? 0 : 1;
	jsav = jl;
	return Max(0, Min(n - mm, jl - ((mm - 2) >> 1)));
}

struct Linear_interp : Base_interp {
	Linear_interp() : Base_interp() {
	}

	Linear_interp(dVector &xv, dVector &yv) : Base_interp(xv, &yv[0], 2) {
	}

	void operator()(dVector & xv, dVector & yv) {
		xx = &xv[0];
		yy = &yv[0];
		n = xv.size();
		mm = 2;
		jsav = 0;
		cor = 0;
		dj = Max(1, (int)pow((double)n, 0.25));
	}

	double rawinterp(int j, double x) {
		if (xx[j] == xx[j + 1])
			return yy[j];
		else
			return yy[j] + ((x - xx[j]) / (xx[j + 1] - xx[j])) * (yy[j + 1] - yy[j]);
	}
};

struct Hermite_interp : Base_interp {
	dVector y2;

	Hermite_interp() : Base_interp(), y2() {
	};

	Hermite_interp(dVector &xv, dVector &yv) : Base_interp(xv, &yv[0], 2), y2(xv.size()) {
		sety2(&xv[0], &yv[0]);
	};

	void operator()(dVector & xv, dVector & yv) {
		xx = &xv[0];
		yy = &yv[0];
		n = xv.size();
		mm = 2;
		jsav = 0;
		cor = 0;
		y2.resize(n);
		dj = Max(1, (int)pow((double)n, 0.25));
		sety2(&xv[0], &yv[0]);
	}

	void sety2(const double *xv, const double *yv);
	double rawinterp(int j, double x);

};

inline void Hermite_interp::sety2(const double *xv, const double *yv) {
	double DeltaK, AlphaK, BetaK, TauK;

	for (int i = 1; i < n - 1; ++i) {
		y2[i] = (yv[i] - yv[i - 1]) / 2. / (xv[i] - xv[i - 1]) + (yv[i + 1] - yv[i]) / 2. /
			(xv[i + 1] - xv[i]);
	}
	y2[0] = (yv[1] - yv[0]) / (xv[1] - xv[0]);
	y2[n - 1] = (yv[n - 1] - yv[n - 2]) / (xv[n - 1] - xv[n - 2]);

	for (int i = 0; i < n - 1; i++) {
		DeltaK = (yv[i + 1] - yv[i]) / (xv[i + 1] - xv[i]);
		if (DeltaK == 0) {
			y2[i] = 0;
			y2[i + 1] = 0;
		}
		else {
			AlphaK = y2[i] / DeltaK;
			BetaK = y2[i + 1] / DeltaK;
			if (BetaK * BetaK + AlphaK * AlphaK > 9) {
				TauK = 3 / sqrt(BetaK * BetaK + AlphaK * AlphaK);
				y2[i] = TauK * AlphaK * DeltaK;
				y2[i + 1] = TauK * BetaK * DeltaK;
			}
		}
	}
}

inline double Hermite_interp::rawinterp(int j, double x) {
	double ret_val, h00, h10, h01, h11, t2, t3, t, h;
	// int k=0;

	if (x <= xx[j]) {
		ret_val = yy[j];
	}
	else if (x >= xx[j + 1]) {
		ret_val = yy[j + 1];
	}
	else {
		h = (xx[j + 1] - xx[j]);
		t = (x - xx[j]) / h;
		t2 = t * t;
		t3 = t2 * t;
		h00 = 2 * t3 - 3 * t2 + 1;
		h10 = t3 - 2 * t2 + t;
		h01 = -2 * t3 + 3 * t2;
		h11 = t3 - t2;
		ret_val = h00 * yy[j] + h * h10 * y2[j] + h01 * yy[j + 1] + h * h11 * y2[j + 1];
	}
	return ret_val;
}

struct Step_interp : Base_interp {
	Step_interp() : Base_interp() {
	}

	Step_interp(dVector &xv, dVector &yv) : Base_interp(xv, &yv[0], 2) {
	}

	void operator()(dVector & xv, dVector & yv) {
		xx = &xv[0];
		yy = &yv[0];
		n = xv.size();
		mm = 2;
		jsav = 0;
		cor = 0;
		dj = Max(1, (int)pow((double)n, 0.25));
	}

	double rawinterp(int j, double x) {
		if (xx[j] == xx[j + 1])
			return yy[j];
		else
			return yy[j];
	}
};

template<class T>
double FindRoot(T & func, const double x1, const double x2) {
	return zbrent(func, x1, x2, 1e-10);
}

template<class T>
bool zbrac(T & func, double & x1, double & x2) {
	const int NTRY = 200;
	const double FACTOR = 0.1;
	if (x1 == x2)
		throw("Bad initial range in zbrac");
	double f1 = func(x1);
	double f2 = func(x2);
	for (int j = 0; j < NTRY; j++) {
		if (f1 * f2 < 0.0)
			return true;
		if (abs(f1) < abs(f2))
			f1 = func(x1 += FACTOR * (x1 - x2));
		else
			f2 = func(x2 += FACTOR * (x2 - x1));
	}
}

template<class T>
bool zbrac2(T & func, double & x1, double & x2, const double & min, const double & max) {
	const int NTRY = 200;
	// const double FACTOR=0.1;
	if (x1 == x2)
		throw("Bad initial range in zbrac");
	double x1lim = min;
	double x2lim = max;
	if (x1 > x2) {
		x1lim = max;
		x2lim = min;
	}
	double f1 = func(x1);
	double f2 = func(x2);
	for (int j = 0; j < NTRY; j++) {
		if (f1 * f2 < 0.0)
			return true;
		if (abs(f1) < abs(f2))
			f1 = func(x1 = (0.9 * x1 + 0.1 * x1lim));
		else
			f2 = func(x2 = (0.9 * x2 + 0.1 * x2lim));
	}
	std::cout << "Do not exist solution" << std::endl;
	std::cout << "x1 :" << x1 << " f1: " << f1 << " x1max: " << x1lim << std::endl;
	std::cout << "x2 :" << x2 << " f2: " << f2 << " x2max: " << x2lim << std::endl;

	return false;

}

template<class T>
void zbrak(T & fx, const double x1, const double x2, const int n, dVector & xb1, dVector & xb2,
	int & nroot) {
	int nb = 20;
	xb1.resize(nb);
	xb2.resize(nb);
	nroot = 0;
	double dx = (x2 - x1) / n;
	double x = x1;
	double fp = fx(x += dx);
	for (int i = 0; i < n; i++) {
		double fc = fx(x += dx);
		if (fc * fp <= 0.0) {
			xb1[nroot] = x - dx;
			xb2[nroot++] = x;
			if (nroot == nb) {
				dVector tempvec1(xb1), tempvec2(xb2);
				xb1.resize(2 * nb);
				xb2.resize(2 * nb);
				for (int j = 0; j < nb; j++) {
					xb1[j] = tempvec1[j];
					xb2[j] = tempvec2[j];
				}
				nb *= 2;
			}
		}
		fp = fc;
	}
}

template<class T>
double zbrent(T & func, const double x1, const double x2, const double tol) {
	const int ITMAX = 100;
	const double EPS = std::numeric_limits<double>::epsilon();
	double a = x1, b = x2, c = x2, d, e, fa = func(a), fb = func(b), fc, p, q, r, s, tol1, xm;
	if ((fa > 0.0 && fb > 0.0) || (fa < 0.0 && fb < 0.0)) {
		if (abs(fa) < abs(fb)) {
			fa = func(a);
			return a;
		} /* Condicion original if((abs(fa) < abs(fb)) && abs(fa) < tol) */
		else if (abs(fa) > abs(fb)) {
			fb = func(b);
			return b;
		} /* Original if((abs(fa) > abs(fb)) && abs(fb) < tol) */
		// return 0;
		/* throw("Root must be bracketed in zbrent"); */
	}

	fc = fb;
	for (int iter = 0; iter < ITMAX; iter++) {
		if ((fb > 0.0 && fc > 0.0) || (fb < 0.0 && fc < 0.0)) {
			c = a;
			fc = fa;
			e = d = b - a;
		}
		if (abs(fc) < abs(fb)) {
			a = b;
			b = c;
			c = a;
			fa = fb;
			fb = fc;
			fc = fa;
		}
		tol1 = 2.0 * EPS * abs(b) + 0.5 * tol;
		xm = 0.5 * (c - b);
		if (abs(xm) <= tol1 || fb == 0.0) {
			/* std::cout << iter << std::endl; */ return b;
		}
		if (abs(e) >= tol1 && abs(fa) > abs(fb)) {
			s = fb / fa;
			if (a == c) {
				p = 2.0 * xm * s;
				q = 1.0 - s;
			}
			else {
				q = fa / fc;
				r = fb / fc;
				p = s * (2.0 * xm * q * (q - r) - (b - a) * (r - 1.0));
				q = (q - 1.0) * (r - 1.0) * (s - 1.0);
			}
			if (p > 0.0)
				q = -q;
			p = abs(p);
			double min1 = 3.0 * xm * q - abs(tol1 * q);
			double min2 = abs(e * q);
			if (2.0 * p < (min1 < min2 ? min1 : min2)) {
				e = d;
				d = p / q;
			}
			else {
				d = xm;
				e = d;
			}

		}
		else {
			d = xm;
			e = d;
		}
		a = b;
		fa = fb;
		if (abs(d) > tol1)
			b += d;
		else
			b += Sign(tol1, xm);
		fb = func(b);

	}

	return b;
}

template<class T>
double rtbis(T & func, const double x1, const double x2, const double xacc) {
	const int JMAX = 50;

	double dx, xmid, rtb;

	double f = func(x1);
	double fmid = func(x2);
	if (f * fmid >= 0.0) {
		if ((abs(f) < abs(fmid)) && abs(f) < xacc) {
			return x1;
		}
		else if ((abs(f) > abs(fmid)) && abs(fmid) < xacc)
			return x2;
		throw("Root must be bracketed for bisection in rtbis");
	}
	rtb = f < 0.0 ? (dx = x2 - x1, x1) : (dx = x1 - x2, x2);
	for (int j = 0; j < JMAX; j++) {
		fmid = func(xmid = rtb + (dx *= 0.5));
		if (fmid <= 0.0)
			rtb = xmid;
		if (abs(dx) < xacc || fmid == 0.0) {
			/* std::cout << j << std::endl; */ return rtb;
		}
	}
	throw("Too many bisections in rtbis");
}

template<class T>
double rtflsp(T & func, const double x1, const double x2, const double xacc) {
	const int MAXIT = 1000;

	double xl, xh, del;

	double fl = func(x1);
	double fh = func(x2);
	if (fl * fh > 0.0)
		throw("Root must be bracketed in rtflsp");
	if (fl < 0.0) {
		xl = x1;
		xh = x2;
	}
	else {
		xl = x2;
		xh = x1;
		Swap(fl, fh);
	}
	double dx = xh - xl;
	for (int j = 0; j < MAXIT; j++) {
		double rtf = xl + dx * fl / (fl - fh);
		double f = func(rtf);
		if (f < 0.0) {
			del = xl - rtf;
			xl = rtf;
			fl = f;
		}
		else {
			del = xh - rtf;
			xh = rtf;
			fh = f;
		}
		dx = xh - xl;
		if (abs(del) < xacc || f == 0.0) {
			/* std::cout << j << std::endl; */ return rtf;
		}
	}
	throw("Maximum number of iterations exceeded in rtflsp");
}

template<class T>
double rtsec(T & func, const double x1, const double x2, const double xacc) {
	const int MAXIT = 100;

	double xl, rts;

	double fl = func(x1);
	double f = func(x2);
	if (abs(fl) < abs(f)) {
		rts = x1;
		xl = x2;
		Swap(fl, f);
	}
	else {
		xl = x1;
		rts = x2;
	}
	for (int j = 0; j < MAXIT; j++) {
		double dx = (xl - rts) * f / (f - fl);
		xl = rts;
		fl = f;
		rts += dx;
		f = func(rts);
		if (abs(dx) < xacc || f == 0.0) {
			/* std::cout << j << std::endl; */ return rts;
		}
	}
	throw("Maximum number of iterations exceede in rtsec");
}

template<class T>
double zriddr(T & func, const double x1, const double x2, const double xacc) {
	const int MAXIT = 100;
	double fl = func(x1);
	double fh = func(x2);
	if ((fl > 0.0 && fh < 0.0) || (fl < 0.0 && fh > 0.0)) {
		double xl = x1;
		double xh = x2;
		double ans = -9.99e99;
		for (int j = 0; j < MAXIT; j++) {
			double xm = 0.5 * (xl + xh);
			double fm = func(xm);
			double s = sqrt(fm * fm - fl * fh);
			if (s == 0.0) {
				/* std::cout << j << std::endl; */ return ans;
			}
			double xnew = xm + (xm - xl) * ((fl >= fh ? 1.0 : -1.0) * fm / s);
			if (abs((double)(xnew - ans <= xacc))) {
				/* std::cout << j << std::endl; */ return ans;
			}
			ans = xnew;
			double fnew = func(ans);
			if (fnew == 0) {
				/* std::cout << j << std::endl; */ return ans;
			}
			if (Sign(fm, fnew) != fm) {
				xl = xm;
				fl = fm;
				xh = ans;
				fh = fnew;
			}
			else if (Sign(fl, fnew) != fl) {
				xh = ans;
				fh = fnew;
			}
			else if (Sign(fh, fnew) != fh) {
				xl = ans;
				fl = fnew;
			}
			else
				throw("never get here.");
			if (abs(xh - xl) <= xacc) {
				/* std::cout << j << std::endl; */ return ans;
			}
		}
		throw("zriddr exceed maximum iterations");
	}
	else {
		if (fl == 0.0)
			return x1;
		if (fh == 0.0)
			return x2;
		throw("root must be bracketed in zriddr.");
	}
}

template<class T>
double rtnewt(T & funcd, const double x1, const double x2, const double xacc) {
	const int JMAX = 20.;
	double rtn = 0.5 * (x1 + x2);
	for (int j = 0; j < JMAX; j++) {
		double f = funcd(rtn);
		double df = funcd.df(rtn);
		double dx = f / df;
		rtn -= dx;
		if ((x1 - rtn) * (rtn - x2) < 0.0)
			throw("Jumped out of brackets in rtnewt");
		if (abs(dx) < xacc)
			return rtn;
	}
	throw("Maximum number of iterations exceede in rtnewt");
}

template<class T>
double rtsafe(T & funcd, const double x1, const double x2, const double xacc) {
	const int MAXIT = 100;

	double xh, xl;

	double fl = funcd(x1);
	double fh = funcd(x2);
	if ((fl > 0.0 && fh > 0.0) || (fl < 0.0 && fh < 0.0)) {
		throw("Root mus be bracketed in rt safe");
	}
	if (fl == 0.0)
		return x1;
	if (fh == 0.0)
		return x2;
	if (fl < 0.0) {
		xl = x1;
		xh = x2;
	}
	else {
		xh = x1;
		xl = x2;
	}
	double rts = 0.5 * (x1 + x2);
	double dxold = abs(x2 - x1);
	double dx = dxold;
	double f = funcd(rts);
	double df = funcd.df(rts);
	for (int j = 0; j < MAXIT; j++) {
		if ((((rts - xh) * df - f) * ((rts - xl) * df - f) > 0.0) ||
			(abs(2.0 * f) > abs(dxold * df))) {
			dxold = dx;
			dx = 0.5 * (xh - xl);
			rts = xl + dx;
			if (xl == rts)
				return rts;
		}
		else {
			dxold = dx;
			dx = f / df;
			double temp = rts;
			rts -= dx;
			if (temp == rts)
				return rts;
		}
		if (abs(dx) < xacc)
			return rts;
		double f = funcd(rts);
		double df = funcd.df(rts);
		if (f < 0.0)
			xl = rts;
		else
			xh = rts;
	}
	throw("Maximum number of iterations exceede in rtsafe");
}

struct LUdcmp {
	int n;
	dMatrix lu;
	iVector indx;
	double d;

	LUdcmp(dMatrix &a);
	void solve(dVector &b, dVector &x);
	void inverse(dMatrix &ainv);

	dMatrix &aref;
};

inline LUdcmp::LUdcmp(dMatrix &a) : n(a.size()), lu(a), aref(a), indx(n) {

	const double TINY = 1.0e-40;
	int i, imax, j, k;
	double big, temp;
	dVector vv(n);
	d = 1.0;
	for (i = 0; i < n; i++) {
		big = 0.0;
		for (j = 0; j < n; j++)
			if ((temp = fabs(lu[i][j])) > big)
				big = temp;
		if (big == 0.0)
			throw("Singular matrix in LUdcmp");
		vv[i] = 1.0 / big;
	}
	for (k = 0; k < n; k++) {
		big = 0.0;
		for (i = k; i < n; i++) {
			temp = vv[i] * fabs(lu[i][k]);
			if (temp > big) {
				big = temp;
				imax = i;
			}
		}
		if (k != imax) {
			for (j = 0; j < n; j++) {
				temp = lu[imax][j];
				lu[imax][j] = lu[k][j];
				lu[k][j] = temp;
			}
			d = -d;
			vv[imax] = vv[k];
		}
		indx[k] = imax;
		if (lu[k][k] == 0.0)
			lu[k][k] = TINY;
		for (i = k + 1; i < n; i++) {
			temp = lu[i][k] /= lu[k][k];
			for (j = k + 1; j < n; j++)
				lu[i][j] -= temp * lu[k][j];

		}
	}
}

inline void LUdcmp::solve(dVector &b, dVector &x) {
	int i, ii = 0, ip, j;
	double sum;
	if (b.size() != n || x.size() != n)
		throw("LUdcmp::solve bad sizes");
	for (i = 0; i < n; i++)
		x[i] = b[i];
	for (i = 0; i < n; i++) {
		ip = indx[i];
		sum = x[ip];
		x[ip] = x[i];
		if (ii != 0)
			for (j = ii - 1; j < i; j++)
				sum -= lu[i][j] * x[j];
		else if (sum != 0.0)
			ii = i + 1;
		x[i] = sum;

	}
	for (i = n - 1; i >= 0; i--) {
		sum = x[i];
		for (j = i + 1; j < n; j++)
			sum -= lu[i][j] * x[j];
		x[i] = sum / lu[i][i];
	}

}



#endif

