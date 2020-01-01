#include <iostream>
#include <opencv2/opencv.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <fstream>
using namespace std;
using namespace cv;

vector<Mat> images;
float** W;
int col_W = 0;
//初始化一些参数
int harris_flag = 200;
int pic_num = 200;
int idx_interval = 1;
const int max_size = 200000;
vector<Point2f> features;
vector<Point2f> features_after;
vector<uchar> status;
vector<float> err;
int start = 0;
int im_height = 480;
int im_width = 640;
double arg_threshold = 0.0001;
int num;
int height;
int width;
vector<vector<double>> W_vec;
bool transed = false;
int ill_cnt = 0;
bool ill_fir = false;
int ill_num = 5;
int ill_svcnt = 0;
int ill_length = 6;
Mat ill_img;
vector<Point2f> ill_lastf;
//注意！！！Mat类型的x朝下，Point2d的x朝右
float per_error = 5;

vector<Mat> read_images_in_folder(cv::String pattern, int pic_num);
void constructW();
void readW();
void illustration(Mat image);
void opticalFlow(Mat images0, Mat images1);

void opticalFlow(Mat images0, Mat images1) {
	features_after.clear();
	int height = images0.rows;
	int width = images0.cols;
	int interval = 1;
	int y_int = 4;
	int len_f = features.size();
	for (int i = 0; i < len_f; i++) {
		int x = features[i].y;
		int y = features[i].x;
		int startx = (x - interval) < 0 ? 0 : x - interval;
		int starty = (y - y_int) < 0 ? 0 : y - y_int;
		int endx = (x + interval) > height ? height : x + interval;
		int endy = (y + y_int) > height ? height : y + y_int;
		int pix_one = (int)images0.at<uchar>(x, y);
		int diff = 1000000;
		int x_after = 0;
		int y_after = 0;
		for (int j = startx; j < endx; j++) {
			for (int k = starty; k < endy; k++) {
				int pix_two = (int)images1.at<uchar>(j, k);
				int tmp_dif = (pix_two - pix_one) * (pix_two - pix_one);
				if (tmp_dif < diff)
				{
					diff = tmp_dif;
					x_after = j;
					y_after = k;
				}
			}
		}
		if (diff < 1000000)
		{
			Point2d pp(y_after, x_after);
			features_after.push_back(pp);
		}
	}
	//len_f = 0;
	features.clear();
	/*for (int i = 0; i < len_fa; i++)
		features[len_f++] = features_after[i];*/
	features = features_after;
}


void illustration(Mat image) {
	if (!(ill_cnt % ill_num))
	{
		//cout << ill_fir << endl;
		int length = features_after.size();
		int last_len = ill_lastf.size();
		if (ill_fir)
		{
			for (int i = 0; i < length && i<last_len; i++) {
				circle(ill_img, features_after[i], 1, Scalar(0, 255, 0), 1);
				line(ill_img, ill_lastf[i], features_after[i], Scalar(255, 0, 0));
			}
			//ill_fir = false;
			//namedWindow("show image", WINDOW_NORMAL);
			//imwrite("show_image.jpg", ill_img);
			//imshow("show image", ill_img);
			//waitKey(0);
			ill_svcnt++;
			string path = "F:/haorenfeng/opencv/factorization/frames/";
			int tmp_ill_len = ill_length;
			int tmp_ill_svcnt = ill_svcnt/10;
			while (tmp_ill_svcnt != 0) {
				tmp_ill_len--;
				tmp_ill_svcnt /= 10;
			}
			tmp_ill_len--;
			for (int i = 0; i < tmp_ill_len; i++) {
				path.append("0");
			}
			path.append(to_string(ill_svcnt));
			path.append(".png");
			imwrite(path, ill_img);

			cvtColor(image, ill_img, COLOR_GRAY2RGB);
			ill_lastf = features_after;
			for (size_t i = 0; i < length; i++)
			{
				circle(ill_img, features_after[i], 1, Scalar(0, 0, 255), 1);
			}
			//ill_fir = true;
		}
		else {
			cvtColor(image, ill_img, COLOR_GRAY2RGB);
			ill_lastf = features_after;
			for (size_t i = 0; i < length; i++)
			{
				circle(ill_img, features_after[i], 1, Scalar(0, 0, 255), 1);
			}
			ill_fir = true;
		}
	}
	ill_cnt++;
}

void readW() {
	ifstream infile("W.txt");
	height = 319;
	width = 72;
	W = new float* [319];
	for (int i = 0; i < height; i++) {
		W[i] = new float[72];
		for (int j = 0; j < width; j++) {
			infile >> W[i][j];
			//cout << W[i][j] << endl;
		}
	}
}

void constructW() {
	//通过图像数量来初始化W矩阵的维度
	num = images.size();
	height = 2 * num;
	width = max_size;

	W = new float* [height];
	//int num = height / 2;
	for (int i = 0; i < height; i++) {
		W[i] = new float[width];
		for (int j = 0; j < width; j++)
			W[i][j] = -1;
	}
	for (int i = num - 1; i >= 0; i-=idx_interval) {
		int idx_i = (num - 1 - i)/idx_interval;
		//cout << idx_i << endl;
		if (!(idx_i % harris_flag)) {
			//进行Harris角点检测找出角点
			Mat cornerStrength(im_height, im_width, CV_32FC1);
			cornerHarris(images[i], cornerStrength, 2, 3, 0.01);
			//对灰度图进行阈值操作
			Mat harrisCorner;
			threshold(cornerStrength, harrisCorner, arg_threshold, 255, THRESH_BINARY);
			if (W[0][0] == -1)
			{
				int cnt = 0;
				for (int j = 0; j < im_height; j++) {
					for (int k = 0; k < im_width; k++) {
						if (harrisCorner.at<float>(j, k) > 10)
						{
							W[idx_i][start + cnt] = j;
							W[num + idx_i][start + cnt] = k;
							cnt++;
							Point2d tmp(k, j);
							features.push_back(tmp);
							//cout << k << endl;
						}
					}
				}
				//cout << W[0][0];
			}
			else {
				//光流法获得特征点的新位置
				features_after.clear();
				calcOpticalFlowPyrLK(images[i + 1], images[i], features, features_after, status, err);
				//不断对比光流法和Harris的结果，把Harris与光流法相等的部分记录下来
				//当光流法的结果的被遍历完后，把Harris的结果依次填在后面
				int tmp_idx_point=0;
				int tmp_after_size = features_after.size();
				int tmp_idx_W = start;
				features.clear();
				for (int k = 0; k < im_width; k++) {
					for (int j = 0; j < im_height; j++) {
						float tmp_x;
						float tmp_y;
						if (tmp_idx_point < tmp_after_size)
						{
							tmp_x = features_after[tmp_idx_point].x;
							tmp_y = features_after[tmp_idx_point].y;
						}
						if (tmp_idx_point>=tmp_after_size)
						{
							if (harrisCorner.at<float>(j, k) > 10) {
								W[idx_i][tmp_idx_W] = j;
								W[idx_i + num][tmp_idx_W++] = k;
								Point2f pp(k, j);
								features.push_back(pp);
							}
						}
						else if ((k < tmp_x&&abs(k-tmp_x)>per_error)||((k == tmp_x|| abs(k - tmp_x) <= per_error)&&j<tmp_y&&abs(j-tmp_y)>per_error))
						{
							if (tmp_idx_point==0)
							{
								start++;
							}
							continue;
						}
						else if (harrisCorner.at<float>(j,k)>10)
						{
							if (abs(k-tmp_x)<= per_error &&abs(j-tmp_y)<= per_error)
							{
								W[idx_i][tmp_idx_W] = j;
								W[idx_i + num][tmp_idx_W++] = k;
								Point2f pp(k, j);
								features.push_back(pp);
								tmp_idx_point++;
							}
							else {
								while ((tmp_x<k&&(k - tmp_x)>per_error)||(abs(k-tmp_x)<= per_error &&tmp_y<j&&(j-tmp_y)>per_error))
								{
									tmp_idx_point++;
									if (tmp_idx_point >= tmp_after_size)
									{
										break;
									}
									tmp_x = features_after[tmp_idx_point].x;
									tmp_y = features_after[tmp_idx_point].y;
								}
							}
						}
					}
				}
			}
		}
		else {
			features_after.clear();
			calcOpticalFlowPyrLK(images[i + 1], images[i], features, features_after, status, err);
			//opticalFlow(images[i + 1], images[i]);
			illustration(images[i]);
			int length = start + features_after.size();
			//cout << length << endl;
			//把track到的点赋值
			if(length!=start)
				features.clear();
			for (int j = start; j < length; j++) {
				int idx_j = j - start;
				W[idx_i][j] = features_after[idx_j].y;
				W[idx_i + num][j] = features_after[idx_j].x;
				Point2d pp(features_after[idx_j].x, features_after[idx_j].y);
				features.push_back(pp);
			}
			//features = features_after;
		}
	}
}

vector<Mat> read_images_in_folder(cv::String pattern, int pic_num)
{
	vector<cv::String> fn;
	glob(pattern, fn, false);

	vector<Mat> images;
	size_t count = fn.size(); //number of png files in images folder
	pic_num = pic_num == 0 ? count : pic_num;
	for (size_t i = 0; i < pic_num; i++)
	{
		images.push_back(imread(fn[i], 0));
		//cout << fn[i] << endl;
		//waitKey(1);
	}
	return images;
}


