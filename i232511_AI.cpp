#include <iostream>
#include <fstream>
#include <string>
using namespace std;
// priority queue for A* search algorithm
class priorityQueue{
    int costs[100];       
    int vertices[100];    
    int size;
    // maintains heap property when a new element is added
    void heapifyUp(int index){
        while(index>0){
            int parent=(index-1)/2;
            if(costs[parent]>costs[index]){
                swap(costs[parent],costs[index]);
                swap(vertices[parent],vertices[index]);
            }
            index=parent;
        }
    }
    // maintains heap property when root element is removed
    void heapifyDown(int index){
        while(true){
            int left=2*index+1;
            int right=2*index+2;
            int smallest=index;
            if(left<size && costs[left]<costs[smallest]){
                smallest=left;
            }
            if(right<size && costs[right]<costs[smallest]){
                smallest=right;
            }
            if(smallest!=index){
                swap(costs[index],costs[smallest]);
                swap(vertices[index],vertices[smallest]);
                index=smallest;
            }
            else{
                break;
            }
        }
    }
public:
    priorityQueue():size(0){}
    void push(int cost,int vertex){
        costs[size]=cost;
        vertices[size]=vertex;
        heapifyUp(size);
        size++;
    }
    int pop(){
        if(size==0){
            return -1;
        }
        int topVertex=vertices[0];
        costs[0]=costs[size-1];
        vertices[0]=vertices[size-1];
        size--;
        heapifyDown(0);
        return topVertex;
    }
    bool isEmpty(){
        return size==0;
    }
};
// Max heap for emergency vehicles and congestion
class MaxHeap{
    string vertices[100];  
    int counts[100];       
    int size;
    void heapifyUp(int index){
        while(index>0){
            int parent=(index-1)/2;
            if(counts[parent]<counts[index]){
                swap(vertices[parent],vertices[index]);
                swap(counts[parent],counts[index]);
            }
            index=parent;
        }
    }
    void heapifyDown(int index){
        while(true){
            int left=2*index+1;
            int right=2*index+2;
            int largest=index;
            if(left<size && counts[left]>counts[largest]){
                largest=left;
            }
            if(right<size && counts[right]>counts[largest]){
                largest=right;
            } 
            if(largest!=index){
                swap(vertices[index],vertices[largest]);
                swap(counts[index],counts[largest]);
                index=largest;
            }
            else{
                break;
            }
        }
    }
public:
    MaxHeap():size(0){}
    void push(string vertex,int count){
        for(int i=0;i<size;i++){
            if(vertices[i]==vertex){
                counts[i]+=count;
                heapifyUp(i);
                return;
            }
        }
        vertices[size]=vertex;
        counts[size]=count;
        heapifyUp(size++);
    }
    string pop(){
        if(size==0){
            return "";
        } 
        string topVertex=vertices[0];
        vertices[0]=vertices[--size];
        counts[0]=counts[size];
        heapifyDown(0);
        return topVertex;
    }
    bool isEmpty(){ 
        return size==0;
    }
};
// Hashtable for storing congestions
class HashNode{
public:
    string key;
    int value;
    HashNode* next;
    HashNode(string key,int value):key(key),value(value),next(NULL){}
};
class HashTable{
    HashNode* table[101];
    //hash function
    int hash(string key){
        int hashValue=0;
        for(int i=0;i<key.length();i++){
            hashValue=(hashValue*31+key[i])%101;
        }
        return hashValue;
    }
public:
    HashTable(){
        for(int i=0;i<101;i++){
            table[i]=NULL;
        }
    }
    void insert(string key,int value){
        int hashIndex=hash(key);
        HashNode* newNode=new HashNode(key,value);
        newNode->next=table[hashIndex];
        table[hashIndex]=newNode;
    }
    int get(string key){
        int hashIndex=hash(key);
        HashNode* temp=table[hashIndex];
        while(temp){
            if(temp->key==key){
                return temp->value;
            }
            temp=temp->next;
        }
        return 0;
    }
    void update(string key,int value){
        int hashIndex=hash(key);
        HashNode* temp=table[hashIndex];
        while(temp){
            if(temp->key==key){
                temp->value+=value;
                return;
            }
            temp=temp->next;
        }
        insert(key,value);
    }
    void display(){
        for(int i=0;i<101;i++){
            HashNode* temp=table[i];
            while(temp){
                cout<<temp->key<<":"<<temp->value<<endl;
                temp=temp->next;
            }
        }
    }
};
class Node{
public:
    Node* next;
    string vertex;
    int weight;
    int greenTime;
    Node(int weight, string vertex):weight(weight),vertex(vertex),next(NULL),greenTime(0){}
};
//linked list class
class List{
public:
    Node* head;
    List():head(NULL){}
    void insert(string vertex,int weight){
        Node* newNode=new Node(weight,vertex);
        if(head==NULL){
            head=newNode;
        } 
        else{
            Node* temp=head;
            while(temp->next!=NULL){
                temp=temp->next;
            } 
            temp->next=newNode;
        }
    }
    void display(string vertex){
        if(head==NULL){
            cout<<"Empty"<<endl;
            return;
        }
        cout<<vertex<<"->";
        Node* temp=head->next;
        while(temp!=NULL){
            cout<<"("<<temp->vertex<< ","<<temp->weight<< ") ";
            temp=temp->next;
        }
        cout<<endl;
    }
};
// main graph class
class Graph{
    List adjList[100];
    string vertices[100];
    int count,distances[100],parent[100];
    bool visited[100];
    string vehicles[100][2];
    string blocked[100][2];
    int blockedCount;
    int vehicleCountTotal;
    HashTable roadCongestion;
    void reversePath(int path[],int pathSize){
        for(int i=0;i<pathSize/2;i++){
            swap(path[i],path[pathSize-1-i]);
        }
    }
    // function for finding the index of the given vertex in the adjacency list
    int findVertex(string vertex){
        for(int i=0;i<count;i++){
            if(adjList[i].head->vertex==vertex){
                return i;
            }
        }
        return -1;
    }
    int mapPriority(string priority) {
        if(priority=="High"){
            return 2;
        }
        if(priority=="Medium"){
            return 1;
        }
        return 0;
    }
    // heuristic function for A* algorithm
    int heuristic(int start,int end){
        return abs(start-end);
    }
public:
    Graph():count(0),vehicleCountTotal(0),blockedCount(0){}
    // reading csv files
    void readNetworkCSV(){
        string start,end,weight;
        ifstream readFile("road_network.csv");
        getline(readFile,weight);
        while(getline(readFile,start,',')){
            getline(readFile,end,',');
            getline(readFile,weight);
            insertEdge(start,end,stoi(weight));
            insertEdge(end,start,stoi(weight));
        }
        readFile.close();
    }
    void readTrafficSignals(){
        string vertex;
        string greenTime;
        ifstream readFile("traffic_signals.csv");
        getline(readFile,vertex);
        while(getline(readFile,vertex,',')){
            getline(readFile,greenTime);
            int index=findVertex(vertex);
            adjList[index].head->greenTime=stoi(greenTime);
        }
    }
    void readVehicles(){
        string Id,start,end;
        ifstream readFile("vehicles.csv");
        getline(readFile,Id);
        while(getline(readFile,Id,',')){
            getline(readFile,start,',');
            getline(readFile,end);
            vehicles[vehicleCountTotal][0]=start;
            vehicles[vehicleCountTotal][1]=end;
            vehicleCountTotal++;
        }
        readFile.close();
    }
    void readEmergencyVehicles(){
        string Id,start,end,priority;
        MaxHeap emergency;
        ifstream readFile("emergency_vehicles.csv");
        getline(readFile,Id);
        while(getline(readFile,Id,',')){
            if(Id==""){
                break;
            }
            getline(readFile,start,',');
            getline(readFile,end,',');
            getline(readFile,priority);
            emergency.push(start+end,mapPriority(priority));
        }
        readFile.close();
        while(!emergency.isEmpty()){
            string full=emergency.pop();
            string start=string(1,full[0]);
            string end=string(1,full[1]);
            cout<<"\nMoving emergency vehicle from "<<start<<" to "<<end<<endl;
            emergencyVehicleRouting(start,end);
        }
    }
    // function for blocking roads by removing the edges
    void blockRoad(string start,string end){
        removeEdge(start,end);
        removeEdge(end,start);
        blocked[blockedCount][0]=start;
        blocked[blockedCount][1]=end;
        blockedCount++;
    }
    void readClosures(){
        string start,end,status;
        ifstream readFile("road_closures.csv");
        while(getline(readFile,start,',')){
            if(start==""){
                break;
            }
            getline(readFile,end,',');
            getline(readFile,status);
            if(status=="Blocked"){
                blockRoad(start,end);
            }
        }
        readFile.close();
    }
    void insertEdge(string start,string end,int weight){
        int startIndex=findVertex(start);
        if(startIndex==-1){
            adjList[count].insert(start,0);
            vertices[count]=start;
            startIndex=count++;
        }
        adjList[startIndex].insert(end,weight);
    }
    void removeEdge(string start,string end){
        int startIndex=findVertex(start);
        if(startIndex==-1){
            return;
        }
        Node* temp=adjList[startIndex].head;
        Node* prev=NULL;
        while(temp!=NULL){
            if(temp->vertex==end){
                if(prev==NULL){
                    adjList[startIndex].head=temp->next;
                }
                else{
                    prev->next=temp->next;
                }
                delete temp;
                return;
            }
            prev=temp;
            temp=temp->next;
        }
    }
    void updateRoadCongestion(string road,int value){
        roadCongestion.update(road,value);
    }
    // Main simulation function
    void simulateTraffic(){
        int paths[100][100],pathSizes[100];
        MaxHeap congestionHeap;
        for(int i=0;i<vehicleCountTotal;i++){
            int startIndex=findVertex(vehicles[i][0]);
            int endIndex=findVertex(vehicles[i][1]);
            dijkstra(startIndex,endIndex,paths[i],pathSizes[i]);
        }
        bool simulationRunning=true;
        while(simulationRunning){
            simulationRunning=false;
            for(int i=0;i<vehicleCountTotal;i++){
                if(pathSizes[i]>0){
                    simulationRunning=true;
                    int current=paths[i][0];
                    int next=-1;
                    if(pathSizes[i]>1){
                        next=paths[i][1];
                    }
                    if(next!=-1){
                        string road=vertices[current]+"->"+vertices[next];
                        updateRoadCongestion(road,1);
                        congestionHeap.push(vertices[next],1);
                        cout<<"Vehicle "<<i+1<<" is moving from "<<vertices[current]<<" to "<<vertices[next]<<endl;
                        for(int j=0;j<pathSizes[i]-1;j++){
                            paths[i][j]=paths[i][j+1];
                        }
                        pathSizes[i]--;
                    } 
                    else{
                        cout<<"Vehicle "<<i+1<<" has reached its destination!"<<endl;
                        pathSizes[i]=0;
                    }
                }
            }
            if(!congestionHeap.isEmpty()){
                string mostCongested=congestionHeap.pop();
                cout<< "Green signal given to vertex: "<<mostCongested<<endl;
                while(!congestionHeap.isEmpty()){
                    string nextVertex=congestionHeap.pop();
                    cout<<"Next green signal given to vertex: "<<nextVertex<<endl;
                }
            }
            cout<<"----------------------------------"<<endl;
        }
    }
    void displayCongestion(){
        cout<<"Road Congestion Data:"<<endl;
        roadCongestion.display();
    }
    void dijkstra(int startIndex,int endIndex,int path[],int& pathSize){
        for(int i=0;i<100;i++){
            distances[i]=30000;
            visited[i]=false;
            parent[i]=-1;
        }
        distances[startIndex]=0;
        for(int i=0;i<count;i++){
            int minDistance=30000,currentIndex=-1;
            for(int j=0;j<count;j++){
                if(!visited[j] && distances[j]<minDistance){
                    minDistance=distances[j];
                    currentIndex=j;
                }
            }
            if(currentIndex==-1){
                break;
            }
            visited[currentIndex]=true;
            Node* temp=adjList[currentIndex].head->next;
            while(temp!=NULL){
                int neighborIndex=findVertex(temp->vertex);
                if(!visited[neighborIndex] && distances[neighborIndex]>distances[currentIndex]+temp->weight){
                    distances[neighborIndex]=distances[currentIndex]+temp->weight;
                    parent[neighborIndex]=currentIndex;
                }
                temp=temp->next;
            }
        }
        if(distances[endIndex]==30000){
            cout<<"No path exists from "<<vertices[startIndex]<<" to "<<vertices[endIndex]<<endl;
            pathSize=0;
            return;  
        }
        pathSize=0;
        int current=endIndex;
        while(current!=-1){
            path[pathSize++]=current;
            current=parent[current];
        }
        reversePath(path,pathSize);
    }
    void printAllPaths(string start,string end){
        int sourceIndex=findVertex(start);
        int destinationIndex=findVertex(end);
        bool visited[100]={false};
        string path[100];
        int pathLength=0;
        int pathCount=0;
        cout<<"All paths from "<<start<<" to "<<end<<endl;
        dfsAllPaths(sourceIndex,destinationIndex,visited,path,pathLength,0,pathCount);
        if(pathCount==0){
            cout<<"No paths exist from "<<start<<" to "<<end<<endl;
        }
    }
    // recursive function for printing all the paths between given start and end vertices
    void dfsAllPaths(int current,int destination,bool visited[],string path[],int& pathLength,int weight,int& pathCount){
        visited[current]=true;
        path[pathLength++]=vertices[current];
        if(current==destination){
            pathCount++;
            cout<<"Path "<<pathCount<<": ";
            for(int i=0;i<pathLength;i++){
                cout<<path[i];
                if(i!=pathLength-1){
                    cout<< " -> ";
                } 
            }
            cout<<" | Total Weight: "<<weight<<endl;
        } 
        else{
            Node* temp=adjList[current].head->next;
            while(temp!=NULL){
                int neighborIndex=findVertex(temp->vertex);
                if(!visited[neighborIndex]){
                    dfsAllPaths(neighborIndex,destination,visited,path,pathLength,weight+temp->weight,pathCount);
                }
                temp=temp->next;
            }
        }
        visited[current]=false;
        pathLength--;
    }
    // function for displaying road network
    void displayNetwork(){
        for(int i=0;i<count;i++){
            adjList[i].display(vertices[i]);
        }
    }
    void displayBlockedRoads(){
        for(int i=0;i<blockedCount;i++){
            cout<<"Road "<<blocked[i][0]<<" to "<<blocked[i][1]<<" is blocked"<<endl;
        }
    }
    void displaySignalStatus(){
        for(int i=0;i<count;i++){
            cout<<"Intersection "<<adjList[i].head->vertex<<" Green time "<<adjList[i].head->greenTime<<endl;
        }
    }
    // A* search Algorithm
    void emergencyVehicleRouting(string start,string end){
        int startIndex=findVertex(start);
        int endIndex=findVertex(end);
        if(startIndex==-1 || endIndex==-1){
            cout<<"Invalid start or end vertex!"<<endl;
            return;
        }
        priorityQueue pq;
        int g[100],h[100],f[100],parent[100];
        bool visited[100]={false};
        for(int i=0;i<count;i++){
            g[i]=30000;  
            h[i]=heuristic(i,endIndex);      
            f[i]=30000;  
            parent[i]=-1;
        }
        g[startIndex]=0;
        f[startIndex]=g[startIndex]+h[startIndex];
        pq.push(f[startIndex],startIndex);
        while(!pq.isEmpty()){
            int current=pq.pop();
            if(current==endIndex){
                break;
            }
            if(visited[current]){
                continue;
            } 
            visited[current]=true;
            Node* temp=adjList[current].head->next;
            while(temp){
                int neighborIndex=findVertex(temp->vertex);
                if(!visited[neighborIndex]){
                    int tentativeG=g[current]+temp->weight;
                    if(tentativeG<g[neighborIndex]){
                        g[neighborIndex]=tentativeG;
                        f[neighborIndex]=g[neighborIndex]+h[neighborIndex];
                        parent[neighborIndex]=current;
                        pq.push(f[neighborIndex],neighborIndex);
                    }
                }
                temp=temp->next;
            }
        }
        int path[100],pathSize=0,current=endIndex;
        while(current!=-1){
            path[pathSize++]=current;
            current=parent[current];
        }
        reversePath(path,pathSize);
        cout<<"Emergency vehicle path: ";
        for(int i=0;i<pathSize;i++){
            cout<<vertices[path[i]];
            if(i==pathSize-1){
                cout<<"";
            }
            else{
                cout<<" -> ";
            }
        }
        cout<<endl;
        for(int i=0;i<pathSize-1;i++){
            string road=vertices[path[i]]+"->"+vertices[path[i+1]];
            cout<<"Clearing path: "<<road<<endl;
        }
        cout<<"Emergency vehicle has reached its destination!"<<endl;
        cout<<"Restoring normal traffic flow..."<<endl;
    }
};
int main(){
    Graph g;
    g.readNetworkCSV();
    g.readVehicles();
    //main loop displaying the menu
    while(true){
        cout<<"1. Display road network"<<endl;
        cout<<"2. Display simulation with road closed"<<endl;
        cout<<"3. Display congestion status"<<endl;
        cout<<"4. Display emergency vehicles routing"<<endl;
        cout<<"5. Display signal statuses"<<endl;
        cout<<"6. Display roads blocked"<<endl;
        cout<<"7. Display All paths"<<endl;
        cout<<"8. Insert or block road"<<endl;
        cout<<"9. Enter custom emergency vehicle"<<endl;
        cout<<"10. Exit"<<endl;
        int choice;
        cin>>choice;
        if(choice==10){
            break;
        }
        switch(choice){
            case 1:
                g.displayNetwork();
                break;
            case 2:
                g.readClosures();
                g.simulateTraffic(); 
                break;
            case 3:
                g.displayCongestion();
                break;
            case 4:
                g.readEmergencyVehicles();
                break;
            case 5:
                g.readTrafficSignals();
                g.displaySignalStatus();
                break;
            case 6:
                g.displayBlockedRoads();
                break;
            case 7:{
                string start,end;
                cout<<"Enter start vertex to find all paths:";
                cin>>start;
                cout<<"Enter end vertex to find all paths:";
                cin>>end;
                g.printAllPaths(start,end);
                break;
            }
            case 8:{
                string choice;
                cout<<"Do you want to insert an road or block a road?";
                cin>>choice;
                if(choice=="insert"){
                    string start,end;
                    int weight;
                    cout<<"Enter starting vertex:";
                    cin>>start;
                    cout<<"Enter ending vertex:";
                    cin>>end;
                    cout<<"Enter weight:";
                    cin>>weight;
                    g.insertEdge(start,end,weight);
                    cout<<"Edge added"<<endl;
                    g.displayNetwork();
                }
                else if(choice=="block"){
                    string start,end;
                    cout<<"Enter starting vertex:";
                    cin>>start;
                    cout<<"Enter ending vertex:";
                    cin>>end;
                    g.blockRoad(start,end);
                    cout<<"Road blocked"<<endl;
                    g.displayNetwork();
                }
                break;
            }
            case 9:{
                string start,end;
                cout<<"Enter emergency vehicles start:";
                cin>>start;
                cout<<"Enter emergency vehicles end:";
                cin>>end;
                g.emergencyVehicleRouting(start,end);
                break;
            }
            default:
                cout<<"Invalid choice"<<endl;
        }
    }
    return 0;
}