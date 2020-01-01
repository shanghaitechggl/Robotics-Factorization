#include <iostream>
#include <opencv2/opencv.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <fstream>
using namespace std;
using namespace cv;
//�Ѿ���W���vetor
void mat2vec(float** W, vector<vector<double>>& vec, int &height, int& width);
void invalid2mean(vector<vector<double>>& vec, int height, int width, int mode);
void matrix_mul(const vector<vector<double>>& mat_one, const vector<vector<double>>& mat_two, vector<vector<double>>& result, int height, int width, int mid);
void transpose(vector<vector<double>>& W_vec, int& height, int& width);
//void centering(vector<vector<double>>& W_vec, int height, int width);
void W2file(vector<vector<double>>& W_vec);

void W2file(vector<vector<double>>& W_vec) {
	ofstream sFile("cppW.txt");
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			sFile << W[i][j] << ' ';
		}
		sFile << '\n';
	}
	sFile.close();
}

void mat2vec(float** W, vector<vector<double>>& vec, int &height, int& width) {
	//�Ѻ���ֻ��-1����ɾ��
	for (int j = width-1; j >= 0; j--) {
		bool exist = false;
		for (int i = height-1; i >=0; i--) {
			if (W[i][j] != -1)
			{
				exist = true;
				break;
			}
		}
		if (exist)
		{
			width = j+1;
			break;
		}
	}
	//��ֻ��-1����ɾ��
	for (int i = height-1; i >=0; i--) {
		bool exist = false;
		for (int j = width-1; j >=0; j--) {
			if (W[i][j]!=-1)
			{
				exist = true;
				break;
			}
		}
		if (exist)
		{
			height = i+1;
			break;
		}
	}
	//Ȼ������õĲ��ָ�vector
	for (int i = 0; i < height; i++) {
		vector<double> tmp;
		for (int j = 0; j < width; j++) {
			tmp.push_back(W[i][j]);
		}
		vec.push_back(tmp);
	}
}

//�Ѿ������-1�����һ֡�ľ�ֵ
void invalid2mean(vector<vector<double>>& vec, int height, int width, int mode=0) {
	if (mode==0)
	{
		for (int i = 0; i < height; i++) {
			int cnt = 0;
			double sum = 0;
			double mean = 0;
			for (int j = 0; j < width; j++) {
				if (vec[i][j] != -1)
				{
					cnt++;
					sum += vec[i][j];
				}
			}
			mean = sum / max(cnt, 1);
			for (int j = 0; j < width; j++) {
				if (vec[i][j] == -1)
				{
					vec[i][j] = 0;
				}
				else {
					vec[i][j] = vec[i][j] - mean;
				}
			}
		}
	}
	else {
		for (int i = 0; i < width; i++) {
			int cnt = 0;
			double sum = 0;
			double mean = 0;
			for (int j = 0; j < height; j++) {
				if (vec[j][i] != -1)
				{
					cnt++;
					sum += vec[j][i];
				}
			}
			mean = sum / max(cnt, 1);
			for (int j = 0; j < height; j++) {
				if (vec[j][i] == -1)
				{
					vec[j][i] = 0;
				}
				else {
					vec[j][i] -= mean;
				}
			}
		}
	}
}

void matrix_mul(const vector<vector<double>>& mat_one, const vector<vector<double>>& mat_two, vector<vector<double>>& result, int height, int width, int mid) {
	for (int i = 0; i < height; i++) {
		vector<double> r_i;
		for (int j = 0; j < width; j++) {
			r_i.push_back(0);
			for (int k = 0; k < mid; k++) {
				r_i[j] += mat_one[i][k] * mat_two[k][j];
			}
		}
		result.push_back(r_i);
	}
}

void transpose(vector<vector<double>>& W_vec, int& height, int& width) {
	vector<vector<double>> W_vec_T;
	for (int j = 0; j < width; j++) {
		vector<double> vec;
		for (int i = 0; i < height; i++) {
			vec.push_back(W_vec[i][j]);
		}
		W_vec_T.push_back(vec);
	}
	int tmp = height;
	height = width;
	width = tmp;
	W_vec = W_vec_T;
}

