
#include "matching.h"
#include <vector>
#include "opencv\cv.h"
#include "opencv\core.hpp"
#include "opencv\opencv.hpp"
#include "opencv\imgproc.hpp"

using namespace std;
using namespace cv;

vector <long> Histogram;
int img_size = 300;
string folder_name;
ofstream myfile;

//them histogram vao list training
int loadFile(vector<string> &nameList, vector <vector <long>> &Trainstogram)
{
	ifstream f;
	f.open(folder_name + "/namelist.txt");
	if (!f.is_open()) {
		cout << "Chua co du lieu!" << endl;
		return -1;
	}
	while (!f.eof())
	{
		string line;
		getline(f, line);
		if (line != "")
		{
			nameList.push_back(line);
		}
	}
	f.close();


	for (int i = 0; i < nameList.size(); i++)
	{
		ifstream in(folder_name + "/training/" + nameList.at(i) + "/" + "histogram.txt");
		for (int j = 0; j < numImage; j++)
		{
			vector<long> his;
			for (int k = 0; k < bin; k++)
			{
				string temp;
				in >> temp;
				his.push_back(stol(temp));
			}
			Trainstogram.push_back(his);
		}
	}
	return sqrt(nameList.size());
}

void gammaCorrection(Mat sourceImage, Mat &destinationImage) {
	float Gamma = 1.5;
	destinationImage = sourceImage.clone();
	for (int j = 0; j < sourceImage.rows; j++)
	{
		for (int i = 0; i < sourceImage.cols; i++)
		{

			destinationImage.at<uchar>(Point(i, j)) = pow((float)(sourceImage.at<uchar>(Point(i, j)) / 255.0), 1 / Gamma) * 255.0f;
		}
	}
}

void DOGfilter(Mat sourceImage, Mat &destinationImage, int kernel_size_1, int kernel_size_2) {
	Mat g1, g2;
	GaussianBlur(sourceImage, g1, Size(kernel_size_1, kernel_size_1), 0);
	GaussianBlur(sourceImage, g2, Size(kernel_size_2, kernel_size_2), 0);
	destinationImage = g1 - g2;
}
//ham ap dung cac phuong phap xu ly anh
void ImagePreprocessing(Mat sourceImage, Mat &destinationImage) {

	Mat temp = sourceImage;
	cvtColor(temp, temp, CV_RGB2GRAY);
	gammaCorrection(temp, temp);
	DOGfilter(temp, temp, 3, 5);
	equalizeHist(temp, temp);
	destinationImage = temp;
}

//ham ma hoa histogram
char binaryEncoder(int src1, int des1, int src2, int des2) {
	int I1 = src1 - des1;
	int I2 = src2 - des2;
	if (I1 * I2 > 0)
		return '0';
	else
		return '1';
}

//ham tinh LDP bac 3
void thirdOrderLDP(Mat img, int x, int y) {

	/*	25 26 27 28 29 30 31
		48 09 10 11 12 13 32
		47 24 01 02 03 14 33
		46 23 08 00 04 15 34
		45 22 07 06 05 16 35
		44 21 20 19 18 17 36
		43 42 41 40 39 38 37	*/


	char binary_string[32];
	vector <int> A;
	A.resize(49);
	fill(A.begin(), A.end(), 0);
	Scalar intensity;

	intensity = img.at<uchar>(Point(x, y));
	A[0] = intensity.val[0];
	intensity = img.at<uchar>(Point(x - 1, y - 1));
	A[1] = intensity.val[0];
	intensity = img.at<uchar>(Point(x - 1, y));
	A[2] = intensity.val[0];
	intensity = img.at<uchar>(Point(x - 1, y + 1));
	A[3] = intensity.val[0];
	intensity = img.at<uchar>(Point(x, y + 1));
	A[4] = intensity.val[0];
	intensity = img.at<uchar>(Point(x + 1, y + 1));
	A[5] = intensity.val[0];
	intensity = img.at<uchar>(Point(x + 1, y));
	A[6] = intensity.val[0];
	intensity = img.at<uchar>(Point(x + 1, y - 1));
	A[7] = intensity.val[0];
	intensity = img.at<uchar>(Point(x, y - 1));
	A[8] = intensity.val[0];
	intensity = img.at<uchar>(Point(x - 2, y - 2));
	A[9] = intensity.val[0];
	intensity = img.at<uchar>(Point(x - 2, y - 1));
	A[10] = intensity.val[0];
	intensity = img.at<uchar>(Point(x - 2, y));
	A[11] = intensity.val[0];
	intensity = img.at<uchar>(Point(x - 2, y + 1));
	A[12] = intensity.val[0];
	intensity = img.at<uchar>(Point(x - 2, y + 2));
	A[13] = intensity.val[0];
	intensity = img.at<uchar>(Point(x - 1, y + 2));
	A[14] = intensity.val[0];
	intensity = img.at<uchar>(Point(x, y + 2));
	A[15] = intensity.val[0];
	intensity = img.at<uchar>(Point(x + 1, y + 2));
	A[16] = intensity.val[0];
	intensity = img.at<uchar>(Point(x + 2, y + 2));
	A[17] = intensity.val[0];
	intensity = img.at<uchar>(Point(x + 2, y + 1));
	A[18] = intensity.val[0];
	intensity = img.at<uchar>(Point(x + 2, y));
	A[19] = intensity.val[0];
	intensity = img.at<uchar>(Point(x + 2, y - 1));
	A[20] = intensity.val[0];
	intensity = img.at<uchar>(Point(x + 2, y - 2));
	A[21] = intensity.val[0];
	intensity = img.at<uchar>(Point(x + 1, y - 2));
	A[22] = intensity.val[0];
	intensity = img.at<uchar>(Point(x, y - 2));
	A[23] = intensity.val[0];
	intensity = img.at<uchar>(Point(x - 1, y - 2));
	A[24] = intensity.val[0];

	//0o
	//1 08-> 00 -> 04 và 24 -> 01 -> 02
	binary_string[0] = binaryEncoder(A[8] - A[0], A[4], A[24] - A[1], A[2]);
	//2 08-> 00 -> 04 và 01 -> 02 -> 03
	binary_string[1] = binaryEncoder(A[8] - A[0], A[4], A[1] - A[2], A[3]);
	//3 08-> 00 -> 04 và 02 -> 03 -> 14
	binary_string[2] = binaryEncoder(A[8] - A[0], A[4], A[2] - A[3], A[14]);
	//4 08-> 00 -> 04 và 00 -> 04 -> 15
	binary_string[3] = binaryEncoder(A[8] - A[0], A[4], A[0] - A[4], A[15]);
	//5 08-> 00 -> 04 và 06 -> 05 -> 16
	binary_string[4] = binaryEncoder(A[8] - A[0], A[4], A[6] - A[5], A[16]);
	//6 08-> 00 -> 04 và 07 -> 06 -> 05
	binary_string[5] = binaryEncoder(A[8] - A[0], A[4], A[7] - A[6], A[5]);
	//7 08-> 00 -> 04 và 22 -> 07 -> 06
	binary_string[6] = binaryEncoder(A[8] - A[0], A[4], A[22] - A[7], A[6]);
	//8 08-> 00 -> 04 và 23 -> 08 -> 00
	binary_string[7] = binaryEncoder(A[8] - A[0], A[4], A[23] - A[8], A[0]);

	//45o
	//1 07 -> 00 -> 03 và 23 -> 01 -> 11
	binary_string[8] = binaryEncoder(A[7] - A[0], A[3], A[21] - A[1], A[11]);
	//2 07 -> 00 -> 03 và 08 -> 02 -> 12
	binary_string[9] = binaryEncoder(A[7] - A[0], A[3], A[8] - A[2], A[12]);
	//3 07 -> 00 -> 03 và 00 -> 03 -> 13
	binary_string[10] = binaryEncoder(A[7] - A[0], A[3], A[0] - A[3], A[13]);
	//4 07 -> 00 -> 03 và 06 -> 04 -> 14
	binary_string[11] = binaryEncoder(A[7] - A[0], A[3], A[6] - A[4], A[14]);
	//5 07 -> 00 -> 03 và 19 -> 05 -> 15
	binary_string[12] = binaryEncoder(A[7] - A[0], A[3], A[19] - A[5], A[15]);
	//6 07 -> 00 -> 03 và 20 -> 06 -> 04
	binary_string[13] = binaryEncoder(A[7] - A[0], A[3], A[20] - A[6], A[4]);
	//7 07 -> 00 -> 03 và 21 -> 07 -> 00
	binary_string[14] = binaryEncoder(A[7] - A[0], A[3], A[21] - A[7], A[0]);
	//8 07 -> 00 -> 03 và 22 -> 08 -> 02
	binary_string[15] = binaryEncoder(A[7] - A[0], A[3], A[22] - A[8], A[2]);

	//90o
	//1 06 -> 00 -> 02 và 08 -> 01 -> 10
	binary_string[16] = binaryEncoder(A[6] - A[0], A[4], A[8] - A[1], A[10]);
	//2 06 -> 00 -> 02 và 00 -> 02 -> 11
	binary_string[17] = binaryEncoder(A[6] - A[0], A[4], A[0] - A[2], A[11]);
	//3 06 -> 00 -> 02 và 04 -> 03 -> 12
	binary_string[18] = binaryEncoder(A[6] - A[0], A[4], A[4] - A[3], A[12]);
	//4 06 -> 00 -> 02 và 05 -> 04 -> 03
	binary_string[19] = binaryEncoder(A[6] - A[0], A[4], A[5] - A[4], A[3]);
	//5 06 -> 00 -> 02 và 18 -> 05 -> 04
	binary_string[20] = binaryEncoder(A[6] - A[0], A[4], A[18] - A[5], A[4]);
	//6 06 -> 00 -> 02 và 19 -> 06 -> A[0]
	binary_string[21] = binaryEncoder(A[6] - A[0], A[4], A[19] - A[6], A[0]);
	//7 06 -> 00 -> 02 và 20 -> 07 -> 08
	binary_string[22] = binaryEncoder(A[6] - A[0], A[4], A[20] - A[7], A[8]);
	//8 06 -> 00 -> 02 và 07 -> 08 -> 01
	binary_string[23] = binaryEncoder(A[6] - A[0], A[4], A[7] - A[8], A[1]);

	//135o
	//1 05 -> 00 -> 01 và 00 -> 01 -> 09
	binary_string[24] = binaryEncoder(A[5] - A[0], A[4], A[0] - A[1], A[9]);
	//2 05 -> 00 -> 01 và 04 -> 02 -> 10
	binary_string[25] = binaryEncoder(A[5] - A[0], A[4], A[4] - A[2], A[10]);
	//3 05 -> 00 -> 01 và 15 -> 03 -> 11
	binary_string[26] = binaryEncoder(A[5] - A[0], A[4], A[15] - A[3], A[11]);
	//4 05 -> 00 -> 01 và 16 -> 04 -> 02
	binary_string[27] = binaryEncoder(A[5] - A[0], A[4], A[16] - A[4], A[2]);
	//5 05 -> 00 -> 01 và 17 -> 05 -> 00
	binary_string[28] = binaryEncoder(A[5] - A[0], A[4], A[17] - A[5], A[0]);
	//6 05 -> 00 -> 01 và 18 -> 06 -> 08
	binary_string[29] = binaryEncoder(A[5] - A[0], A[4], A[18] - A[6], A[8]);
	//7 05 -> 00 -> 01 và 19 -> 07 -> 23
	binary_string[30] = binaryEncoder(A[5] - A[0], A[4], A[19] - A[7], A[23]);
	//8 05 -> 00 -> 01 và 06 -> 08 -> 24
	binary_string[31] = binaryEncoder(A[5] - A[0], A[4], A[6] - A[8], A[24]);

	char * end;
	long long value = strtoll(binary_string, &end, 2);
	int threshold = value / pow(2, 32) * bin;
	Histogram.at(threshold)++;
}

//ham ap dung LDP cho HOG
void LDP(Mat img) {
	Histogram.resize(bin);
	fill(Histogram.begin(), Histogram.end(), 0);
	int size = img.cols;
	for (int i = 2; i < size - 2; i++)
	{
		for (int j = 2; j < size - 2; j++)
		{
			thirdOrderLDP(img, i, j);
		}
	}
}


void Training(String name)
{
	Mat input_img, grayscale_img;
	vector<Rect> face;

	CascadeClassifier face_cascade;
	if (!face_cascade.load("C:\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt2.xml")) {
		printf("Loi thu vien");
	}

	VideoCapture capture(0);
	if (!capture.isOpened())
	{
		printf("Loi camera");
	}
	namedWindow("Trai du lieu", 1);
	bool check = true;
	int count = 0;
	_mkdir(folder_name.c_str());
	String path = folder_name + "/training/";
	_mkdir(path.c_str());
	String folder_path = path + name + "/";
	String image_path = folder_path + "image/";
	_mkdir(folder_path.c_str());
	_mkdir(image_path.c_str());
	myfile.open(folder_path + "histogram.txt");
	while (check)
	{
		capture >> input_img;
		rectangle(input_img, Point(180 + 300, 100 + 300), Point(180, 100), cvScalar(0, 255, 0), 2, 8, 0);
		Mat processed_img;
		if (!input_img.empty()) {
			waitKey(10);

			face_cascade.detectMultiScale(input_img, face, 1.1, 10, CV_HAAR_SCALE_IMAGE | CV_HAAR_DO_CANNY_PRUNING, cvSize(0, 0), cvSize(300, 300));
			for (int i = 0; i < face.size(); i++)
			{
				Point pt1(face[i].x + face[i].width - 10, face[i].y + face[i].height - 10);
				Point pt2(face[i].x + 10, face[i].y + 10);

				if (pt1.x <= 480 && pt1.y <= 400 && pt2.x >= 160 && pt2.y >= 120)
				{
					//dong khung khuon mat
					rectangle(input_img, pt1, pt2, cvScalar(255, 0, 0), 4, 8, 0);
					putText(input_img,/*"Training..."*/to_string(count), cvPoint(30, 50), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 255, 0), 1, CV_AA);

					String filepath = folder_path + "image/image-" + to_string(count) + ".jpg";
					Size size(img_size, img_size);
					ImagePreprocessing(input_img(face[i]), processed_img);
					resize(processed_img, processed_img, size, CV_INTER_AREA);
					imwrite(filepath, processed_img);
					LDP(processed_img);

					for (int i = 0; i < bin; i++) {
						myfile << Histogram.at(i) << " ";
					}
					myfile << endl;
					count++;
					cout << " " << endl;
					if (count == numImage)
						check = false;
				}
			}
			imshow("Face training", input_img);
		}
		else {
			printf("Hay de vua khuon mat vao hinh vuong");
		}
	}
	myfile.close();
	ofstream myfile;
	myfile.open(folder_name + "/namelist.txt", ios::out | ios::app);
	myfile << name;
	myfile << endl;
	myfile.close();

	cvDestroyAllWindows();
}

void Recognition()
{
	vector<string> nameList;
	vector <vector <long>> Traintogram;
	if (loadFile(nameList, Traintogram) == -1)
	{
		return;
	}
	int K = sqrt(Traintogram.size());
	Mat input_img;
	vector<Rect> face;
	//goi thu muc ho tro nhan dang khuon mat
	CascadeClassifier face_cascade;
	if (!face_cascade.load("C:\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt2.xml")) {
		printf("Loi thu vien");
	}
	VideoCapture capture(0);
	bool check = true;
	if (!capture.isOpened())
	{
		printf("Loi camera");
		check = false;
	}
	//qua trinh nhan dien khuon mat
	while (check)
	{
		capture >> input_img;
		Mat processed_img;
		if (!input_img.empty()) {
			if (waitKey(10) == 27)
			{
				check = false;
			}
			face_cascade.detectMultiScale(input_img, face, 1.1, 10, CV_HAAR_SCALE_IMAGE | CV_HAAR_DO_CANNY_PRUNING, cvSize(0, 0), cvSize(300, 300));
			for (int i = 0; i < face.size(); i++)
			{

				Point pt1(face[i].x + face[i].width + 10, face[i].y + face[i].height + 10);
				Point pt2(face[i].x - 10, face[i].y - 10);

				//dong khung khuon mat
				rectangle(input_img, pt1, pt2, cvScalar(255, 0, 0), 4, 8, 0);
				Size size(img_size, img_size);
				ImagePreprocessing(input_img(face[i]), processed_img);
				resize(processed_img, processed_img, size);
				LDP(processed_img);
				//the hien thong tin nhan dien duoc
				putText(input_img, KNN(Histogram, nameList, Traintogram, K), cvPoint(face[i].x - 10, face[i].y - 20), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 0, 0), 1, CV_AA);
			}
			imshow("Face recognition", input_img);
		}
		else {
			printf("Khong the nhan dien khuon mat");
		}
	}
	cvDestroyAllWindows();
}
