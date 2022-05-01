#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm>
#include<string>
#include<cmath>

using namespace std;

void splitStr2Vec(string str, vector<string>& buf)
{
	  int current = 0; //initial position
	  int next;
 
	  while (true)
	  {
	      next = str.find_first_of(" ", current);
		    if (next != current)
		    {
			      string tmp = str.substr(current, next - current);
			      if (tmp.size() != 0)   buf.push_back(tmp);
        }
		    if (next == string::npos) break;
		    current = next + 1; 
    }
}

//class cell
class Cell{
public:
    vector<int> net_connect;
    int locked;
    int part;
    int gain;
    Cell(){
        locked = 0;
        part = 0;
        gain = 0;
    }
	void print_celllist(vector<Cell> &celllist){
     for(int i=0;i<celllist.size();++i){
        cout << "cell"<<i+1<<": "<<endl;
        for(int j=0;j<celllist[i].net_connect.size();++j){
            cout <<celllist[i].net_connect[j]<<" ";
        }
        cout <<"gain:"<<celllist[i].gain<<endl;
        cout <<"part:"<<celllist[i].part<<endl;
        cout <<endl;
    }
}
};

//class net
class Net{
public:
    vector<int> cell_connect;
    //part1:0,part2:1
    int part1;
    int part2;
	
	void print_netlist(vector<Net> &netlist)
	{
		for(int i=0;i<netlist.size();++i){
			cout << "net "<<i+1<<": "<<endl;
			for(int j=0;j<netlist[i].cell_connect.size();++j){
				cout << netlist[i].cell_connect[j]<<" ";
			}
			cout << endl;
		}

	}
};

class Bucket{
public:
    int cell_num;
    Bucket* next_ptr;
};

int part1(vector<Net> &netlist,vector<Cell> &celllist,int i);
void splitStr2Vec(string str, vector<string>& buf); 
int part2(vector<Net> &netlist,vector<Cell> &celllist,int i);
void compute_gain(vector<Net> &netlist,vector<Cell> &celllist);
bool balance_or_not(vector<Cell> &celllist);
void push(Bucket* bucketlist, int gain_pmax, int &vertex,int& offset);
void remove(Bucket* bucketlist, int &vertex);
void insert(Bucket* bucketlist, int &vertex);
void Delete(Bucket* bucketlist,int size);
int find_and_check_buc(Bucket* bucketlist,int bucket_size);

int part1(vector<Net> &netlist,vector<Cell> &celllist,int i){
    int num = 0;
    for(int j=0;j<netlist[i].cell_connect.size();++j){
        if(celllist[netlist[i].cell_connect[j]].part == 0) ++num;
    }
    return num;
}

int part2(vector<Net> &netlist,vector<Cell> &celllist,int i){
    int num = 0;
    for(int j=0;j<netlist[i].cell_connect.size();++j){
        if(celllist[netlist[i].cell_connect[j]].part == 1) ++num;
    }
    return num;
}

void push(Bucket* bucketlist, int gain_pmax, int &vertex)
{

	if (bucketlist[gain_pmax].cell_num == -1){
		bucketlist[gain_pmax].cell_num = vertex;
	}
	else{
		Bucket* ptr;
		Bucket* now;
		now = new Bucket;
		now->cell_num = vertex;
		now->next_ptr = NULL;
		ptr = &bucketlist[gain_pmax];
		while (ptr->next_ptr != NULL){
			ptr = ptr->next_ptr;
		}
		ptr->next_ptr = now;
	}
}

void insert(Bucket* bucketlist,int gain_pmax,int &vertex)
{
	if (bucketlist[gain_pmax].cell_num == -1)
	{
		bucketlist[gain_pmax].cell_num = vertex;
		return;
	}
	else
	{
		Bucket* ptr;
		ptr = &bucketlist[gain_pmax];
		Bucket* now;
		now = new Bucket;
		if (vertex < ptr->cell_num)//開頭
		{
			now->cell_num = ptr->cell_num;
			now->next_ptr = ptr->next_ptr;
			ptr->cell_num = vertex;
			ptr->next_ptr = now;
			return;
		}
		now->cell_num = vertex;
		now->next_ptr = NULL;
   
		bool flag = false;
		while (ptr->next_ptr != NULL)
		{
			if ((ptr->next_ptr)->cell_num > vertex)
			{
				now->next_ptr = ptr->next_ptr;
				ptr->next_ptr = now;
				flag = true;
				return;
			}
			ptr = ptr->next_ptr;
		}
		ptr->next_ptr = now;
	}
	return;
}

void remove(Bucket* bucketlist,int gain_pmax, int &vertex)
{
	if (bucketlist[gain_pmax].next_ptr == NULL)
	{
		if (bucketlist[gain_pmax].cell_num == vertex)
		{
			bucketlist[gain_pmax].cell_num = -1;
			return;
		}

	}
	else
	{
		Bucket* now;
		Bucket*temp;
		now = &bucketlist[gain_pmax];
		if (now->cell_num == vertex)
		{
			now->cell_num = now->next_ptr->cell_num;
			now->next_ptr = now->next_ptr->next_ptr;
			return;
		}
		else
		{
			while (now->next_ptr != NULL)
			{
				if (now->next_ptr->cell_num == vertex)
				{
					temp = now->next_ptr;
					now->next_ptr = now->next_ptr->next_ptr;
					return;
				}
				now = now->next_ptr;
			}
		}

	}
	return;
}

void Delete(Bucket* bucketlist, int size)
{
	Bucket* now,*next;
	for (int i = 0; i < size; i++)
	{
		if (bucketlist[i].cell_num!=-1)
		{
			bucketlist[i].cell_num = -1;
			now = bucketlist[i].next_ptr;
			while (now != NULL)
			{
				next = now->next_ptr;
				delete now;
				now = next;
			}
			bucketlist[i].next_ptr = NULL;
		}
	}
}


void compute_gain(vector<Net> &netlist,vector<Cell> &celllist){
    int net;
    
    for(int i=0;i<netlist.size();++i){
        netlist[i].part1 = part1(netlist,celllist,i);
        netlist[i].part2 = part2(netlist,celllist,i);
    }
    
    for(int i=0;i<celllist.size();++i){
        celllist[i].gain = 0;
        celllist[i].locked = -1;
        if(celllist[i].part == 1){
            for(int j=0;j<celllist[i].net_connect.size();++j){
                net = celllist[i].net_connect[j];
                //F=1,T=0
                if(netlist[net].part2 == 1)  ++celllist[i].gain; 
                if(netlist[net].part1 == 0)  --celllist[i].gain;
            }
        }
        else{
            for(int j=0;j<celllist[i].net_connect.size();++j){
                net = celllist[i].net_connect[j];
                //F=1,T=0
                if(netlist[net].part1 == 1)  ++celllist[i].gain;
                if(netlist[net].part2 == 0)  --celllist[i].gain;
            }
        }
    }
}

void balance_or_not(vector<Cell> &celllist,int &min,int &max,int &offset){
    bool flag = false;
    if (celllist.size() > 10000)
		    offset = 9;
    else
		    offset = 99;
    if (celllist.size() < 20)
    {
		    if (celllist.size() % 2 == 0)
		    {
			      flag = true;
			      min = celllist.size() / 2 - 1;
			      max = celllist.size() / 2 + 1;
		    }
		    else
		    {
			      min = celllist.size() / 2;
			      max = celllist.size() / 2 + 1;
		    }
    }
	else
	{
		min = ceil(0.45*celllist.size());
		max = floor(0.55*celllist.size());
    }
}

void update_gain(vector<Net> &netlist,vector<Cell> &celllist,Bucket* bucketlist,int max_index,int Pmax,int &current_max,int &current_gain){
    int net,ver;
    celllist[max_index].locked = 1;
    remove(bucketlist, celllist[max_index].gain + Pmax, max_index);
    current_gain += celllist[max_index].gain;
	
    //refresh gain before the move:to
    if(celllist[max_index].part == 1){
        for(int i=0;i<celllist[max_index].net_connect.size();++i){
            net = celllist[max_index].net_connect[i]-1;
            //cout << net <<endl;
            //cout <<netlist[net].part1<<" "<<netlist[net].part2<<endl;
        
            if(netlist[net].part1 == 0) { //t=0
                for(int j=0;j<netlist[net].cell_connect.size();++j){
                    ver = netlist[net].cell_connect[j]-1;
                    //cout <<celllist[netlist[net].cell_connect[j]-1].gain<<endl;
                    if(celllist[ver].locked!=1){
                        remove(bucketlist, celllist[ver].gain + Pmax, ver);
                        ++celllist[ver].gain;
                        insert(bucketlist,celllist[ver].gain + Pmax,ver);
						if(celllist[ver].gain + Pmax > current_max) current_max = celllist[ver].gain + Pmax;
                        //cout <<celllist[netlist[net].cell_connect[j]-1].gain<<endl;
                    }
                }
            }
            else if(netlist[net].part1 == 1){ // t=1
                for(int j=0;j<netlist[net].cell_connect.size();++j){
                    ver = netlist[net].cell_connect[j]-1;
                    if(celllist[ver].locked!=1){
                        //cout <<celllist[netlist[net].cell_connect[j]-1].gain<<endl;
                        if(celllist[ver].part == 0){
                            remove(bucketlist, celllist[ver].gain + Pmax, ver);
                            --celllist[ver].gain;
                            insert(bucketlist,celllist[ver].gain + Pmax,ver);
							if(celllist[ver].gain + Pmax > current_max) current_max = celllist[ver].gain + Pmax;
                        }
                    }
                }
            }
            ++netlist[net].part1;
            --netlist[net].part2;
            if(netlist[net].part2 == 0) { //F=0
                for(int j=0;j<netlist[net].cell_connect.size();++j){
                    ver = netlist[net].cell_connect[j]-1;
                    if(celllist[ver].locked!=1){
                        remove(bucketlist, celllist[ver].gain + Pmax, ver);
                        --celllist[ver].gain;
                        insert(bucketlist,celllist[ver].gain + Pmax,ver);
						if(celllist[ver].gain + Pmax > current_max) current_max = celllist[ver].gain + Pmax;
                    }  
                }
            }
            else if(netlist[net].part2 == 1){ // F=1
                for(int j=0;j<netlist[net].cell_connect.size();++j){
                    ver = netlist[net].cell_connect[j]-1;
                    if(celllist[ver].locked!=1){
                        if(celllist[ver].part == 1){
                            remove(bucketlist, celllist[ver].gain + Pmax, ver);
                            ++celllist[ver].gain;
                            insert(bucketlist,celllist[ver].gain + Pmax,ver);
							if(celllist[ver].gain + Pmax > current_max) current_max = celllist[ver].gain + Pmax;
                        }
                    }
                }
            }
        }    
    }
    else{  //part = 0
        for(int i=0;i<celllist[max_index].net_connect.size();++i){
            //cout<<celllist[max_index].net_connect.size()<<endl;
            net = celllist[max_index].net_connect[i]-1;
            //cout << net <<endl;
            //cout <<netlist[net].part1<<" "<<netlist[net].part2<<endl;
            if(netlist[net].part2 == 0) { //t=0
                for(int j=0;j<netlist[net].cell_connect.size();++j){
                    ver = netlist[net].cell_connect[j]-1;
                    //cout << celllist[netlist[net].cell_connect[j]-1].locked<<endl;
                    if(celllist[ver].locked!=1){
                        //cout <<"t=0 "<<celllist[netlist[net].cell_connect[j]-1].gain<<endl;
                        remove(bucketlist, celllist[ver].gain + Pmax, ver);
                        ++celllist[ver].gain;
                        insert(bucketlist,celllist[ver].gain + Pmax,ver);    
						if(celllist[ver].gain + Pmax > current_max) current_max = celllist[ver].gain + Pmax;						
                    }
                }
            }
            else if(netlist[net].part2 == 1){ // t=1
                for(int j=0;j<netlist[net].cell_connect.size();++j){
                    ver = netlist[net].cell_connect[j]-1;
                    if(celllist[ver].locked!=1){
                         //cout <<"t=1 "<<celllist[netlist[net].cell_connect[j]-1].gain<<endl;
                        if(celllist[ver].part == 1){
                            remove(bucketlist, celllist[ver].gain + Pmax, ver);
                            --celllist[ver].gain;
                            insert(bucketlist,celllist[ver].gain + Pmax,ver); 
							if(celllist[ver].gain + Pmax > current_max) current_max = celllist[ver].gain + Pmax;
                        }
                    }
                }
            }
            ++netlist[net].part2;
            --netlist[net].part1;
            if(netlist[net].part1 == 0) { //F=0
                for(int j=0;j<netlist[net].cell_connect.size();++j){
                    ver = netlist[net].cell_connect[j]-1;
                    if(celllist[ver].locked!=1){
                        remove(bucketlist, celllist[ver].gain + Pmax, ver);
                        --celllist[ver].gain;
                        insert(bucketlist,celllist[ver].gain + Pmax,ver); 
						if(celllist[ver].gain + Pmax > current_max) current_max = celllist[ver].gain + Pmax;
                    }
                }
            }
            else if(netlist[net].part1 == 1){ // F=1
                for(int j=0;j<netlist[net].cell_connect.size();++j){
                    ver = netlist[net].cell_connect[j]-1;
                    if(celllist[ver].locked!=1){
                        if(celllist[ver].part == 0){
                            remove(bucketlist, celllist[ver].gain + Pmax, ver);
                            ++celllist[ver].gain;
                            insert(bucketlist,celllist[ver].gain + Pmax,ver);
							if(celllist[ver].gain + Pmax > current_max) current_max = celllist[ver].gain + Pmax;
                        }
                    }
                }
            }
        }
    }
}

//find the max gain
int find_and_check_buc(Bucket* bucketlist,int bucket_size,int current_max){
    bool flag = false;
    for (int i = current_max; i >= 0; --i)
	  {
		    if (bucketlist[i].cell_num != -1)
		    {
				  //cout<< i<<":" <<bucketlist[i].cell_num<<endl;
			      return bucketlist[i].cell_num; //max gain
		    }
    }
}

int main(int argc,char* argv[])
{
    ifstream infile;
    string instr; 
    
    infile.open(argv[1]);
    ofstream outfile;
    outfile.open("output.txt");    
    
    int line;
    int net_count,node_count,total_pin;
    int total_gain = 0;
    vector<Net> netlist;  //to store the net's connection
    vector<Cell> celllist,max_list;
    Bucket* bucket_list;
    int part1,cell_move,min,max,offset;
	int current_max = -1000000;
	int current_gain = 0;
	int max_gain = -1000000;
    /*------------------------read file-------------------------------------*/   
    if(!infile.is_open()){
        cout << "fail to open" << endl;
    }
    else{
        while(!infile.eof()){ //read in
            vector<string> new_vec;
            getline(infile,instr);
            splitStr2Vec(instr,new_vec);
            //cout << instr << endl;
            ++line;
            
            if(line == 1){
                net_count = stoi(new_vec[0]);
                node_count =  stoi(new_vec[1]);
                netlist.resize(net_count);
                celllist.resize(node_count);
            }
            else if(!new_vec.empty()){
                Net new_net;
                for(int i=0;i<new_vec.size();++i){
					celllist[stoi(new_vec[i])-1].net_connect.push_back(line-1);
                    new_net.cell_connect.push_back(stoi(new_vec[i]));
                }
                netlist[line-2] = new_net;
            }
        }
    }
	/*----------------------------------------------------------------------*/
 
    total_pin = 0;
    for(int i=0;i<netlist.size();++i){
        total_pin += netlist[i].cell_connect.size();
    }
	  
    balance_or_not(celllist,min,max,offset);
    compute_gain(netlist,celllist);
    bucket_list = new Bucket[total_pin*2+1];
    
    for(int i = 0; i < total_pin * 2 + 1; ++i){
		bucket_list[i].cell_num = -1;
		bucket_list[i].next_ptr = NULL;
    }
	
    for(int i=0;i<celllist.size();++i){
        push(bucket_list, total_pin + celllist[i].gain, i);
    }
    
    part1 = celllist.size();
    while(part1 > celllist.size()*0.45){
		if(current_max <= 0){
			cell_move = find_and_check_buc(bucket_list,total_pin*2+1,total_pin*2+1); //find the best gain
		}
		else
		{
			cell_move = find_and_check_buc(bucket_list,total_pin*2+1,current_max); //find the best gain
		}
		//cout << cell_move << endl;
        update_gain(netlist,celllist,bucket_list,cell_move,total_pin,current_max,current_gain);
		
        --part1;
		celllist[cell_move].part = 1;
		if((max_gain < current_gain) && (part1<celllist.size()*0.55)){
			max_gain = current_gain;
			max_list = celllist;
			
		}
		//cout << max_gain <<endl;
    }
	
    for(int i=0;i<celllist.size();++i){
        if(i!=max_list.size()-1)  outfile << max_list[i].part<<endl;
        else outfile << max_list[i].part;
    }
    
    infile.close();
    outfile.close();
    
    return 0;
}
