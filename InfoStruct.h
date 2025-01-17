#pragma once
#include<iostream>
#include<sstream>

using namespace std;

struct Info {
	string key;
	string value;

	Info(string k = "", string val = "") {
		key = k;
		value = val;
	}

	Info(const Info& other) : key(other.key), value(other.value) {}

};

class Bnode {
public:
	Info** Bkey;
	Bnode** children;
	int num_keys;
	bool leaf;
	int order;

	Bnode(int ORDER, bool isleaf = false) {
		num_keys = 0;
		leaf = isleaf;
		order = ORDER;
		Bkey = new Info * [order - 1];
		children = new Bnode * [order];

		for (int i = 0; i < order - 1; i++) {
			Bkey[i] = NULL;
		}
		for (int i = 0; i < order; i++) {
			children[i] = NULL;
		}
	}

	void InserttoNode(Info* newInfo) {
		int i = num_keys - 1;

		//find the appropriate position for the new key
		while (i >= 0 && Bkey[i] != NULL && newInfo->key < Bkey[i]->key) {
			Bkey[i + 1] = Bkey[i];
			i--;
		}

		//insert the new key at the correct position
		Bkey[i + 1] = newInfo;
		num_keys++;
	}

	Info* RemoveFromNode(int index) {
		Info* delkey = Bkey[index];
		Bkey[index] = NULL;

		//readjusting keys
		for (int i = index; i < num_keys - 1; i++) {
			Bkey[i] = Bkey[i + 1];
			Bkey[i + 1] = NULL;
		}

		//readjusting children
		for (int i = index + 1; i < num_keys; i++) {
			children[i] = children[i + 1];
			children[i + 1] = NULL;
		}

		num_keys--;

		return delkey;
	}

	void splitChild(int i, Bnode* y, int splitIndex) {
		Bnode* z = new Bnode(y->order, y->leaf);
		bool special_case = false;	//special case is where the new entry lies directly between split and split+1

		// Copy the keys from y to z
		for (int j = 1; (splitIndex + j) < y->num_keys; j++) {
			z->InserttoNode(y->Bkey[splitIndex + j]);
		}

		//if y is an internal node, copy its children as well
		if (!y->leaf) {
			for (int j = 1; j < y->order; j++)
				z->children[j - 1] = y->children[splitIndex + j];
		}

		//making space for the new key in the current node
		for (int j = num_keys - 1; j >= i; j--)
			Bkey[j + 1] = Bkey[j];

		//insert the middle key of y into the current node
		Bkey[i] = y->Bkey[splitIndex];

		//removing the moved keys from y
		for (int j = y->num_keys - 1; j >= splitIndex; j--) {
			y->RemoveFromNode(j);
		}

		//make space for the new child pointer in the current node
		for (int j = num_keys; j >= i + 1; j--)
			children[j + 1] = children[j];

		children[i + 1] = z;

		num_keys = num_keys + 1;
	}

	void insertNonFull(Bnode* x, Info* newInfo) {
		int i = x->num_keys - 1;

		if (x->leaf) {		//insertion point found (assuming that the splitting has been dealt with)
			x->InserttoNode(newInfo);
		}
		else {		//continue traversal and split where necessary
			while (i >= 0 && newInfo->key < x->Bkey[i]->key) {
				i--;
			}
			i += 1;		//i now contains the index for the child to continue traversal
			if (x->children[i]->num_keys == x->order - 1) {	//checking if child is full
				int splitIndex = 0;
				int newI = -1;
				if (order % 2 == 0) {	//if order is even then splitindex is the default middle
					splitIndex = (order / 2) - 1;
				}
				else {	//if order is odd, then the splitIndex needs to be calculated depending on the new entry
					splitIndex = (order / 2) - 1;

					if ((newInfo->key > x->children[i]->Bkey[splitIndex]->key)) {
						splitIndex += 1;
						if (!(newInfo->key < x->children[i]->Bkey[splitIndex + 1]->key)) {//special case check
							newI = i + 1;
						}
					}
				}

				splitChild(i, x->children[i], splitIndex);
				if (newI != -1) {
					i = newI;
				}
			}
			x->children[i]->insertNonFull(x->children[i], newInfo);
		}
	}


};

class Btree {
public:
	Bnode* root;
	int tree_order;



	Btree(int Order) {
		root = NULL;
		tree_order = Order;
	}

	void displayInOrder(Bnode* node, int level = 0) {
		if (node != nullptr) {
			for (int i = 0; i < node->num_keys; i++) {
				displayInOrder(node->children[i], level + 1);
				cout << "Level " << level << ": " << node->Bkey[i]->key << endl;
			}
			displayInOrder(node->children[node->num_keys], level + 1);
		}
	}

	bool duplication(string value) {
		for (int i = 0; i < value.length(); i++) {
			if (value[i] == '\n') {
				return true;
			}
		}
		return false;
	}

	bool exist(string key, Bnode* node) {
		if (node != nullptr) {
			for (int i = 0; i < node->num_keys; i++) {
				if (node->Bkey[i]->key == key) {
					return true;
				}
				if (exist(key, node->children[i])) {
					return true;
				}
			}
			if (exist(key, node->children[node->num_keys])) {
				return true;
			}
		}
		return false;
	}

	Info* search(string key, Bnode* node) {
		if (node != nullptr) {
			for (int i = 0; i < node->num_keys; i++) {
				if (node->Bkey[i]->key == key) {
					return node->Bkey[i];
				}
				search(key, node->children[i]);
			}
			search(key, node->children[node->num_keys]);
		}
	}


	void insert(Info* newInfo) {
		if (root == nullptr) {
			root = new Bnode(tree_order, true);
			root->InserttoNode(newInfo);
		}
		else if (exist(newInfo->key, root)) {		//same key already exist(duplication)
			Info* current = search(newInfo->key, root);
			stringstream ss;
			ss << current->value << "\n" << newInfo->value;
			current->value = ss.str();
		}
		else if (root->num_keys == tree_order - 1) {	//root is full
			Bnode* new_root = new Bnode(tree_order);
			new_root->children[0] = root;
			root = new_root;

			int splitIndex = 0;

			if (root->order % 2 == 0) {	//if order is even then splitindex is the default middle
				splitIndex = (root->order / 2) - 1;
			}
			else {	//if order is odd, then the splitIndex needs to be calculated depending on the new entry
				splitIndex = (root->order / 2) - 1;

				if (newInfo->key > root->children[0]->Bkey[splitIndex]->key) {
					splitIndex += 1;
				}
			}

			root->splitChild(0, root->children[0], splitIndex);		//splitting the previous root
			root->insertNonFull(root, newInfo);
		}
		else {
			root->insertNonFull(root, newInfo);
		}
	}

	void deleteKey(string k, Bnode* current) {

		if (duplication(search(k, root)->value)) {	//key to be deleted has duplicates
			Info* del = search(k, root);
			int choice=-1;
			stringstream ss;

			cout << del->value<<endl<<"Which of these do you want to delete?";
			cin >> choice;
			choice--;

			for (int i = 0; i < del->value.length(); i++) {
				if (del->value[i] == '\n') {
					choice--;
				}
				if (choice == 0) {
					i++;
					for (; i < del->value.length(); i++) {
						if (del->value[i] == '\n') {
							ss << del->value[i];
							break;
						}
					}
					choice = -1;
				}
				ss << del->value[i];
			}
			return;
		}

		int t = (current->order + 1) / 2;
		int i = 0;

		// Find the appropriate position for the key
		while (i < current->num_keys && k > current->Bkey[i]->key) {
			i++;
		}

		if (current->leaf) {
			// If the key is in current node and it's a leaf, remove it
			if (i < current->num_keys && current->Bkey[i]->key == k) {
				if (current == root && i == 0) {
					root = NULL;
				}
				delete current->Bkey[i];
				current->RemoveFromNode(i);
				if (current->Bkey[0] == NULL) {
					delete current;
					current = NULL;
				}
			}
			return;
		}

		if (i < current->num_keys && current->Bkey[i]->key == k) {
			//if the key is in current node but it's not a leaf, call delete_internal_node
			delete_internal_node(current, k, i);
		}
		else if (current->children[i]->num_keys >= t) {
			//if the child node at index i has enough keys, recursively call delete
			deleteKey(k, current->children[i]);
		}
		else {
			//cases where the child node needs adjustment
			if (i != 0 && i + 2 < current->order) {
				if (current->children[i - 1]->num_keys >= t) {
					delete_sibling(current, i, i - 1);
				}
				else if (current->children[i + 1] != NULL && current->children[i + 1]->num_keys >= t) {
					delete_sibling(current, i, i + 1);
				}
				else {
					int merge_index = i + 1;
					if (current->children[i + 1] == NULL) {
						merge_index = i - 1;
					}

					delete_merge(current, i, merge_index);
					if (merge_index == i - 1) {
						i = i - 1;		//trying
					}
				}
			}
			else if (i == 0) {
				if (current->children[i + 1]->num_keys >= t) {
					delete_sibling(current, i, i + 1);
				}
				else {
					delete_merge(current, i, i + 1);
				}
			}
			else if (i + 2 == current->order) {
				if (current->children[i - 1]->num_keys >= t) {
					delete_sibling(current, i, i - 1);
				}
				else {
					delete_merge(current, i, i - 1);
					i = i - 1;
				}
			}

			//continue the deletion recursively
			if (current->children[i] != NULL) {
				deleteKey(k, current->children[i]);
			}
		}
	}
	void delete_internal_node(Bnode* x, string k, int i) {
		int t = (x->order + 1) / 2;

		if (x->leaf) {
			//if x is a leaf and contains the key, remove it
			if (x->Bkey[i]->key == k) {
				delete x->Bkey[i];
				x->RemoveFromNode(i);
			}
			return;
		}

		if (x->children[i]->num_keys >= t) {
			//if x is not a leaf and contains the key, call delete_predecessor
			x->Bkey[i] = delete_predecessor(x->children[i]);
			return;
		}
		else if (x->children[i + 1]->num_keys >= t) {
			//if the right sibling has enough keys, replace the key with the successor
			x->Bkey[i] = delete_successor(x->children[i + 1]);
			return;
		}
		else {
			//merge the current node and its right sibling
			delete_merge(x, i, i + 1);

			//continue the deletion recursively
			delete_internal_node(x->children[i], k, t - 1);
			if (x->Bkey[0] == NULL) {
				delete x;
				x = NULL;
			}
		}
	}

	Info* delete_predecessor(Bnode* current) {
		if (current->leaf) {
			//if the node is a leaf, remove and return the last key
			Info* deletedKey = current->RemoveFromNode(current->num_keys - 1);
			return deletedKey;
		}

		int n = current->num_keys - 1;
		int t = (current->order - 1) / 2;

		if (current->children[n]->num_keys >= t) {
			//if the last child has enough keys, call delete_sibling
			delete_sibling(current, n + 1, n);
		}
		else {
			//if not, merge the last child and its sibling
			delete_merge(current, n, n + 1);
		}

		//continue the deletion recursively
		return delete_predecessor(current->children[n]);
	}

	Info* delete_successor(Bnode* current) {
		if (current->leaf) {
			//if the node is a leaf, remove and return the first key
			Info* deletedKey = current->Bkey[0];
			current->RemoveFromNode(0);
			return deletedKey;
		}
		int t = (current->order - 1) / 2;
		if (current->children[1]->num_keys >= t) {
			//if the second child has enough keys, call delete_sibling
			delete_sibling(current, 0, 1);
		}
		else {
			//if not, merge the first child and its sibling
			delete_merge(current, 0, 1);
		}

		//continue the deletion recursively
		return delete_successor(current->children[0]);
	}

	void delete_merge(Bnode* x, int i, int j) {
		Bnode* cnode = x->children[i];

		if (j > i) {
			Bnode* rsnode = x->children[j];

			//move the key from the current node to the left child
			cnode->Bkey[cnode->num_keys] = x->Bkey[i];
			cnode->num_keys++;

			//copy keys and children from the right sibling to the left child
			for (int k = 0; k < rsnode->num_keys; k++) {
				cnode->Bkey[cnode->num_keys] = rsnode->Bkey[k];
				cnode->num_keys++;

				cnode->children[cnode->num_keys - 1] = rsnode->children[k];
			}
			cnode->children[cnode->num_keys] = rsnode->children[rsnode->num_keys];

			x->RemoveFromNode(i);

			//delet the right sibling
			x->children[i + 1] = nullptr;
			delete rsnode;
		}
		else {
			Bnode* lsnode = x->children[j];

			//move the key from the parent node to the left child
			lsnode->Bkey[lsnode->num_keys] = x->Bkey[j];
			lsnode->num_keys++;

			//copy keys and children from the left sibling to the right child
			for (int k = 0; k < cnode->num_keys; k++) {
				lsnode->Bkey[lsnode->num_keys] = cnode->Bkey[k];
				lsnode->num_keys++;

				lsnode->children[lsnode->num_keys - 1] = cnode->children[k];
			}

			lsnode->children[lsnode->num_keys] = cnode->children[cnode->num_keys];

			x->RemoveFromNode(j);

			//delete the left child
			x->children[i] = nullptr;
			delete cnode;
		}

		//if x is the root and has no keys left, update the root

		if (x == root && x->num_keys == 0) {
			root = (j > i) ? x->children[i] : x->children[j];
		}
	}
	void delete_sibling(Bnode* x, int i, int j) {
		Bnode* cnode = x->children[i];

		if (i < j) {
			Bnode* rsnode = x->children[j];				//current sibling is i, other sibling is j

			cnode->InserttoNode(x->Bkey[0]);	//first key of x

			x->Bkey[0] = rsnode->RemoveFromNode(0);
		}
		else {
			Bnode* lsnode = x->children[j];

			cnode->InserttoNode(x->Bkey[x->num_keys - 1]);	//last key of x

			x->Bkey[x->num_keys - 1] = lsnode->RemoveFromNode(lsnode->num_keys - 1);
		}
	}


	void deleteTree(Bnode* node) {
		if (node) {
			// Delete children recursively
			for (int i = 0; i <= node->num_keys; i++) {
				deleteTree(node->children[i]);
			}

			// Delete keys and set them to NULL
			for (int i = 0; i < node->num_keys; i++) {
				delete node->Bkey[i];
				node->Bkey[i] = nullptr;
			}

			// Delete the node
			delete node;
		}
	}

	void deleteEntireTree() {
		deleteTree(root);
		root = nullptr;
	}

	Info smallest() {
		Bnode* current = root;
		while (current->children[0] != NULL) {
			current = current->children[0];
		}

		return *current->Bkey[0];
	}

};