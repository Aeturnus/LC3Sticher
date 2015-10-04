#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <sstream>


#define NOFILES -1
#define SUCCESS 0
#define NOTVALID 1

using namespace std;

struct ObjectFile
{
    string name;
    uint16_t entryPoint;
};



uint16_t processObjectFile(ostream& stream, ObjectFile& objectFile, uint16_t last);
void sortObjectFiles(vector<struct ObjectFile*>& objectFiles);
bool readBigEndian16(istream& stream,uint16_t* num);
void writeBigEndian16(ostream& stream,uint16_t* num);
//lc3st (start)-s file1 file2 file3 fileN
int main(int argc, char *argv[])
{
    //vector<string> fileNames;
    //vector<uint16_t> entryPoints;
    vector<struct ObjectFile*> objectFiles;

    struct ObjectFile *startFile = nullptr;

    string outName;                     //Output filename
    ofstream fileOut;                     //Output stream;
    vector<uint16_t> outVector;         //What we'll be outputting

    cout << "Hello, World!" << endl;
    cout << "Object code to be stiched: "<<endl;

    if(argc == 1)
    {
        cout << "No files provided" <<endl;
        return NOFILES;
    }


    string startName = "";
    bool startSet = false;
    bool libMode = false;   //Disables at start jumping to get to entry point
    bool outputSet = false;
    string tempString;
    struct ObjectFile* curObj;
    ifstream fileIn;
    for(int i = 1; i < argc; i++)
    {
        //string tempString (argv[i]);
        tempString  = argv[i];

        if(tempString.compare("-s") == 0 && !startSet)
        {
            startSet = true;
            if(i == argc -1)
            {
                cout <<"Start file not provided after flag!"<<endl;
                return NOFILES;
            }
            startName = argv[++i];
        }
        else if (tempString.compare("-l") == 0 && !libMode)
        {
            libMode = true;
        }
        else if  (tempString.compare("-o") == 0 && !outputSet)
        {
            outputSet = true;
            if(i == argc -1)
            {
                cout <<"No output file given"<<endl;
                return NOFILES;
            }

            outName = argv[++i];        //Grab next string and then skip it in the iteration
            fileOut.open(outName);
        }
        else
        {
            fileIn.open(tempString,ios::binary|ios::in);    //Open file in binary mode
            if(fileIn.is_open())
            {
                curObj = new struct ObjectFile;
                curObj->name = tempString;
                //fileIn.read((char*)&(curObj->entryPoint),2);     //Read the entry point in
                readBigEndian16(fileIn,&(curObj->entryPoint));
                objectFiles.push_back(curObj);
                fileIn.close();
                /*
                if (startSet == true)
                {
                    startSet = false;
                    startFile = objectFiles[objectFiles.size()-1];     //startFile points to the object now
                }
                 */
                cout << curObj->name<< " at " <<hex<<"0x"<<curObj->entryPoint<< endl;
            }
            else
            {
                cout<<tempString<<" is not a valid file!"<<endl;
                return NOTVALID;
            }
        }
    }
    if(objectFiles.size() == 0)
    {
        cout<<"No valid files!"<<endl;
        return NOFILES;
    }
    //Find the start struct
    for(int i = 0; i < objectFiles.size(); i++)
    {
        if(startName.compare(objectFiles[i]->name) == 0)
        {
            startFile = objectFiles[i];
        }
    }
    //Sort the object files by entry point
    sortObjectFiles(objectFiles);
    if(startFile == nullptr)
        startFile = objectFiles[0];

    cout<<"File's entry point to be used: "<<startFile->name<<" at "<<hex<<"0x"<<startFile->entryPoint<<endl;
    if(outName.size() == 0)
    {
        outName = "a.obj";      //If no output filename provided, give one here. Callback to a.out
    }




    #define JUMPSIZE 3; //Number of addresses the jump will take

    fileOut.open(outName, ios::binary | ios::out);
    if(!fileOut.is_open())
    {
        cout<<"Cannot open output file "<<outName<<endl;
        return NOTVALID;
    }
    if(libMode || startFile == objectFiles[0])
    {
        if(libMode)
        {
            cout << "Library mode engaged: no load-jump to start address will be inserted. " << hex << "0x" << objectFiles[0]->entryPoint << " will be the start address" << endl;
        }
        writeBigEndian16(fileOut,&startFile->entryPoint);
    }
    else
    {
        uint16_t loadAddr= objectFiles[0]->entryPoint - (uint16_t)JUMPSIZE;
        cout << "Start address is not the same as the earliest object file origin. A load-jump to the start address will be inserted. The new loaded address will be "<<hex<<" 0x"<<loadAddr<<endl;
        uint16_t instLoad = 0x2001;
        uint16_t instJump = 0xC000;

        writeBigEndian16(fileOut,&loadAddr);    //Starting address, not an instruction
        writeBigEndian16(fileOut,&instLoad);
        writeBigEndian16(fileOut,&instJump);
        writeBigEndian16(fileOut,&startFile->entryPoint);       //Load the start address to jump to it
    }

    uint16_t last = 0;
    for(int i = 0; i < objectFiles.size(); i++)
    {
        last = processObjectFile(fileOut,*objectFiles[i],last);
    }
    fileOut.close();
    cout<<"Complete!"<<endl;
    cout<<"Outputted to "<<outName<<endl;
    return SUCCESS;

}

//Returns the address that it leaves off on
uint16_t processObjectFile(ostream& stream, ObjectFile& objectFile, uint16_t last)
{
    uint16_t data;
    ifstream fileIn;
    uint16_t start = objectFile.entryPoint;
    uint16_t addr = objectFile.entryPoint;      //The final address given
    cout<<"Processing "<<objectFile.name<<"..."<<endl;
    fileIn.open(objectFile.name,ios::binary|ios::in);
    readBigEndian16(fileIn,&start);  //Discard this word; it's just the start address

    if(last != 0)
    {
        uint16_t zero = 0x00;
        for (int i = start - last; i > 0; i--)
        {
            writeBigEndian16(stream, &zero);     //zero fill until start
        }
    }

    //Read every word then output it
    while(readBigEndian16(fileIn,&data))
    {
        writeBigEndian16(stream,&data);
        addr++;
    }

    return addr;
}

//false if eof hit
bool readBigEndian16(istream& stream,uint16_t* num)
{
    char* msb = ((char*)num)+1;
    char* lsb = ((char*)num);
    stream.read(msb,1);
    stream.read(lsb,1);
    return !stream.eof();
}
void writeBigEndian16(ostream& stream,uint16_t* num)
{
    char* msb = ((char*)num)+1;
    char* lsb = ((char*)num);
    stream.write(msb,1);
    stream.write(lsb,1);
}

void objSwap(int indexU, int indexV, vector<struct ObjectFile*>& objectFiles)
{
    struct ObjectFile* s1 = objectFiles[indexU];
    struct ObjectFile* s2 = objectFiles[indexV];
    objectFiles[indexU] = s2;
    objectFiles[indexV] = s1;
}

void sortObjectFiles(vector<struct ObjectFile*>& objectFiles)
{
    int mindex;
    if(objectFiles.size() <=1 )
        return;
    for(int i = 0; i < objectFiles.size(); i++)
    {
        mindex = i;
        for(int j = i+1; j < objectFiles.size(); j++)
        {
            if(objectFiles[j]->entryPoint < objectFiles[mindex]->entryPoint)
            {
                mindex = j;
            }
        }
        if(i != mindex)
            objSwap(i,mindex,objectFiles);
    }
}

//Includes the ".obj"
void copySymbolTable(string startName, string outName)
{
    /*
    string input= startName;
    string output= outName;
    char* iopointer = &(input.c_str()[input.length() - 3]);
    char* oopointer = &(output.c_str()[output.length() - 3]);
    *iopointer = 's';
    *oopointer = 's';
    iopointer++;
    oopointer++;
    *iopointer = 'y';
    *oopointer = 'y';
    iopointer++;
    oopointer++;
    *iopointer = 'm';
    *oopointer = 'm';
    iopointer++;
    oopointer++;
    */

    startName.replace(startName.find_last_of('.'),4,".sym");
    outName.replace(startName.find_last_of('.'),4,".sym");
    ifstream fileIn;
    ofstream fileOut;
}
