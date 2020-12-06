#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

int decimalNum(int);
int* binaryNum(int);

int main(){
	string line;
	ifstream inFile;
	inFile.open("data2Vs.txt");

	if(inFile.is_open()){

		//while(getline(inFile,line)){
			getline(inFile, line);
			bool data = false;
			bool generator = false;
			vector<int> dataVec;
			vector<int> generatorVec;


for(unsigned int i = 0; i < line.size(); i++){
            if(!data && !generator){
               if(line[i] == ' '){
                  data = true;
               }
               else{
                  int x;
                  stringstream str;
                  str << line[i];
                  str >> x;
                  dataVec.push_back(x);
               }
            }
            else if(data && !generator){
               if(line[i] == ' '){
                  generator = true;
               }
               else{
                  int x;
                  stringstream str;
                  str << line[i];
                  str >> x;
                  generatorVec.push_back(x);
               }
            }
            else{
               int x;
               stringstream str;
               str << line[i];
               str >> x;
               errorVec.push_back(x);
            }
         }

         	int nData = dataVec.size();
         	int nGenerator = generatorVec.size();
         	//int nError = errorVec.size();
         	int dataArrayChecksum[nData+nGenerator-1];
         	//int generatorArray[nGenerator];
         	//int errorArray[nError];

         	for(int i = 0; i < nData; i++){
            	dataArrayChecksum[i] = dataVec[i];
         	}

         	string dataString;
         	for(int i = 0; i < nData; i++){
            	dataString.push_back(dataArrayChecksum[i] + '0');
         	}

         	int binaryVal = 0;  
         	stringstream tempDataTx(dataString);
         	tempDataTx >> binaryVal;

         	int decimalVal = decimalNum(binaryVal);

         	//Checksum starts
         	//decimal = 291;
         	int quotient = decimalVal/256; //2^8
         	int remainder = decimalVal%256;
         	int sum = quotient + remainder;
         	while(sum > 255){
            	quotient = sum/256; //2^8
            	remainder = sum%256;
            	sum = quotient+remainder;
        	}	

         	int onesComplement = 255-sum;
         	cout<<onesComplement<<endl;

         	int* flippedBinaryArrayTx;   
         	flippedBinaryArrayTx = binaryNum(onesComplement);
         	//Checksum ends
         	/*cout<<"checksum in binary:"<<endl;
         	for(int i = (nGenerator-2); i > = 0; i--){

         	}*/
         	//Add it to the array
         	int counter = nData;
         	for(int i = (nGenerator-2); i >= 0; i--){
            	dataArrayChecksum[counter] = flippedBinaryArrayTx[i];
            	counter++;
         	}



         	for(int i = 0; i < (nData+nGenerator-1); i++){
         		cout<<dataArrayChecksum[i];
         	}
         	cout<<endl;
         	cout<<nData+8<<endl;






		//}



		inFile.close();
	}

   
   return 0;
}



int decimalNum(int n){
   int binaryNum = n;
   int decimalNum = 0;
   int base = 1;

   int tempVal = binaryNum;
   while(tempVal != 0){
      int lastDigit = tempVal%10;
      tempVal = tempVal/10;

      decimalNum = decimalNum + (lastDigit*base);

      base = base * 2;
   }
   return decimalNum;
}

int* binaryNum(int number){
   //array to store binary number that I will return
   static int binaryArray[8];

   //index position in array, starting from last position
   int i = 0;
   while(number > 0){
      binaryArray[i] = number%2;
      number = number/2;
      i++;
   }
   return binaryArray;
}