#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<utility>
#include<algorithm>
#include <bits/stdc++.h>

using namespace std;

void error_handler(int);
void quick_sort(char*);
int partition(char*);
string form_string(char*);
string extract_string(string);
string make_string(char*);

vector<pair<string,pair<int,int>>> cols;
vector<pair<string,int>> atts;
vector<pair<int,int>> blocks;
vector<pair<int,int>> segments;

void error_handler(int code){
	if (code == 1)
		cout << "Invalid No.of arguments\n";
	exit(0);
}

string form_string(char s[]){
	string t = "";

	for (int i = 0; i < cols.size(); i++){
		for (int j = cols[i].second.first; j <= cols[i].second.first+cols[i].second.second; j++)
			t += s[j];
	}
	return t;
}

string extract_string(string s){
	string t = "";
	int k = 0, l = s.length();
	for (int i = 0; i < atts.size(); i++){
		for (int j = 1; j <= atts[i].second; j++){
			t += s[k++];
		}
		while (k < l && s[k] == ' ')
			k++;
	}
	return t;
}

string make_string(char *a){
	string t = "";
	int j = 0, k = 1;

	for (int i = 0; i < atts.size(); i++){
		k = 1;
		while (k <= atts[i].second)
			t += a[j++],k++;
		if (i != (atts.size() - 1))
			t += "  ";
	}
	return t;
}

bool compareRec(char *a, char *b){
	string s = form_string(a);
	string t = form_string(b);
	return (s <= t);
}

int partition(char *a[], int low, int high){
	int pivot = low;
	string p = form_string(a[low]);
	string e;
	int i = low, j = high;
	char *temp;

	while (i < j){
		e = form_string(a[i]);
		while(i <= high && p <= e){
			e = form_string(a[i]);
			i++;
		}

		e = form_string(a[j]);
		while(j >= low && p >= e){
			e = form_string(a[j]);
			j--;
		}
		

		if (i >= j){
			temp = a[pivot];
			a[pivot] = a[j];
			a[j] = temp;
		}
		else{
			temp = a[i];
			a[i] = a[j];
			a[j] = temp;
		}
	}
	return j;
}

void quick_sort(char *a[], int low, int high){
	if (low >= high){
		cout << "-----------Here\n";
		return;
	}
	int p = partition(a, low, high);
	cout << "There------------\n";
	quick_sort(a, low, p-1);
	quick_sort(a, p+1, high);
}

int main(int argc, char *argv[]){

	fstream f;
	fstream fout;
	
	int j = 0, k = 0;
	long int i = 0;
	int tuple_size = 0;
	int n_threads = 0;
	unsigned long int RAM = 1000000 * stoi(argv[3]);
	unsigned long int used = 0; 
	string order;

	f.open("metadata.txt",ios::in);
	string s;
	while(getline(f, s)){
		i = s.find(",");
		atts.push_back(make_pair(s.substr(0,i),stoi(s.substr(i+1))));
		tuple_size += stoi(s.substr(i+1));
	}
	f.close();

	s = argv[4];
	if(s != "asc" && s != "desc"){
		n_threads = stoi(argv[4]);
		s = argv[5];
		if (s == "asc")
			order = "asc";
		else if (s == "desc")
			order = "desc";
		else
			error_handler(1);
		i = 6;
	}

	else{
		if (s == "asc")
			order = "asc";
		else if (s == "desc")
			order = "desc";
		else
			error_handler(1);
		i = 5;
	}

	for(;i<argc;i++){
		k = 0;
		for (j = 0; j < atts.size(); j++){
			if (atts[j].first == argv[i]){
				cols.push_back(make_pair(argv[i],make_pair(k,atts[j].second)));
				break;
			}
			else
				k += atts[j].second;
		}
		//cols.push_back(argv[i]);
	}

	long int frames = 0;
	if (RAM / tuple_size > 100000000)
		frames = 100000000;
	else
		frames = RAM / (tuple_size+1);

	cout << frames << "\n";

	char **a = new char*[frames];
	for (long int i = 0; i < frames;i++)
		a[i] = new char[tuple_size+1];

	bool end = false;
	int pass = 0;
	f.open(argv[1],ios::in);
	fout.open("middle.txt",ios::out);
	string chars = " ";
	while (!end){
		for (i = 0;i < frames;i++){
			if(getline(f,s)){
				s = extract_string(s);
			    if (s.length() != (tuple_size))
			    	cout << s+"\n";
				strcpy(a[i], s.c_str());
			}
			else{
				end = true;
				break;
			}
		}
		if (i != 0){
			cout << i << "\n";
			sort(a,a+i,compareRec);
			blocks.push_back(make_pair(0,i));
			pass += 1;
			cout << s+"\n";
			//quick_sort(a, 0, i-1);
			cout << "Sorting " << pass << "\n" << flush;
			for (j = 0; j < i;j++){
				fout << a[j] << "\n";
			}
			cout << "Writing " << pass << " to disk completed\n" << flush;
		}
	}

	f.close();
	fout.close();

	fstream recs[pass];
	int sort_size = frames / (pass+1);

	recs[0].open("middle.txt",ios::in);
	segments.push_back(make_pair(0,0));
	for (i = 1; i < pass; i++){
		recs[i].open("middle.txt",ios::in);
		for (int j = 0; j < i;j++)
			recs[i].seekg(frames*(tuple_size+1), recs[i].cur);
		segments.push_back(make_pair(0,0));
	}
	segments.push_back(make_pair(0,frames - pass*sort_size));

	fout.open("output.txt",ios::out);

	for (i = 0, j = 0; i < pass; i++){
		segments[i].first = i*sort_size;
		j = i*sort_size;
		for (int block_size = 0; block_size < sort_size; block_size++){
			if(getline(recs[i],s)){
				strcpy(a[j++], s.c_str());
				blocks[i].first++;
				segments[i].second++;
				if (blocks[i].first == blocks[i].second)
					break;
			}
			else
				break;
		}
	}
	segments[pass].first = pass*sort_size;

	//for (int i = 0; i < frames; i++){
	//	cout << a[i] << "\n";
	//}

	/*for (int i = 0; i < pass; i++){
		cout << segments[i].first << "\n";
		cout << segments[i].second << "\n";
		cout << blocks[i].first << "\n";
		cout << blocks[i].second << "\n";
	}*/

	int completed = 0;
	char *t = NULL;
	int min_loc = -1;

	while (completed < pass){
		min_loc = -1;

		for (i = 0; i < pass; i++){
			if (segments[i].second > 0){
				if (min_loc == -1 || form_string(a[segments[i].first]) < form_string(t)){
					min_loc = i;
					t = a[segments[i].first];
				}
			}
		}

		//cout << min_loc << "\n";
		if (min_loc != -1){
			s = t;
			strcpy(a[segments[pass].first++], s.c_str());
			segments[min_loc].first++;
			segments[min_loc].second--;
			segments[pass].second--;
		}

		else{
			for (i = pass*sort_size;i < segments[pass].first; i++){
				s = make_string(a[i]);
				fout << s+"\n"; 
			}
			fout << flush;
			segments[pass].first = pass*sort_size;
			segments[pass].second = frames - segments[pass].first;
		}
		if (segments[pass].second <= 0){
			for (i = pass*sort_size;i < segments[pass].first; i++){
				s = make_string(a[i]);
				fout << s+"\n"; 
			}
			fout << flush;
			segments[pass].first = pass*sort_size;
			segments[pass].second = frames - segments[pass].first;
		}

		if (segments[min_loc].second <= 0){
			i = min_loc*sort_size;
			while (blocks[min_loc].first < blocks[min_loc].second && segments[min_loc].second < sort_size){
				getline(recs[min_loc], s);
				strcpy(a[i++], s.c_str());
				blocks[min_loc].first++;
				segments[min_loc].second++;
			}
			segments[min_loc].first = min_loc*sort_size;
			if (blocks[min_loc].first >= blocks[min_loc].second && segments[min_loc].second <= 0){
				cout << "List " << min_loc+1 << " Completed\n";
				completed++;
			}
		}
	}

	for (i = pass*sort_size;i < segments[pass].first; i++){
		s = make_string(a[i]);
		fout << s+"\n"; 
	}
	fout << flush;
	segments[pass].first = pass*sort_size;
	segments[pass].second = frames - segments[pass].first;

	for (i = 0; i < pass; i++)
		recs[i].close();
	fout.close();
	/*f.open(argv[2], ios::out);

	int completed = 0;

	getline(f,s);
	cout << s;*/

	//while(1);
	//cout << sizeof(s);
	return 0;
}