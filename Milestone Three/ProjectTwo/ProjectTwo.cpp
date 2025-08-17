#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include <list>
#include <unordered_map>
#include <stack>

using namespace std;
//==============
// Course Struct
//==============

struct Course {
	string courseId, courseCode, courseName;
	// courseCode and courseNum are the courseId split into parts
	int courseNum;
	vector<string> prerequisites;
};

//============================
// Hash Table Class Definition
//============================
const unsigned int DEFAULT_SIZE = 5;

class HashTable {
private:
	struct Node {
		Course course;
		unsigned int key;
		Node* next;

		Node() { key = UINT_MAX; next = nullptr;}
		Node(Course aCourse) : Node() { course = aCourse;} // Takes Course
		Node(Course aCourse, unsigned int aKey) : Node(aCourse) { key = aKey;} // Takes Course and key
	};

	vector<Node> nodes; // Holds the courses and their liks
	vector<Node> sortedCourses; // Holds the coruses sorted into alphanumeric order
	unsigned int tableSize = DEFAULT_SIZE; // 5

public:
	//Default Constructor
	HashTable() {
		nodes.resize(tableSize);
	}
	//Destructor
	~HashTable() {
		for (unsigned int i = 0; i < tableSize; ++i) {
			Node* current = nodes[i].next; // Start with the first node in the list
			while (current != nullptr) {
				Node* temp = current; // Keep track of the node to delete
				current = current->next; // Move to the next node
				delete temp; // Delete the old node
			}
		}
	}



	//----- Project Enhancement for CS-499-------
	// Implementing topological sort for the courses and their prerequisites and have them printed for the user

	unordered_map<string, vector<string>> prereqGraph; // Will store course prerequisite information in an unordered map

	// The following function will iterate through the hash table and add entries to the map
	void BuildPrereqGraph() {
		for (unsigned int i = 0; i < nodes.size(); ++i) {
			Node* current = &nodes[i];
			while (current != nullptr && current->key != UINT_MAX) {
				Course course = current->course;
				prereqGraph[course.courseId] = course.prerequisites;
				current = current->next;
			}
		}
	}

	// Parses through the map to find a course and will recursively find their prereqs. Once done, they're pushed to a stack
	void topologicalSortUtil(string courseId, unordered_map<string, bool>& visited, stack<string>& courseStack) {

		visited[courseId] = true;

		for (const string& prereq : prereqGraph[courseId]) {
			if (!visited[prereq]){

				topologicalSortUtil(prereq, visited, courseStack);
				
			}
		}
		courseStack.push(courseId);
	}

	// Sorts the stack created by the Util function
	void topologicalSort() {
		unordered_map<string, bool> visited;
		stack<string> courseStack;

		// Initializes all courses as not visited
		for (const auto& pair : prereqGraph) {
			visited[pair.first] = false;
		}

		// Visit all unvisited courses
		for (const auto& pair : prereqGraph) {
			if (!visited[pair.first]) {
				topologicalSortUtil(pair.first, visited, courseStack);
			}
		}

		vector<string> orderedCourses;
		while (!courseStack.empty()) {
			orderedCourses.push_back(courseStack.top());
			courseStack.pop();
		}

		cout << "\nValid course order:\n" << endl;

		// Prints in reverse so that the initial required course is first and the final course is last
		for (auto it = orderedCourses.rbegin(); it != orderedCourses.rend(); ++it) {
			string courseId = *it;

			// Finds the course name from hash table
			for (Node& node : nodes) {
				Node* current = &node;
				while (current != nullptr && current->key != UINT_MAX) {
					if (current->course.courseId == courseId) {
						cout << current->course.courseId << ": " << current->course.courseName << endl;
						break;
					}
					current = current->next;
				}
			}
		}
		cout << endl;
	}
	


	//-------End of Enhancement-----------

	//Takes the information from the read file and separates the information into related course struct info
	void CourseConversion(vector<vector<string>> aCourseVector) {
		for (int i = 0; i < aCourseVector.size(); i++) {

			Course course; // Course struct to store in Nodes
			string letters = ""; // Will be used to split the course Id into chars and numbers
			int num = 0; // Will be used to split the course Id into chars and numbers
			int key = 0; // Used for hashed key

			for (int j = 0; j < aCourseVector[i].size(); j++) {

				switch (j) {
					case 0:
						course.courseId = aCourseVector[i][j];
						for (char ch : aCourseVector[i][j]) {
							if (isalpha(ch)) {
								letters += ch;
							}
							else if (isalnum(ch)) {
								num = num * 10 + (ch - '0');
							}
						}
						key = hash(num);
						break;
					case 1:
						course.courseName = aCourseVector[i][j];
						course.courseCode = letters;
						course.courseNum = num;
						break;
					case 2:
						course.prerequisites.push_back(aCourseVector[i][j]);
						break;
					case 3:
						course.prerequisites.push_back(aCourseVector[i][j]);
						break;
					default:
						break;
				}
			}
			Node node(course, key);
			addCourse(node); // Node now contains the course information
		}
	}

	unsigned int hash(int key) {
		return key % tableSize;
	}

	

	void addCourse(Node node) {
		if (node.key >= nodes.size()) {
			cerr << "Error: Key out of bounds!" << endl;
			return;
		}

		// Reminder: Each node has a course, a key and a next pointer
		if (nodes[node.key].key == UINT_MAX) { // Check if this key is uninitialized
			nodes[node.key] = node; // Place the node directly if the slot is available
		}
		else {
			Node* current = &nodes[node.key];
			while (current->next != nullptr) {
				current = current->next;
			}
			current->next = new Node(node.course, node.key); // Adds new node at the end of the chain
		}
	}

	// Will sort the courses into an alphanumeric list and print them
	void PrintSortedList() {
		vector<Course> courseList;

		// Collect all courses from the hash table into the vector
		for (unsigned int i = 0; i < nodes.size(); ++i) {
			Node* current = &nodes[i];
			while (current != nullptr && current->key != UINT_MAX) {
				courseList.push_back(current->course);
				current = current->next;
			}
		}

		// courseList sorted based on courseId (alphanumeric sort)
		sort(courseList.begin(), courseList.end(), [](const Course& a, const Course& b) {
			return a.courseId < b.courseId;
		});

		for (const Course& course : courseList) {
			cout << course.courseId << " - " << course.courseName << endl;
		}
		cout << endl << endl;
	}

	// Converts a string to uppercase
	std::string toUpper(const std::string& s) {
		std::string result = s; // Create a copy of the original string
		for (char& ch : result) {
			ch = toupper(static_cast<unsigned char>(ch)); // Apply toupper to each character
		}
		return result;
	}

	// Print course title and prerequisites
	void SearchCourse(string courseId) {
		// Convert courseId to uppercase
		courseId = toUpper(courseId);

		// Iterate over the nodes in the vector
		for (Node& node : nodes) {
			Node* current = &node; // Start with the current node

			// Traverse the linked list in case of collisions
			while (current != nullptr && current->key != UINT_MAX) {
				if (toUpper(current->course.courseId) == courseId) {
					// Print the course details
					cout << current->course.courseId << ", " << current->course.courseName << endl;

					// Check for prerequisites
					if (current->course.prerequisites[0] == "") { // Will check if there are no prerequisites in the vector's first index.
						cout << "No prerequisites.\n" << endl;
					}
					else if (current->course.prerequisites[0] != "") {
						// Print prerequisites if there are any
						cout << "Prerequisites: ";
						for (int i = 0; i < current->course.prerequisites.size(); i++) {
							cout << current->course.prerequisites[i];
							if (i != current->course.prerequisites.size() - 1) {
								cout << ", ";
							}
						}
						cout << endl;
					}
					return; // Course found, so exit the function
				}
				current = current->next; // Move to the next node in case of collision
			}
		}
	}
};

//Load the csv file
vector<vector<string>> loadFile() {
	ifstream file("CS 300 ABCU_Advising_Program_Input.csv"); // File is open

	if (!file.is_open()) {
		cerr << "Error opening file!" << endl;
	}

	string line;
	vector<vector<string>> data;
	while (getline(file, line)) {
		stringstream ss(line);
		string value;
		vector<string> row;
		while (getline(ss, value, ',')) {
			if (value == " ") {
				continue;
			}else 
				row.push_back(value);
		}
		data.push_back(row);
	}
	file.close();
	return data;
}

int main(){
	HashTable* table = new HashTable();
	cout << "Welcome to the course planner." << endl;
	cout << "1: Load Data Structure." << endl;
	cout << "2. Print Course List." << endl;
	cout << "3. Print Course." << endl;
	cout << "9: Exit" << endl;


	int option;
	string findCourse;
	
	while (true) {
		try {
			cout << "What would you like to do?\n" << endl;
			cin >> option;
			if (cin.fail()) {
				throw invalid_argument("Invalid Input. Please enter a valid option.");
			}
		}
		catch (const invalid_argument& e) {
			cerr << e.what() << endl;
			cin.clear();
			cin.ignore();
		}
			
		 
		switch (option) {
			case 1:
				cout << "Loaded File\n" << endl;
				//Call function to load file
				table->CourseConversion(loadFile());
				break;
			case 2:
				cout << "Here is a sample schedule:\n" << endl;
				//Call void function to print all sorted courses in the table
				table->PrintSortedList();
				break;
			case 3:
				cout << "What course do you want to know about?\n" << endl;
				cin >> findCourse;
				//Call function to search and print course info
				table->SearchCourse(findCourse);
				break;
			case 4:
				table->BuildPrereqGraph();
				table->topologicalSort();
				break;
			case 9:
				cout << "Exiting Program...\n" << endl;			
				return 0;
				break;
			default:
				cout << option << " is not a valid option.\n" << endl;
				cin.clear();
				break;
		}
		cout << "1: Load Data Structure." << endl;
		cout << "2. Print Course List." << endl;
		cout << "3. Print Course." << endl;
		cout << "4. Print Valid Course Order." << endl;
		cout << "9: Exit" << endl;
	}
}