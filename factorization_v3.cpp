// factorization_reconstruct.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <opencv2/opencv.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include "svd.h"
#include "factorization.h"
#include "after_svd.h"
#include <fstream>
using namespace std;
using namespace cv;


int main()
{
	////获得所有的图像
	//cv::String pattern = "F:/haorenfeng/opencv/factorization/data1/*.png";
	//images = read_images_in_folder(pattern, pic_num);
	//constructW();

	////把W变成vector形式，num是height，start+features_after.size()是width，把只有-1的列去掉，所以width会被改变
	////只有-1的行也去掉，所以height也会变
	//width = start + features_after.size();
	////readW();
	//mat2vec(W, W_vec, height, width);
	////把矩阵的-1变成这帧的均值
	//invalid2mean(W_vec, height, width, 1);
	//W2file(W_vec);
	////如果行小于列，把矩阵转置
	//if (height < width)
	//{
	//	transpose(W_vec, height, width);
	//	transed = true;
	//}
	//cout << height << width << endl;
	////SVD分解
	//vector<vector<double>> U, V;
	//vector<double> S;
	//const int k = 3;
	//svd(W_vec, k, U, S, V);
	//int uHeight = k;
	//int uWidth = height;
	//transpose(U, uHeight, uWidth);
	//vector<vector<double>> S_mat;
	//for (int i = 0; i < k; i++) {
	//	vector <double> vec;
	//	for (int j = 0; j < k; j++) {
	//		vec.push_back(0);
	//	}
	//	S_mat.push_back(vec);
	//}
	//for (int i = 0; i < k; i++) {
	//	S_mat[i][i] = sqrt(S[i]);
	//}
	//vector<vector<double>> Rot, Sp;
	//matrix_mul(U, S_mat, Rot, height, k, k);
	//matrix_mul(S_mat, V, Sp, k, width, k);
	//if (transed)
	//{
	//	int rH = height, rW = k;
	//	transpose(Rot, rH, rW);
	//	int sH = k, sW = width;
	//	transpose(Sp, sH, sW);
	//	vector<vector<double>> tmp = Rot;
	//	Rot = Sp;
	//	Sp = tmp;
	//	int tmp_two = height;
	//	height = width;
	//	width = tmp_two;
	//}
	//ofstream sFile("shape.txt");
	//for (int i = 0; i < k; i++) {
	//	for (int j = 0; j < width; j++) {
	//		sFile << Sp[i][j] << ' ';
	//	}
	//	sFile << '\n';
	//}
	//cout << Sp.size() << endl << Sp[1].size();
	//sFile.close();

	return 0;
}


