#pragma once
#include <iostream>
#include<cmath>
#include <string>

#include"InfoStruct.h"
#include"Hashing.h"

using namespace std;
class Machine;
class RoutingNode {
public:
	RoutingNode* prev;
	RoutingNode* next;
	Machine* Node;

	RoutingNode() {
		prev = NULL;
		next = NULL;
		Node = NULL;
	}
};

class Machine {  // node
public:
	Btree* btree;
	string M_Id;
	Machine* next;
	string MachineName;
	int bitSp;
	string iSpace;
	RoutingNode* ftp;
	int unique_chars;

	Machine(string id, int bits, int m, int uchars, string name)
	{
		this->M_Id = id;
		this->next = NULL;
		bitSp = bits;
		iSpace = powerOfTwoToHex(bitSp);
		btree = new Btree(m);
		ftp = NULL;
		unique_chars = uchars;
		MachineName = name;
		//define unique_chars which shows the number of characters to be extracted at the end of the hash
	}

	Machine* succ(string id, Machine* head) {
		Machine* current = head;

		while (current->M_Id < id) {
			current = current->next;
			if (current == head) {
				return current;
			}
		}
		return current;
	}

	void makeFT(Machine* head) {
		RoutingNode* current = NULL;
		RoutingNode* prev = NULL;
		if (ftp == NULL) {	//null for the first time
			ftp = new RoutingNode();
			current = ftp;
			current->prev = prev;
			for (int i = 2; i <= bitSp; i++) {
				current->next = new RoutingNode();
				prev = current;
				current = current->next;
				current->prev = prev;
			}
		}
		//assigning values to ftp
		current = ftp;
		prev = NULL;
		string num = "";
		for (int i = 1; i <= bitSp; i++) {
			num = addHexadecimalStrings(M_Id, powerOfTwoToHex(i - 1));
			num = padString(num, unique_chars);
			while (num >= iSpace)
			{
				num=subtractHexadecimalStrings(num, iSpace);
				num = padString(num, unique_chars);
			}

			current->Node = succ(num, head);
			current = current->next;
		}

	}

	void findFT(string macID) {
		if (M_Id == macID)
		{
			cout << "Routing Table: " << endl;
			this->displayFT();
			return;
		}
		else
		{
			RoutingNode* current = ftp;
			if (M_Id < macID && (macID <= current->Node->M_Id || current->Node->M_Id < M_Id))
			{
				cout << "Routing Table: " << endl;
				current->Node->displayFT();
				return;
			}
			else
			{
				RoutingNode* Successor = current->next;
				while (Successor != NULL)
				{
					if ((current->Node->M_Id < macID) && (macID <= Successor->Node->M_Id))
					{
						current->Node->findFT(macID);
						return;
					}
					Successor = Successor->next;
					current = current->next;
				}
				//if still not added then go to the last entry of routing table
				current->Node->findFT(macID);
			}
		}
	}

	void displayFT() {
		RoutingNode* current = ftp;
		cout << "Routing table for: " << M_Id << endl;
		while (current != NULL) {
			current->Node->printMachine();
			cout << " -> ";
			current = current->next;
		}
	}

	void InsertFile(Info* newFile)
	{	
		printMachine();
		cout << " -> ";
		if (M_Id == newFile->key)
		{
			btree->insert(newFile);
			return;
		}
		else
		{
			RoutingNode* current = ftp;
			if (M_Id < newFile->key && (newFile->key <= current->Node->M_Id || current->Node->M_Id < M_Id))
			{	
				current->Node->printMachine();
				cout << " -> ";
				current->Node->btree->insert(newFile);
				return;
			}
			else
			{
				RoutingNode* Successor = current->next;
				while (Successor != NULL)
				{
					if ((current->Node->M_Id < newFile->key) && (newFile->key <= Successor->Node->M_Id))
					{
						current->Node->InsertFile(newFile);
						return;
					}
					Successor = Successor->next;
					current = current->next;
				}
				//if still not added then go to the last entry of routing table
				current->Node->InsertFile(newFile);
				return;
			}
		}
	}

	void DeleteFile(string key)
	{
		printMachine();
		if (M_Id == key)
		{
			btree->deleteKey(key, btree->root);
			return;
		}
		else
		{
			RoutingNode* current = ftp;
			if (M_Id < key && (key <= current->Node->M_Id || current->Node->M_Id < M_Id))
			{
				current->Node->printMachine();
				current->Node->btree->deleteKey(key, current->Node->btree->root);
				return;
			}
			else
			{
				RoutingNode* Successor = current->next;
				while (Successor != NULL)
				{
					if ((current->Node->M_Id < key) && (key <= Successor->Node->M_Id))
					{
						current->Node->DeleteFile(key);
						return;
					}
					Successor = Successor->next;
					current = current->next;
				}
				//if still not added then go to the last entry of routing table
				current->Node->DeleteFile(key);
			}
		}
	}

	void findBtree(string macID) {
		if (M_Id == macID)
		{
			cout << "Btree: " << endl;
			btree->displayInOrder(btree->root);
			return;
		}
		else
		{
			RoutingNode* current = ftp;
			if (M_Id < macID && (macID <= current->Node->M_Id || current->Node->M_Id < M_Id))
			{	
				cout << "Btree: " << endl;
				current->Node->btree->displayInOrder(current->Node->btree->root);
				return;
			}
			else
			{
				RoutingNode* Successor = current->next;
				while (Successor != NULL)
				{
					if ((current->Node->M_Id < macID) && (macID <= Successor->Node->M_Id))
					{
						current->Node->findBtree(macID);
						return;
					}
					Successor = Successor->next;
					current = current->next;
				}
				//if still not added then go to the last entry of routing table
				current->Node->findBtree(macID);
			}
		}
	}

	void printMachine() {
		cout << "(Name: " << MachineName << " ID: " << M_Id << ")";
	}

};
class RingDHT {  // Singly Circular Linked List
public:
	Machine* head;
	int bitSpace;
	string iSpace;
	int tree_order;
	int unique_chars;	//number of characters to be chosen at the end of the hash
	bool manual;

	RingDHT(int bits, int m, bool opt) {
		head = NULL;
		bitSpace = bits;
		iSpace = powerOfTwoToHex(bitSpace);
		tree_order = m;
		manual = opt;
		unique_chars = calculateUniqueChars(iSpace);
	}
	void UpdateRouting()
	{
		Machine* ptr = head;

		do
		{
			ptr->makeFT(head);
			ptr = ptr->next;
		} while (ptr != head);
	}
	void Add_machine() //Machine ID
	{
		std::string macName;
		const int size = 20;


		while (1) {
			cout << "Enter the name of Machine(without spaces)" << endl;
			cin >> macName;

			string machineID;

			if (manual)
			{
				cout << "Enter The Machine ID : ";
				cin >> machineID;
				if (machineID > iSpace) {
					continue;
				}
			}
			else
			{
				machineID = getLastSubstring(Hash(macName), unique_chars);
				while (machineID >= iSpace)
				{
					machineID = subtractHexadecimalStrings(machineID, iSpace);
					machineID = padString(machineID, unique_chars);
				}
			}
			if (!SearchMachine(machineID))
			{
				InsertMac(machineID, macName);
				break;
			}
			else
			{
				cout << "Warning: Name/ID of Machine is not Unique." << endl;
			}
		}

	}
	void displayRing()
	{
		cout << endl;
		Machine* ptr = head;
		do
		{
			ptr->printMachine();
			ptr = ptr->next;
			if (ptr != head) {
				cout << " -> ";
			}
		} while (ptr != head);
		cout << endl;
	}
	void InsertMac(string id, string macName)
	{


		Machine* newnode = new Machine(id, bitSpace, tree_order, unique_chars, macName);
		Machine* traversal_Ptr = head;


		if (head == nullptr) {
			head = newnode;
			head->next = head;
			return;
		}
		else if (head->next == head) {
			head->next = newnode;
			newnode->next = head;
			if (newnode->M_Id < head->M_Id) {
				head = newnode;
			}
			return;
		}

		do {
			if ((traversal_Ptr->M_Id < id && traversal_Ptr->next->M_Id > id) || traversal_Ptr->next==head) {
				newnode->next = traversal_Ptr->next;
				traversal_Ptr->next = newnode;
				if (newnode->M_Id < head->M_Id) {
					head = newnode;
				}
				break;
			}
			traversal_Ptr = traversal_Ptr->next;
		} while (traversal_Ptr != head);

		if (newnode->next->btree->root == NULL) {		//successor is empty
			return;
		}

		Machine* successor = newnode->next;

		//adjusting the btrees to snatch files from the successor
		while (1) {
			if (successor->btree->root == NULL) {
				break;
			}
			Info temp = successor->btree->smallest();
			if (temp.key <= newnode->M_Id) {
				newnode->btree->insert(new Info(temp));
				successor->btree->deleteKey(temp.key, successor->btree->root);
			}
			else {
				break;
			}
		}

	}
	void Delete_Machine(string id)
	{
		Machine* delNode = head;
		Machine* prevNode = head;

		while (delNode->M_Id != id) {
			delNode = delNode->next;
		}
		Machine* successor = delNode->next;
		while (prevNode->next != delNode) {
			prevNode = prevNode->next;
		}
		//prev contains the node before del
		if (head->next == head) {
			delete delNode;
			delNode = NULL;
			head = NULL;
			return;
		}

		//giving the files of the btree to its successor

		while (delNode->btree->root!=NULL) {
			Info temp = delNode->btree->smallest();
			successor->btree->insert(new Info(temp));
			delNode->btree->deleteKey(temp.key, delNode->btree->root);
		}
		//now that the files have been distributed. the node can be deleted

		prevNode->next = successor;
		if (head == delNode) {
			head = successor;
		}
		delete delNode;
		delNode = NULL;
	}


	bool SearchMachine(string id)
	{
		Machine* ptr = head;
		if (head == NULL)
		{
			return false;
		}
		do
		{
			if (ptr->M_Id == id)
			{
				return true;
			}
			ptr = ptr->next;
		} while (ptr != head);
		return false;
	}

	void FileInsertion(string path)
	{
		string keyGen = getLastSubstring(Hash(readFile(path)), unique_chars);
		while (keyGen >= iSpace)
		{
			keyGen = subtractHexadecimalStrings(keyGen, iSpace);
			keyGen = padString(keyGen, unique_chars);
		}
		Info* newFile = new Info(keyGen, path);
		cout << "Key generated for file is: " << keyGen << endl;

		head->InsertFile(newFile);
	}

	void FileDeletion(string key) {
		head->DeleteFile(key);
	}


	void printBtree(string macID) {
		head->findBtree(macID);
	}

	void printRoutingTable(string macID) {
		head->findFT(macID);
	}

};