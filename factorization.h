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
int harris_flag = 20;
int pic_num = 0;
int idx_interval = 1;
const int max_size = 200000;
vector<Point2f> features;
vector<Point2f> features_after;
vector<uchar> status;
vector<float> err;
int start = 0;
int im_height = 480;
int im_width = 640;
double arg_threshold = 0.001;
int num;
int height;
int width;
vector<vector<double>> W_vec;
bool transed = false;
int ill_cnt = 0;
bool ill_fir = false;
int ill_num = 10;
int ill_svcnt = 0;
int ill_length = 6;
Mat ill_img;
vector<Point2f> ill_lastf;
//注意！！！Mat类型的x朝下，Point2d的x朝右

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
		if (ill_fir)
		{
			for (int i = 0; i < length; i++) {
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
				//cout << "hhhahha" << endl;
				//光流法获得特征点的新位置
				features_after.clear();
				calcOpticalFlowPyrLK(images[i + 1], images[i], features, features_after, status, err);
				//opticalFlow(images[i + 1], images[i]);
				//获得新位置的最后一个点的位置，即上一张图最后一个特征点在新图的位置
				size_t fasize = features_after.size();
				//cout << fasize << endl;
				Point2d tmp = features_after[fasize - 1];
				int cnt = 0;
				//记下最后的几个点
				vector<Point2d> tmp_points;
				//退出循环的flag
				bool quit = false;
				for (int j = im_height - 1; j >= 0; j--) {
					for (int k = im_width - 1; k >= 0; k--) {
						if (harrisCorner.at<float>(j, k) > 10) {
							if (j > tmp.x || (j == tmp.x && k > tmp.y))
							{
								cnt++;
								Point2d pp(j, k);
								tmp_points.push_back(pp);
							}
							else {
								quit = true;
								break;
							}
						}
					}
					if (quit)
					{
						break;
					}
				}

				features.clear();
				//中间track的结果认为正确，直接赋值
				for (size_t j = cnt; j < fasize; j++)
				{
					int idx_j = start + j;
					W[idx_i][idx_j] = features_after[j].y;
					W[num + idx_i][idx_j] = features_after[j].x;
					Point2d pp(features_after[j].x, features_after[j].y);
					features.push_back(pp);
				}
				//在最后加cnt个新点
				if (start+cnt>=max_size)
				{
					cout << "处理到了第" << i << "张图像" << endl;
					break;
				}
				for (int j = 0; j < cnt; j++) {
					int idx_j = cnt - 1 - j;
					W[idx_i][fasize + start + j] = tmp_points[idx_j].y;
					W[idx_i + num][fasize + start + j] = tmp_points[idx_j].x;
					Point2d pp(tmp_points[idx_j]);
					features.push_back(pp);
				}
				start += cnt;
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


