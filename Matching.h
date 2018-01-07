
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <direct.h>
#include <vector>
#include <Windows.h>
#include <string>


using namespace std;
const int bin = 1024;
int numImage = 200;

				   //tinh khoang cach giua 2 vector
double calDistance(vector <long> a, vector <long> b) {
	double kc = 0;
	int i = 0;
	while (i < bin) {
		double temp = a.at(i) - b.at(i);
		kc += temp * temp;
		i++;
	}
	return sqrt(kc);
}

//ham so khop theo KNN
string KNN(vector<long> inputHis, vector<string> listName, vector <vector <long>> trainHis, int K)
{
	vector <vector <double>> KC;

	vector<int> id;
	id.resize(listName.size());
	fill(id.begin(), id.end(), 0);

	for (int i = 0; i < trainHis.size(); i++)
	{
		vector <double> tmp;
		tmp.push_back(calDistance(inputHis, trainHis.at(i)));
		tmp.push_back(i / numImage);
		KC.push_back(tmp);
	}
	sort(KC.begin(), KC.end());

	for (int i = 0; i < K; i++)
	{
		id.at(KC.at(i).at(1))++;
	}

	int vt = distance(id.begin(), max_element(id.begin(), id.end())); 
	return listName.at(vt);
}

#pragma once
