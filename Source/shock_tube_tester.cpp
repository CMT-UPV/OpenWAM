#include "Globales.h"
#include "BasicPipe.hpp"
#include "LaxWendroff.hpp"
#include <chrono>


int main()
{
	RowVector x(2);
	RowVector A(2);
	double dx;
	TBasicPipe pipe;
	TLaxWendroff method;
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
	method.Connect(&pipe);
	pipe.setPTU(p, T, u);
	auto start = chrono::steady_clock::now();
	for (int i = 0; i < 1000000; i++)
	{
		pipe.Solve();
	}
	auto end = chrono::steady_clock::now();
	auto diff = end - start;
	cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;

	return EXIT_SUCCESS;
}