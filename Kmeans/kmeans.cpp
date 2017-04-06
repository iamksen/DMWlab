#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <set>
#include <fstream>
#include <stdlib.h>
#include <cmath>

#define K 2
#define MAX_ITERATIONS 150
using namespace std;

vector< vector<float> > dataset; //map to store dataset; vector<row> 
set<int> temp_cluster[K+1], cluster[K+1]; // there will K clusters
set<int> :: iterator sit;
pair<float, float> centroid[K+1]; // centroids (A_centroid, B_centroid) of K clusters

/* Process input from input file */
void processinput()
{
	string line;
	vector<float> entry;
	ifstream file;
	file.open("input.txt");

	if( file.is_open() == false ){ //if there is error in opening the file
		cout<<"Error while opening input file"<<endl;
		return;
	}

	int count = 0, c;
	//Reading file line by line
	while( getline(file, line)){
		//cout<<" line "<<line<<endl;
		if( count == 0 ){
			count++; continue;
		}
		entry.clear();
		for(int i = 0; i < (int)line.length(); i++){
			if( line[i] == ' ' || line[i] == '\t') continue; //delimiters
			char value[10];
			c = 0;
			while( ((line[i] >= '0' && line[i] <= '9') || line[i] == '.') && (int)line.length() ){ //for string => float
				value[c++] = line[i];
				i++;
			}
			entry.push_back(atof(value)); //convert string(char *) to float
		}
		dataset.push_back(entry);
	}
	file.close(); //close file
}

/* function to assing cluster to datapoints*/
void assign_cluster()
{
	float d_min, sq_eclid_dist;
	int index;
		
	for(int row = 0 ; row < (int)dataset.size(); row++){ // loop through all dataset
		index = 1;
		d_min = pow((dataset[row][1] - centroid[1].first),2) + pow((dataset[row][2] - centroid[1].second),2);
		for(int in = 1 ; in <= K ; in++){ //check distance from all centroids
			sq_eclid_dist = pow((dataset[row][1] - centroid[in].first),2) + pow((dataset[row][2] - centroid[in].second),2);
			if( sq_eclid_dist <= d_min ){
				index = in;
				d_min = sq_eclid_dist;	
			}
		}
		cluster[index].insert(row);
	}
}

void calculate_centroid()
{
	int row_no, size_of_cluster;
	for(int i = 1 ; i <= K; i++){
		float sumA = 0, sumB = 0;
		size_of_cluster = (int)cluster[i].size(); //size of ith cluster
		for( sit = cluster[i].begin() ; sit != cluster[i].end(); sit++){ // loop through whole cluster
			row_no = *sit;
			sumA += dataset[row_no][1]; // value of A (in dataset[row_no][1])
			sumB += dataset[row_no][2]; // value of B (in dataset[row_no][2])
		}
		centroid[i] = make_pair(sumA/size_of_cluster, sumB/size_of_cluster); //centroid of ith cluster
	}
}

void copy_cluster()
{
	for(int i = 1; i <= K; i++){
		temp_cluster[i].clear();
		for(sit = cluster[i].begin() ; sit != cluster[i].end(); sit++)
			temp_cluster[i].insert(*sit);
		cluster[i].clear();
	}
}

bool is_equal()
{
	for(int i = 1; i <= K; i++){
		if( cluster[i].size() != temp_cluster[i].size() ) return false;
		if( cluster[i] != temp_cluster[i]) return false;
	}	
	return true;
}

void print_clusters()
{
	for(int i = 1; i <= K; i++){
		cout<<"Cluster "<<i<<" -->\n";
		cout<<"Subject A B"<<endl;
		for(sit = cluster[i].begin(); sit != cluster[i].end(); sit++){
			int row = *sit;
			cout<<dataset[row][0]<<" "<<dataset[row][1]<<" "<<dataset[row][2]<<endl;
		}
		cout<<endl;
	}	
}

void kmeans()
{
	for(int i = 1; i <= K; i++)
		cluster[i].insert(i-1); //choosing first k rows as k clusters
	
	for(int i = 0; i <= K; i++){ //making a copy of cluster
		for(sit = cluster[i].begin(); sit != cluster[i].end(); sit++)
			temp_cluster[i].insert(*sit);
	}

	for(int i = 0 ; i < MAX_ITERATIONS ; i++){
		calculate_centroid(); // calculate the centroid of all clusters
		copy_cluster(); // make copy of cluster into a temp_cluster
		assign_cluster(); // assign row to clusters
		if( is_equal() ) // checking if there is not shifting of any pt from one cluster to another
			break;
	}
	print_clusters(); // print clusters
}

int main()
{
	processinput();
	kmeans();
	return 0;
}