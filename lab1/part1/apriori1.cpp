#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <cmath>
using namespace std;

int MIN_SUPPORT;
int MIN_CONFIDENCE;

map< int, vector<string> > dataset; //map to store dataset; map< TID, ITEMSET >
map< int, vector<string> > :: iterator mpit;
map< vector<string>, int> freq; //stores frequency of itemsets; map< ITEMSET, FREQUENCY >

/* Function to print dataset (transactions) */
void print_dataset()
{
	cout<<"Dataset\n";
	cout<<"[ TID ]--> ITEMSET\n";
	for(mpit = dataset.begin(); mpit != dataset.end(); mpit++){
		cout<<"[ "<<mpit->first<<" ]--> ";
		cout<<"{ "<<mpit->second[0]; 
		for(int i = 1 ; i < mpit->second.size(); i++)
			cout<<" , "<<mpit->second[i];
		cout<<" }"<<endl;
	}
}

/* Printing utility function */
void print( map< vector<string>, int> data ) // map< ITEMSET, FREQUENCY >
{
	vector<string> temp;
	map< vector<string>, int> :: iterator mit;
	cout<<"[ ITEMSET ]--> SUPPORT\n";
	for( mit = data.begin(); mit != data.end(); mit++){ //loop over data
		temp = mit->first;
		cout<<"[ ";
		cout<<temp[0];
		for(int i = 1 ; i < (int)temp.size(); i++) //itemset
			cout<<" , "<<temp[i];
		cout<<" ]--> "<<mit->second; //frequency of itemset
		if( mit->second < MIN_SUPPORT ) cout<<" (will be pruned)"; //pruned - while calculating Lk
		cout<<endl;
	}
}

/* Calculate C1 : possible candidate with 1-itemset */
void calculate_c1()
{
	vector<string> temp;
	for(mpit = dataset.begin(); mpit != dataset.end(); mpit++){
		for(int i = 0 ; i < mpit->second.size(); i++){
			temp.clear();
			temp.push_back(mpit->second[i]);
			freq[temp]++;
		}
	}
}

/* Calculate L1 : frequent 1-itemsets */
void calculate_l1()
{
	map< vector<string>, int> :: iterator mit;
	for( mit = freq.begin(); mit != freq.end(); mit++){
		if( mit->second < MIN_SUPPORT ){
			freq.erase(mit); //remove that itemset; support < min_support
		}
	}
}

/* Calculate frequency of itemset in all transactions */
int support(set<string> itemset)
{
	set<string> :: iterator it;
	int count, result = 0;
	for(mpit = dataset.begin(); mpit != dataset.end(); mpit++){
		count = 0;
		for(it = itemset.begin(); it != itemset.end(); it++)
			if( find( mpit->second.begin(), mpit->second.end(), *it) != mpit->second.end()) //check if itemset is present in the transaction
				count++;
		if( count == itemset.size())
			result++;
	}
	return result;
}

/* Calculate item set in Ck */
void calculate_ck(int k)
{
	map< vector<string>, int> result;
	map< vector<string>, int> :: iterator mp1, mp2;
	set<string> itemset;
	set<string> ::iterator it;
	for(mp1 = freq.begin() ; mp1 != freq.end(); mp1++){ //generating possible candidates for frequent itemset
		for(mp2 = freq.begin(); mp2 != freq.end(); mp2++){
			if( mp1->first == mp2->first ) continue; //continue for same itemset
			itemset.clear();
			itemset.insert(mp1->first.begin(), mp1->first.end() );
			itemset.insert(mp2->first.begin(), mp2->first.end() ); //generate the candidate itemset
			if( itemset.size() == k ){ // if no of item in itemset is k
				if( support(itemset) != 0 ){ //if candidate itemset is present in dataset (transactions)
					vector<string> tt(itemset.begin(), itemset.end());
					result[tt] = support(itemset); //add itemset to result ( Ck )
				}
			}
			//else
			//	cout<<"koi na\n";
		}
	}
	freq.clear();
	freq = result; //update freq with the current result ( Ck )
}

/* Calculate item set in Lk, using Ck */
bool calculate_lk(int k)
{
	map< vector<string>, int> result;
	map< vector<string>, int> :: iterator mp1;
	set<string> s1;
	for(mp1 = freq.begin() ; mp1 != freq.end(); mp1++){ //loop through for frequent itemset
		s1.clear();
		s1.insert(mp1->first.begin(), mp1->first.end());
		if( support(s1) >= MIN_SUPPORT ){ //check support of itemset
			result[mp1->first] = support(s1); //add itemset to result ( Lk )
		}
	}
	freq.clear();
	freq = result; //update freq with the current result ( Lk )
}

/* Process input from input file */
int processinput()
{
	string line, column_header;
	vector<string> entry;
	ifstream file;
	file.open("FILE1.txt"); //input file

	if( file.is_open() == false ){ //if there is error in opening the file
		cout<<"Error while opening input file"<<endl;
		return 0;
	}

	map< int, string > items; // map< set_bit_number, column header>
	int count = 0, column_position = 0;

	//Reading file line by line
	while( getline(file, line)){ //read only first two lines ( header )
		count++;
		if( count == 1) continue; //skip first line
		column_header = "";
		for(int i = 0; i < (int)line.length(); i++){
			if( line[i] == ',' || line[i+1] == '\0'){ //if a complete column_header is processed or it is last column_header 
				column_position++;
				items[column_position] = column_header;
				column_header = "";
				while( (i+1) < (int)line.length() && line[i+1] == ' ' )
					i++;
			} else
				column_header += line[i]; // add current character to column name
		}
		if( count >= 2) break;
	}
	
	//process rest of file
	count = 0;
	while( getline(file, line) ){
		entry.clear();
		column_position = 0;
		for(int i = 0; i < (int)line.length(); i++){
			if( line[i] == '1' || line[i] == '0' ) //increament column position count
				column_position++;
			if( line[i] == ',' || line[i] == ' '|| line[i] == '0') continue; //consider only set bits
			entry.push_back( items[column_position]  ); //only column_header for set bits are pushed (only for FILE1.txt and FILE2.txt)
		}
		dataset[count++] = entry; //count->TID, entry->ITEMSET
	}
	file.close(); //close file
	return count; //return no of transactions ; input file read successfully
}

int main()
{
	int min_per, total_transaction = processinput();
	if( total_transaction == 0 ) //error while reading input file
		return 1;
	
	cout<<"Please enter minimum support % value ";
	cin>>min_per;

	MIN_SUPPORT = ceil(min_per*total_transaction)/100;

	print_dataset(); //to print dataset
	cout<<"\nC1\n";
	calculate_c1();
	print(freq);

	cout<<"\nL1\n";
	calculate_l1();
	print(freq);

	int k = 2;
	while(1){
		calculate_ck(k);
		if( freq.size() == 0 ) {
			cout<<"\nL"<<k<<"\nNo itemset\n";
			break;
		}
		cout<<"\n\nC"<<k<<endl;
		print(freq);

		calculate_lk(k);
		if( freq.size() == 0 ) {
			cout<<"\nL"<<k<<"\nNo itemset\n";
			break;
		}
		cout<<"\nL"<<k<<endl;
		print(freq);
		k++;
	}
	return 0;
}
