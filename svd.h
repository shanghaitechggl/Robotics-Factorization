#include <iostream>
#include <vector>
#include <time.h>

using namespace std;

const int MAX_ITER = 100000;      //��������������
const double eps = 0.0000001;      //�����������

double get_norm(double* x, int n)   //��sqrt(x[1]*x[1] + x[2]*x[2] +......+ x[n]*x[n])��                                     //������x��ģ��|x|��
{
	double r = 0;
	for (int i = 0; i < n; i++)
		r += x[i] * x[i];
	return sqrt(r);
}

double normalize(double* x, int n)   //����ĵ�����x��ģС��eps��Сֵʱ����ȥ��
{                        //��x��ģ����epsʱ������������x[i]/|x|��������x��һ��Ϊ��λ����e��
	double r = get_norm(x, n);
	if (r < eps)
		return 0;
	for (int i = 0; i < n; i++)
		x[i] /= r;
	return r;                 //��������x��ģ����ģ������̫С��
}



inline double product(double* a, double* b, int n)  //�������a������b���ڻ�����(a[1]*b[1] + a[2]*b[2] +......+ a[n]*b[n])��
{
	double r = 0;
	for (int i = 0; i < n; i++)
		r += a[i] * b[i];
	return r;
}


void orth(double* a, double* b, int n)    //��������|a|=1 ������aΪ��λ����e��������b�뵥λ
{                                         // ��������������
	double r = product(a, b, n);        //����b�뵥λ����e���ڻ�����Ϊ����b�ڵ�λ����e�ϵ�ͶӰֵ
	for (int i = 0; i < n; i++)
		b[i] -= r * a[i];                   //b'[i] = b[i] - (e, b)*b[i] ,�õ�������a��������                                         //��b' ����a, b')���ڻ�Ϊ0��
}


//S,U,V��size��Kָ��,K����Ҫ�ֽ��rank��0<K<=min(m,n)

bool svd(vector<vector<double> > A, int K, std::vector<std::vector<double> >& U, std::vector<double>& S, std::vector<std::vector<double> >& V)
{                                //����ΪA(M��N) ��A = U S (V^T)�� U(M��K)  S(K��K)  V(N��K)
	int M = A.size();
	int N = A[0].size();
	U.clear();
	V.clear();
	S.clear();

	S.resize(K, 0);      //SΪ�ԽǾ���ֻ�豣��Խ����ϵ�����ֵ���ɣ���Sȡ�������棬��Լ�ռ䣻                         //S����K��Ԫ�ؼ���   

	U.resize(K);              // U����ΪK����������ÿ����������M��Ԫ�أ�
	for (int i = 0; i < K; i++)
	{
		U[i].resize(M, 0);
	}

	V.resize(K);            // V����ҲΪK����������ÿ����������N��Ԫ�أ�
	for (int i = 0; i < K; i++)
	{
		V[i].resize(N, 0);
	}

	srand(time(0));            //��һ�����������,

	double* left_vector = new double[M];          //��̬�����ڴ�,����left_vector��                                                           //next_left_vector�������ڵ�������
	double* next_left_vector = new double[M];
	double* right_vector = new double[N];
	double* next_right_vector = new double[N];

	while (1)
	{
		for (int i = 0; i < M; i++)
			left_vector[i] = (float)rand() / RAND_MAX;    //�������һ������vector����������                                                       //����M��Ԫ�أ����������������� 
		if (normalize(left_vector, M) > eps)               //��������ģ������epsʱ�����ɽ�����                                                        //����һ��ģ�����ʵĵ�������
			break;
	}

	int col = 0;
	for (int col = 0; col < K; col++)    //���м��������������,����
	{
		double diff = 1;
		double r = -1;

		for (int iter = 0; diff >= eps && iter < MAX_ITER; iter++)     //������iter�ĵ�������Ϊ10000��
		{
			memset(next_left_vector, 0, sizeof(double) * M);
			memset(next_right_vector, 0, sizeof(double) * N);  //�����ڴ��������������ҵ�������


			for (int i = 0; i < M; i++)
				for (int j = 0; j < N; j++)
					next_right_vector[j] += left_vector[i] * A[i][j];   //������(1��M)��A(M��N)���󣬵õ��ҵ���������_next(1��N)

			r = normalize(next_right_vector, N);                 //��λ��������_next

			if (r < eps) break;                                  //����_next ģ��̫С�����˳��ڲ�ѭ��
			for (int i = 0; i < col; i++)
				orth(&V[i][0], next_right_vector, N);              //���Ҿ���V�������������õ���'_next
			normalize(next_right_vector, N);                      //��λ����'_next



			for (int i = 0; i < M; i++)
				for (int j = 0; j < N; j++)
					next_left_vector[i] += next_right_vector[j] * A[i][j]; //����A(M��N)��(��'_next)^T(N��1)�������õ������������_next(1��M)

			r = normalize(next_left_vector, M);                       //��λ��������_next

			if (r < eps) break;                                       //����_next ģ��̫С�����˳��ڲ�ѭ��
			for (int i = 0; i < col; i++)
				orth(&U[i][0], next_left_vector, M);                 //�������U�������������õ���'_next
			normalize(next_left_vector, M);                         //��λ����'_next

			diff = 0;

			for (int i = 0; i < M; i++)
			{
				double d = next_left_vector[i] - left_vector[i];      //����ǰ����������������2������ƽ������������������ƽ��
				diff += d * d;
			}

			memcpy(left_vector, next_left_vector, sizeof(double) * M);     //��������������ֵ��ʹ�õ���������ǰ����
			memcpy(right_vector, next_right_vector, sizeof(double) * N);
		}

		if (r >= eps)
		{
			S[col] = r;                                                  //��������ģ��������Сֵeps����ȡΪ����A������ֵ

			memcpy((char*)&U[col][0], left_vector, sizeof(double) * M);   //���п�����������������������U��

			memcpy((char*)&V[col][0], right_vector, sizeof(double) * N);  //���п����ҵ������������������V��

		}
		else break;
	}

	delete[] next_left_vector;
	delete[] next_right_vector;
	delete[] left_vector;
	delete[] right_vector;

	return true;
}

