#include "Globales.h"
#include "BasicPipe.hpp"
#include "LaxWendroff.hpp"


int main()
{
	RowVector x(2);
	RowVector A(2);
	double dx;
	TBasicPipe pipe;
	int n_nodes = 200;
	RowVector p(n_nodes);
	RowVector T(n_nodes);
	RowVector u(n_nodes);

	x(0) = 0.;
	x(1) = 1.;
	A.setConstant(1.);
	dx = x(1)/ (n_nodes - 1);

	p.setConstant(1E5);
	T.setConstant(1E5 / 287. / 1.);
	u.setConstant(0.);
	p.tail(n_nodes / 2).setConstant(1E4);
	T.tail(n_nodes / 2).setConstant(1E4 / 287. / 0.125);

	pipe.setGeometry(x, dx, A);
	pipe.setPTU(p, T, u);

	return EXIT_SUCCESS;
}