#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include <iomanip>
#include<deque>

using namespace std;

class Block{
public:
	int leftdown_x;
	int leftdown_y;
	int rightup_x;
	int rightup_y;
};

class Net{
public:
	int source_x;
	int source_y;
	int target_x;
	int target_y;
	deque<int> rout_x;
	deque<int> rout_y;
	int grid_used;
	int manhattan_distance;
};

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

int manhattan_distance(int x1,int y1,int x2,int y2){
	int dis = 0;
	if(x1 > x2){
		dis += x1-x2;
		if(y1 > y2){
			dis += y1-y2;
		}
		else{
			dis += y2-y1;
		}
	}
	else{
		dis += x2-x1;
		if(y1 > y2){
			dis += y1-y2;
		}
		else{
			dis += y2-y1;
		}
	}
	
	return dis;
}

int main(int argc,char* argv[]){
	
	ifstream infile;
    string instr; 
    int count_blk = 0,count_net = 0;
	
	//map information
	int row_length,col_length;
	int current_distance = 0,current_distance2 = 0,sample_distance = 0;
	int x_temp =0,y_temp = 0;
	int xx,yy;
	
	//for backtracking
	int x_next,y_next;
	int smallest = 100000;
	int grid_used = 0;
	int index1,index2;
	bool path_or_not = false;
	
	deque<int> X,Y;
	
	//for net ordering
	deque<int> rerout;
	
	int dr[] ={-1,1,0,0};
	int dc[] ={0,0,1,-1}; 
	//int dr[] ={0,0,-1,1};
	//int dc[] ={-1,1,0,0}; 
	
	vector<Block> blk_vec;
	vector<Net> net_vec;
	
    infile.open(argv[1]);
    ofstream outfile;
    
	outfile.open(argv[2]);  
	
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
			
			if(!new_vec.empty()){
				if(new_vec[0] == ".row"){
					row_length = stoi(new_vec[1]);
				}
				else if(new_vec[0] == ".col"){
					col_length = stoi(new_vec[1]);
				}
				else if(new_vec[0] == ".block"){
					count_blk = stoi(new_vec[1]);
				}
				else if(new_vec[0] == ".net"){
					count_net = stoi(new_vec[1]);
				}
				else{
					if(count_blk >0){
						Block new_blk;
						new_blk.leftdown_x = stoi(new_vec[0]);
						new_blk.leftdown_y = stoi(new_vec[2]);
						new_blk.rightup_x = stoi(new_vec[1]);
						new_blk.rightup_y = stoi(new_vec[3]);
						blk_vec.push_back(new_blk);
						--count_blk;
					}
					else if(count_net > 0){
						Net new_net;
						new_net.source_x = stoi(new_vec[1]);
						new_net.source_y = stoi(new_vec[2]);
						new_net.target_x = stoi(new_vec[3]);
						new_net.target_y = stoi(new_vec[4]);
						net_vec.push_back(new_net);
						--count_net;
					}
				}
			}
        }
    }
	
	/*------------------------initial map setting-------------------------------------*/ 
	int map[row_length][col_length],step[row_length][col_length],step2[row_length][col_length];
	
	int rout_path[row_length][col_length]; //for routing and rerout
	int count = 0;
	
	for(int i=0;i<row_length;++i){
		for(int j=0;j<col_length;++j){
			map[i][j] = 0;
			rout_path[i][j] = 0;
		}
	}
	//we can not go through block
	for(int i=0;i<blk_vec.size();++i){
		for(int x = blk_vec[i].leftdown_x;x<= blk_vec[i].rightup_x;++x){
			for(int y = blk_vec[i].leftdown_y;y<=blk_vec[i].rightup_y;++y){
				map[x][y] = -1;
			}
		}
	}
	
	/*------------------------initial routing-------------------------------------*/ 
	
	//store new_vec into rerout
	for(int i= 0;i < net_vec.size();++i){
		net_vec[i].manhattan_distance = manhattan_distance(net_vec[i].source_x,net_vec[i].source_y,net_vec[i].target_x,net_vec[i].target_y);
	}
	
	//net ordering
	vector <int> sort_index;
	vector <int> sort_dis;
	
	for(int i= 0;i < net_vec.size();++i) {
		sort_index.push_back(i);
		sort_dis.push_back(net_vec[i].manhattan_distance);
	}
	
	int temp,temp_index;
	for(int i= 0;i < sort_dis.size()-1;++i){
		for(int j = i+1; j<sort_dis.size();++j){
			if(sort_dis[j] < sort_dis[i]){
				temp = sort_dis[j];
				sort_dis[j] = sort_dis[i];
				sort_dis[i] = temp;
				temp_index = sort_index[j];
				sort_index[j] = sort_index[i];
				sort_index[i] = temp_index;
			}
		}
	}
	for(int i= 0;i < sort_index.size();++i){
		//cout << sort_index[i]<<endl;
		rerout.push_back(sort_index[i]);
	}
	
	current_distance = manhattan_distance(net_vec[rerout.front()].source_x,net_vec[rerout.front()].source_y,net_vec[rerout.front()].target_x,net_vec[rerout.front()].target_y);
	//cout << "current distance:"<<current_distance<<endl;
	
	deque<int> rerout_index;
	deque<int> rerout_place_x,rerout_place_y;
	
	while(!rerout.empty()){
		int i = rerout.front();
		//cout <<"i:"<< i << endl;
		rerout.pop_front();
		x_temp = net_vec[i].source_x;
		y_temp = net_vec[i].source_y;
		rerout_index.clear();
		rerout_place_x.clear();
		rerout_place_y.clear();
		++count;
		
		X.clear();
		Y.clear();
		X.push_back(x_temp);
		Y.push_back(y_temp);
		
		//reset
		for(int k=0;k<row_length;++k){
			for(int j=0;j<col_length;++j){
				step[k][j] = 0;
			}
		}
		//we can not go through block
		for(int k=0;k<blk_vec.size();++k){
			for(int x = blk_vec[k].leftdown_x;x<= blk_vec[k].rightup_x;++x){
				for(int y = blk_vec[k].leftdown_y;y<=blk_vec[k].rightup_y;++y){
					step[x][y] = 3;
				}
			}
		}
		
		//source target
		for(int j=0;j<net_vec.size();++j){
			step[net_vec[j].source_x][net_vec[j].source_y] = 2;
			step[net_vec[j].target_x][net_vec[j].target_y] = 2;
		}
		
		//print map
		/*
		cout << endl;
		for(int k=0;k<row_length;++k){
			for(int j=0;j<col_length;++j){	
				//cout <<setw(4)<< map[k][j];
				cout <<setw(4)<< step[k][j];
			}
			cout << endl;
		}
		
		for(int k=0;k<row_length;++k){
			for(int j=0;j<col_length;++j){	
				if((map[k][j]!= -1)&&(map[k][j]!=-2)){
					map[k][j] = 0;
				}
			}	
		}	
		*/
		
		while((!((x_temp== net_vec[i].target_x) && (y_temp == net_vec[i].target_y))) && (!X.empty())){
			x_temp = X.front();
			y_temp = Y.front();
			
			X.pop_front();
			Y.pop_front();
			
			current_distance2 = manhattan_distance(x_temp,y_temp,net_vec[i].target_x,net_vec[i].target_y);
			
			//cout << "current distance:"<<current_distance<<endl;
			//cout << "current distance2:"<<current_distance2<<endl;
			path_or_not = false;
			
			//using deque
			for(int j=0;j<4;++j){
				
				if((x_temp<=row_length-dr[j]) && (x_temp >= (-1)*dr[j])) xx = x_temp+dr[j];
				else xx = x_temp;
				
				if((y_temp<=col_length-dc[j]) && (y_temp >= (-1)*dc[j])) yy = y_temp+dc[j];
				else  yy = y_temp;
				
				//cout <<"xx:"<<xx<<",yy:"<<yy <<endl;
				
				if(map[xx][yy] == -2){
					int add = true;
					for(deque<int>::iterator it=rerout_index.begin();it < rerout_index.end(); it++){
						//cout << *it <<endl;
						if( (*it) == rout_path[xx][yy]) add = false;
					}
					if(add && (i !=rout_path[xx][yy]-1)){
						rerout_index.push_back(rout_path[xx][yy]);
						rerout_place_x.push_back(x_temp);
						rerout_place_y.push_back(y_temp);
					}
				}
				else if((map[xx][yy] >= 0) && (step[xx][yy]!=1)&& (step[xx][yy]!=2)&& (step[xx][yy]!=3)){
					path_or_not = true;
					sample_distance = manhattan_distance(xx,yy,net_vec[i].target_x,net_vec[i].target_y);
					//cout << "sample_distance"<<sample_distance<<endl;
					step[xx][yy] = 1;
					if(sample_distance > current_distance2){
						//if()
						//cout << map[x_temp][y_temp] << endl;
						map[xx][yy] = sample_distance-current_distance2+map[x_temp][y_temp];
						//cout << "map"<<map[xx][yy] << endl;
						//cout << sample_distance-current_distance2 << endl;
						//if(map[xx][yy] == -1) map[xx][yy]=0;
						X.push_back(xx);
						Y.push_back(yy);
					}
					else{
						map[xx][yy] = map[x_temp][y_temp];
						X.push_front(xx);
						Y.push_front(yy);
					}
				}
				else if((xx == net_vec[i].target_x) && (yy == net_vec[i].target_y)){
					path_or_not = true;
					sample_distance = manhattan_distance(xx,yy,net_vec[i].target_x,net_vec[i].target_y);
					//cout << "sample_distance"<<sample_distance<<endl;
					if(sample_distance > current_distance2){
						map[xx][yy] = sample_distance-current_distance2+map[x_temp][y_temp];
						//cout << "map"<<map[xx][yy] << endl;
						X.push_back(xx);
						Y.push_back(yy);
					}
					else{
						map[xx][yy] = map[x_temp][y_temp];
						X.push_front(xx);
						Y.push_front(yy);
					}
					xx = x_temp;
					yy = y_temp;
					x_temp = net_vec[i].target_x;
					y_temp = net_vec[i].target_y;
					break;
				}
			}
			
			//when no path to go,how to solve it?
			if((!path_or_not) && X.empty() ){
				//cout <<"path"<<i+1<< ":no path to target"<<endl;
				
				int reset_index = -1;
				for(int j=0;j<4;++j){
					if(map[x_temp+dr[j]][y_temp+dc[j]]==-2){
						reset_index = rout_path[x_temp+dr[j]][y_temp+dc[j]];
						//cout <<"reset index = "<<reset_index<<endl;
						//cout<<"fuck"<<endl;
					}
				}
				//rerout
				if(reset_index != -1){
					int add = true;
					for(deque<int>::iterator it=rerout.begin();it < rerout.end(); it++){
						//cout << *it <<endl;
						//cout << reset_index<<endl;
						if( (*it) == reset_index-1) add = false;
					}
					if(add){
						//cout<<"fuck"<<endl;
						--count;
						X.push_front(x_temp);
						Y.push_front(y_temp);
						net_vec[reset_index-1].rout_x.clear();
						net_vec[reset_index-1].rout_y.clear();
						net_vec[reset_index-1].grid_used = 0;
						rerout.push_back(reset_index-1);
					}
					else{
						//cout<<"fuck2"<<endl;
						X.push_front(x_temp);
						Y.push_front(y_temp);
						net_vec[reset_index-1].rout_x.clear();
						net_vec[reset_index-1].rout_y.clear();
						net_vec[reset_index-1].grid_used = 0;
					}
				}
				else if(!rerout_index.empty()){
					int add = true;
					for(deque<int>::iterator it=rerout.begin();it < rerout.end(); it++){
						//cout << *it <<endl;
						//cout <<rerout_index.front()<< endl;
						if( (*it) == rerout_index.front()-1) add = false;
					}
					if(add){
						//cout<<"fuck"<<endl;
						--count;
						reset_index = rerout_index.front();
						net_vec[reset_index-1].rout_x.clear();
						net_vec[reset_index-1].rout_y.clear();
						net_vec[reset_index-1].grid_used = 0;
						X.push_front(rerout_place_x.front());
						Y.push_front(rerout_place_y.front());
						rerout_index.pop_front();
						rerout_place_x.pop_front();
						rerout_place_y.pop_front();
						rerout.push_back(reset_index-1);
						
					}
					else{
						//cout<<"fuck2"<<endl;
						reset_index = rerout_index.front();
						net_vec[reset_index-1].rout_x.clear();
						net_vec[reset_index-1].rout_y.clear();
						net_vec[reset_index-1].grid_used = 0;
						X.push_front(rerout_place_x.front());
						Y.push_front(rerout_place_y.front());
						rerout_index.pop_front();
						rerout_place_x.pop_front();
						rerout_place_y.pop_front();
					}
				}
				
				for(int j=0;j<row_length;++j){
					for(int k=0;k<col_length;++k){
						if( rout_path[j][k] == reset_index){
							map[j][k] = 0;
							rout_path[j][k] = 0;
						}
					}
				}
				//break;
			}
			/*
			//print map
			cout << endl;
			for(int k=0;k<row_length;++k){
				for(int j=0;j<col_length;++j){	
					cout <<setw(4)<< map[k][j];
					//cout <<setw(4)<< step[k][j];
				}
				cout << endl;
			}
			*/			
		}
		/*
		cout << endl;
		for(int k=0;k<row_length;++k){
			for(int j=0;j<col_length;++j){	
				cout <<setw(4)<< map[k][j];
				//cout <<setw(4)<< step[k][j];
			}
			cout << endl;
		}
		
		cout << endl;
		for(int k=0;k<row_length;++k){
			for(int j=0;j<col_length;++j){	
				//cout <<setw(4)<< map[k][j];
				cout <<setw(4)<< step[k][j];
			}
			cout << endl;
		}
		*/
		
		//backtracking
		grid_used = 0;
		smallest = map[x_temp][y_temp];
		index1 = -1; index2 = -1;
		
		//set backtracking step
		for(int k=0;k<row_length;++k){
			for(int j=0;j<col_length;++j){
				step2[k][j] = 0;
			}
		}
		//we can not go through block
		for(int k=0;k<blk_vec.size();++k){
			for(int x = blk_vec[k].leftdown_x;x<= blk_vec[k].rightup_x;++x){
				for(int y = blk_vec[k].leftdown_y;y<=blk_vec[k].rightup_y;++y){
					step2[x][y] = 1;
				}
			}
		}
		
		//source target
		for(int j=0;j<net_vec.size();++j){
			step2[net_vec[j].source_x][net_vec[j].source_y] = 2;
			step2[net_vec[j].target_x][net_vec[j].target_y] = 2;
		}
		
		//map[x_temp][y_temp] = -2;
		//step2[x_temp][y_temp] = 1;
		//++grid_used;
		rout_path[x_temp][y_temp] = i+1;
		
		net_vec[i].rout_x.push_front(x_temp); //targetx
		net_vec[i].rout_y.push_front(y_temp); //targety
		
		while(!((x_temp== net_vec[i].source_x) && (y_temp == net_vec[i].source_y))){
			//every move
			path_or_not = false;
			map[x_temp][y_temp] = -2;
			++grid_used;
			rout_path[x_temp][y_temp] = i+1; //record each rout
			step2[x_temp][y_temp] = 1;
			index2 = index1;
			int min_man_dis = 10000;
			
			for(int j=0;j<4;++j){
				if((x_temp<=row_length-dr[j]) && (x_temp >= (-1)*dr[j])) xx = x_temp+dr[j];
				else continue;
				
				if((y_temp<=col_length-dc[j]) && (y_temp >= (-1)*dc[j])) yy = y_temp+dc[j];
				else  continue;
				
				if((xx == net_vec[i].source_x) && (yy == net_vec[i].source_y)){
					path_or_not = true;
					smallest = map[xx][yy];
					x_next = xx;
					y_next = yy;
					index1 = j;
					break;
				}
				else if(((map[xx][yy] == smallest)||(map[xx][yy] == smallest-1)) && (map[xx][yy] >= 0)&& (step[xx][yy] == 1)&& (step2[xx][yy] != 1)&& (step2[xx][yy] != 2)){
					int dis = manhattan_distance(xx,yy,net_vec[i].source_x,net_vec[i].source_y);
					if( dis <= min_man_dis){
						min_man_dis = dis;
						path_or_not = true;
						smallest = map[xx][yy];
						x_next = xx;
						y_next = yy;
						index1 = j;
					}
				}
			}
			//up down left right no path,back 
			if(!path_or_not){
				grid_used = grid_used-2; //back
				
				map[x_temp][y_temp] = 0;
				rout_path[x_temp][y_temp] = 0;
				if(index1 == 0){
					x_next = x_temp+dr[1];
					y_next = y_temp+dc[1];
				}
				else if(index1 == 1){
					x_next = x_temp+dr[0];
					y_next = y_temp+dc[0];
				}
				else if(index1 == 2){
					x_next = x_temp+dr[3];
					y_next = y_temp+dc[3];
				}
				else if(index1 == 3){
					x_next = x_temp+dr[2];
					y_next = y_temp+dc[2];
				}
				//net_vec[i].rout_x.pop_front(); 
				//net_vec[i].rout_y.pop_front(); 
			}
		
			//cout <<"smallest:" <<smallest <<endl;
			//cout << "store"<<" ,current grid:"<<grid_used<<endl;
			//cout <<"index1 = "<<index1<<",index2 = "<<index2 <<endl;
			//cout << "("<<x_temp<<","<<y_temp<<")"<<endl << endl;
		
			if((index1 != index2) && (index2!= -1)){
				//cout <<"smallest:" <<smallest <<endl;
				//cout << "store"<<" ,current grid:"<<grid_used<<endl;
				//cout <<"index1 = "<<index1<<",index2 = "<<index2 <<endl;
				//cout << "("<<x_temp<<","<<y_temp<<")"<<endl << endl;
				net_vec[i].rout_x.push_front(x_temp);
				net_vec[i].rout_y.push_front(y_temp);
			}
			x_temp = x_next;
			y_temp = y_next;
			
			//cout << "("<<x_temp<<","<<y_temp<<")"<<endl << endl;
			//print map
			/*
			cout << endl;
			for(int k=0;k<row_length;++k){
				for(int j=0;j<col_length;++j){	
					if((k == net_vec[i].source_x) && (j == net_vec[i].source_y)) cout <<setw(4)<< "S"<<i;
					else if((k == net_vec[i].target_x) && (j == net_vec[i].target_y)) cout <<setw(4)<< "T"<<i;
					else cout <<setw(4)<< map[k][j];
				}
				cout << endl;
			}
			*/			
		}
		map[x_temp][y_temp] = -2;
		rout_path[x_temp][y_temp] = i+1;
		net_vec[i].rout_x.push_front(x_temp);
		net_vec[i].rout_y.push_front(y_temp);
		net_vec[i].grid_used = grid_used-1;
		
		//cout << "store"<<" ,current grid:"<<grid_used-1<<endl;
		//cout <<"index1 = "<<index1<<",index2 = "<<index2 <<endl;
		//cout << "("<<x_temp<<","<<y_temp<<")"<<endl << endl;
		
		for(int k=0;k<row_length;++k){
			for(int j=0;j<col_length;++j){	
				if((map[k][j]!= -1)&&(map[k][j]!=-2)){
					map[k][j] = 0;
				}
			}	
		}	
		/*
		//print map
		cout <<endl<<"i:"<< i <<endl;
		cout << "count:" << count << endl;
		cout << "rerout index size:"<<rerout_index.size() << endl;
		cout << "rerout index front:"<<rerout_index.front() << endl;
		cout << "rerout index back:"<<rerout_index.back() << endl;
		cout << "rerout size:"<<rerout.size() << endl;
		cout << "rerout front:"<<rerout.front() << endl;
		cout << "rerout back:"<<rerout.back() << endl;
		cout << endl;
		for(int k=0;k<row_length;++k){
			for(int j=0;j<col_length;++j){	
				//cout <<setw(4)<< step[k][j];
				cout <<setw(4)<< map[k][j];
				//cout <<setw(4)<< rout_path[k][j];
			}
			cout << endl;
		}
		
		cout << endl;
		for(int k=0;k<row_length;++k){
			for(int j=0;j<col_length;++j){	
				//cout <<setw(4)<< step[k][j];
				//cout <<setw(4)<< map[k][j];
				cout <<setw(4)<< rout_path[k][j];
			}
			cout << endl;
		}
		*/
		if(count == net_vec.size()) {
			break;
		}
	}
	
	//for(int i=0;i<rerout.size();++i) cout <<"rerout index =" <<rerout[i]<<endl;
	
	/*------------------------write file-------------------------------------*/  
	for(int i=0;i<net_vec.size();++i){
		outfile <<"net"<<i+1<<" "<<net_vec[i].grid_used<<endl;
		outfile <<"begin"<<endl;
		
		while(net_vec[i].rout_x.size() != 1){
			outfile <<net_vec[i].rout_x.front()<<" "<<net_vec[i].rout_y.front()<<" ";
			net_vec[i].rout_x.pop_front();
			net_vec[i].rout_y.pop_front();
			outfile <<net_vec[i].rout_x.front()<<" "<<net_vec[i].rout_y.front()<<endl;
		}
		if(i == net_vec.size()-1) outfile <<"end";
		else outfile <<"end"<<endl;
	}
	
	infile.close();
    outfile.close();
	
	return 0;
}
