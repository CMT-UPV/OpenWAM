#include "BasicPipe.hpp"

TBasicPipe::TBasicPipe()
{
	FXref = 1;
	FDeltaTime = 1;
	FIsIntegrated = true;
}


double TBasicPipe::Colebrook(double rug, double d, double Re) const
{
	double temp = 0., temp2 = 0.;
	double pow_approx = 0.;
	double f = 0.;

	if (Re > 2000) {
		if (Re > 130000.) {
			temp = rug / (3700.0 * d) + 5.74 / pow(Re, 0.9);
		} else if (Re > 50000.) {
			// pow(Re, 0.9) approximation between Re = 50000 and Re = 130000
			// with less than 0.015 % of error.
			pow_approx = 7.04596786e-13 * pow3(Re) - 3.55878012e-07 * pow2(Re)
					+ 3.34536053e-01 * Re + 1.02365893e+03;
			temp = rug / (3700.0 * d) + 5.74 / pow_approx;
		} else if (Re > 20000.) {
			// pow(Re, 0.9) approximation between Re = 20000 and Re = 50000
			// with less than 0.015 % of error.
			pow_approx = 5.09028031e-12 * pow3(Re) - 1.00134262e-06 * pow2(Re)
					+ 3.67476796e-01 * Re + 4.40171892e+02;
			temp = rug / (3700.0 * d) + 5.74 / pow_approx;
		} else if (Re > 10000.) {
			// pow(Re, 0.9) approximation between Re = 10000 and Re = 20000
			// with less than 0.015 % of error.
			pow_approx = 2.92727184e-11 * pow3(Re) - 2.48693415e-06 * pow2(Re)
					+ 3.98901290e-01 * Re + 2.11628209e+02;
			temp = rug / (3700.0 * d) + 5.74 / pow_approx;
		} else if (Re > 4000.) {
			// pow(Re, 0.9) approximation between Re = 4000 and Re = 10000
			// with less than 0.015 % of error.
			pow_approx = 1.49478492e-10 * pow3(Re) - 5.88098006e-06 * pow2(Re)
					+ 4.31645206e-01 * Re + 1.03406848e+02;
			temp = rug / (3700.0 * d) + 5.74 / pow_approx;
		} else {
			temp = rug / (3700.0 * d) + 0.00328895476345;
		}
		if (temp > 0.042) {
			temp2 = pow2(log10(temp));
		} else if (temp > 0.019) {
			// pow2(log10(temp)) approximation between temp = 0.019 and
			// temp = 0.042 with less than 0.02 % of error.
			temp2 = 6.76495011e+05 * pow4(temp) - 1.07370937e+05 * pow3(temp)
					+ 6.95435632e+03 * pow2(temp) - 2.44427511e+02 * temp
					+ 5.74410691e+00;
		} else if (temp > 0.008) {
			// pow2(log10(temp)) approximation between temp = 0.008 and
			// temp = 0.019 with less than 0.02 % of error.
			temp2 = 2.08516955e+07 * pow4(temp) - 1.46552875e+06 * pow3(temp)
					+ 4.20051994e+04 * pow2(temp) - 6.58067557e+02 * temp
					+ 7.63733377e+00;
		} else if (temp > 0.00329) {
			// pow2(log10(temp)) approximation between temp = 0.00329 and
			// temp = 0.008 with less than 0.02 % of error.
			temp2 = 7.84991530e+08 * pow4(temp) - 2.30952512e+07 * pow3(temp)
					+ 2.77321091e+05 * pow2(temp) - 1.83296385e+03 * temp
					+ 9.92239603e+00;
		} else if (temp > 0.0013) {
			// pow2(log10(temp)) approximation between temp = 0.0013 and
			// temp = 0.00329 with less than 0.02 % of error.
			temp2 = 3.29050277e+10 * pow4(temp) - 3.93727194e+08 * pow3(temp)
					+ 1.92219159e+06 * pow2(temp) - 5.18844300e+03 * temp
					+ 1.25952908e+01;
		} else if (temp > 0.0005) {
			// pow2(log10(temp)) approximation between temp = 0.0005 and
			// temp = 0.0013 with less than 0.02 % of error.
			temp2 = 1.57146781e+12 * pow4(temp) - 7.37699295e+09 * pow3(temp)
					+ 1.41288296e+07 * pow2(temp) - 1.50182432e+04 * temp
					+ 1.56956366e+01;
		} else if (temp > 0.00019) {
			// pow2(log10(temp)) approximation between temp = 0.00019 and
			// temp = 0.0005 with less than 0.02 % of error.
			temp2 = 8.09890844e+13 * pow4(temp) - 1.45825030e+11 * pow3(temp)
					+ 1.07169184e+08 * pow2(temp) - 4.38671762e+04 * temp
					+ 1.92059658e+01;
		} else if (temp > 0.00007) {
			// pow2(log10(temp)) approximation between temp = 0.00007 and
			// temp = 0.00019 with less than 0.02 % of error.
			temp2 = 4.47104917e+15 * pow4(temp) - 3.03334424e+12 * pow3(temp)
					+ 8.39454836e+08 * pow2(temp) - 1.29630353e+05 * temp
					+ 2.31540981e+01;
		} else {
			temp2 = pow2(log10(temp));
		}
		f = 0.0625 / temp2;
	} else if (Re > 1) {
		f = 32. / Re;
	} else {
		f = 32.;
	}

	return f;
}


double TBasicPipe::getMaxTimeStep()
{
	return FMethod->getMaxTimeStep();
}


void TBasicPipe::setBCs(TCondicionContorno* leftBC, TCondicionContorno* rightBC)
{
	FLeftBC = leftBC;
	FRightBC = rightBC;
}


void TBasicPipe::setDeltaTime(double dt)
{
	FDeltaTime = dt;
}


void TBasicPipe::setGeometry(const RowVector& x, double dx, const RowVector& A)
{
	double n_nodes = round(x.tail(1)(0) / dx) + 1;
	if (n_nodes < 3)
	{
		n_nodes = 3;
	}
	dx = x.tail(1)(0) / n_nodes;
	Fdx.setConstant(n_nodes - 1, dx);
	Fx.setLinSpaced(n_nodes, 0, x.tail(1)(0));
	FMx = (Fx.head(n_nodes - 1) + Fx.tail(n_nodes - 1)) / 2.;
	FArea = linear_interp(x, A, Fx);
	FMArea = (FArea.head(n_nodes - 1) + FArea.tail(n_nodes - 1)) / 2.;
	FVolume = FMArea * dx;
	FDerLinArea = (FArea.tail(n_nodes - 1) - FArea.head(n_nodes - 1)) / Fdx;
	
	FU0.setZero(3, n_nodes);
}


void TBasicPipe::setPTU(double p, double T, double u)
{
	FMethod->setPTU(p, T, u);
}


void TBasicPipe::setPTU(const RowVector& p, const RowVector& T,
	const RowVector& u)
{
	FMethod->setPTU(p, T, u);
}


double TBasicPipe::getCurrentTime() const
{
	return FCurrentTime;
}


RowVector TBasicPipe::getPressure() const
{
	return Fpressure;
}


double TBasicPipe::getPressure(unsigned int i) const
{
	if (i < Fpressure.size())
	{
		return Fpressure(i);
	}
	else
	{
		return Fpressure.tail(1)(0);
	}
}


double TBasicPipe::getPressure(double x) const
{
	return linear_interp(Fx, Fpressure, x);
}


RowVector TBasicPipe::getSpeed() const
{
	return Fspeed;
}


double TBasicPipe::getSpeed(unsigned int i) const
{
	if (i < Fspeed.size())
	{
		return Fspeed(i);
	}
	else
	{
		return Fspeed.tail(1)(0);
	}
}


double TBasicPipe::getSpeed(double x) const
{
	return linear_interp(Fx, Fspeed, x);
}



RowVector TBasicPipe::getSpeedOfSound() const
{
	return Fa;
}


double TBasicPipe::getSpeedOfSound(unsigned int i) const
{
	if (i < Fa.size())
	{
		return Fa(i);
	}
	else
	{
		return Fa.tail(1)(0);
	}
}


double TBasicPipe::getSpeedOfSound(double x) const
{
	return linear_interp(Fx, Fa, x);
}


RowVector TBasicPipe::getTemperature() const
{
	return Ftemperature;
}


double TBasicPipe::getTemperature(unsigned int i) const
{
	if (i < Ftemperature.size())
	{
		return Ftemperature(i);
	}
	else
	{
		return Ftemperature.tail(1)(0);
	}
}


double TBasicPipe::getTemperature(double x) const
{
	return linear_interp(Fx, Ftemperature, x);
}


void TBasicPipe::Solve()
{
	FMethod->Solve();
}