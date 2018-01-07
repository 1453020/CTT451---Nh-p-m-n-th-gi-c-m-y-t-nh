#include <iostream>
#include <stdio.h>
#include <string>
#include "Recognition.h"

using namespace std;

int main()
{
	cout << "Chon mot trong cac lenh sau" << endl;
	cout << "0 = Thoat" << endl;
	cout << "1 = Train du lieu" << endl;
	cout << "2 = Nhan dien khuon mat" << endl;
	while (true) {
		string cmnd;
		cout << "Nhap lenh de tiep tuc:";
		cin >> cmnd;
		if (cmnd == "0") {
			return 0;
		}
		if (cmnd == "1") {
			folder_name = "camera";
			string name;
			cout << "Nhap ten: ";
			cin >> name;
			cout << "Nhan phim space de chup anh" << endl;
			Training(name);
			continue;
		}
		if (cmnd == "2") {
			folder_name = "camera";
			Recognition();
			continue;
		}
	}
	return 0;
}


