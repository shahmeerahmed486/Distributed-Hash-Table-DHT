#include<iostream>
#include"InfoStruct.h"

#include"Hashing.h"
#include"RingDHT.h"
using namespace std;

void displayMenu()
{
    cout << endl;
    cout << "__________________________________________________________________________________________" << endl;
    cout << "1.Add a Data in form of key,value" << endl;
    cout << "2.Remove a Data by using key" << endl;
    cout << "3.Print the Routing Table " << endl;
    cout << "4.Print the B-Tree " << endl;
    cout << "5.Add new Machines wihtout disrupting the Functionality" << endl;
    cout << "6.Delete any Machine without disrupting the Functionality of DHT" << endl;
    cout << "7.Display Ring" << endl;
    cout << "8.Exit" << endl;
    cout << endl << "Press...." << endl;
}
int getUserChoice()
{
    int choice = 0;
    cin >> choice;
    return choice;
}
int main()
{   
    cout << "--------------------------------------Welcome to InterPlanetary System-------------------------------" << endl;
    cout << endl;
    cout << "Need some Information to Proceed Further" << endl;
    cout << "Identfier Space Size(in bits)" << endl;
    cout << "Do you want to want to add Id's of Machine manually or Randomly(auto)" << endl;
    cout << "Specify the Number of Initial Numbers of Machine to create" << endl;
    cout << "What would be the Order of B-Tree" << endl;
    cout << endl;
    int IdentifierSpaceSize = 0;
    int ManualOption = -1;
    int totalMachines = 0;
    int orderOfTree = 0;

    cout << "Identifier Space: ";
    cin >> IdentifierSpaceSize;
    cout << "Manual(Press-1)/Auto(Press-0) IDs : ";
    cin >> ManualOption;
    cout << "Btree Order/Degree: ";
    cin >> orderOfTree;
    cout << "Total Machines(to start): ";
    cin >> totalMachines;
    cout << endl;


    RingDHT Ring(IdentifierSpaceSize, orderOfTree, ManualOption);

    for (int i = 0; i < totalMachines; i++)
    {
        Ring.Add_machine();
    }
    Ring.UpdateRouting();

    string Path;
    string keyForDeletion;
    string idForMacDel;
    string idForTree;
    string nameForDel;
    string hashID;
    int choice;
    do {
        displayMenu();
        choice = getUserChoice();

        switch (choice) {
        case 1:

            cout << "Enter the Path of File: " << endl;
            cin >> Path;
            Ring.FileInsertion(Path);
            break;
        case 2:
            cout << "Enter the Key for Deletion: " << endl;
            cin >> keyForDeletion;
            Ring.FileDeletion(keyForDeletion);
            break;
        case 3:
            cout << "Enter the Id for Machine to Print the Routing Table:" << endl;
            cin >> idForTree;
            Ring.printRoutingTable(idForTree);

            break;
        case 4:
            cout << "Enter the Id for Machine to Print the Btree:" << endl;
            cin >> idForTree;
            Ring.printBtree(idForTree);
            break;
        case 5:
            Ring.Add_machine();
            Ring.UpdateRouting();
            break;
        case 6:
            cout << "Enter the Name for Machine" << endl;
            cin >> nameForDel;

            if (!ManualOption)
            {
                idForMacDel = getLastSubstring(Hash(nameForDel), Ring.unique_chars);
                while (idForMacDel >= Ring.iSpace)
                {
                    idForMacDel = subtractHexadecimalStrings(idForMacDel, Ring.iSpace);
                    idForMacDel = padString(idForMacDel, Ring.unique_chars);
                }
            }
            else {
                cout << "Enter the Id of Machine to Delete: " << endl;
                cin >> idForMacDel;
            }
            Ring.Delete_Machine(idForMacDel);
            Ring.UpdateRouting();
            break;
        case 7:
            Ring.displayRing();
            break;
        case 8:
            cout << "Ending the program" << endl;
            break;

        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }
    } while (choice != 8);


}