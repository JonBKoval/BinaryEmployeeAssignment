#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <string>
//===============================================================
//   PROGRAM:  Programming Assignment #2
//   AUTHOR :  Jonathan Koval
//   DUE    :  4/8/2014
//   DESCRIPTION:
//          Assignment 2: working with binary files to create an
//   index storing and creating records
//===============================================================

using namespace std;

// GLOBAL CONSTANT ====================================
const int MAXNAME = 31; // Empl names 30 in length + 1, can have spaces
char BIN_FILENAME[] = "empl.dat";// binary file name
const int INDEXCOUNT = 100; // constant to be used if index size changes

// employee structure def
struct EMPL_TYPE
{
  int dept_num;
  char name[MAXNAME];
  int age;
  int emplid;		// is a unique key for indexing
 };

struct INDEX_TYPE
{
	int key;   // unique key for indexing
	int RID;   // location of record in file
};

// Function Prototypes =================================
// Menu Functions
int main_menu();
int view_employee_menu();
int edit_employee_dept(INDEX_TYPE index[]);
int list_employees();
void clearscreen();

// Index functions
void build_index(INDEX_TYPE index[], int & count);
void sort_index(INDEX_TYPE index[]);
void print_ascend_index(INDEX_TYPE index[]);
int return_rid(INDEX_TYPE index[],int empid,int count);
EMPL_TYPE return_record(INDEX_TYPE index[],int RID);
void write_record(INDEX_TYPE index[],int RID,int newdeptnum);
void print_record(INDEX_TYPE index[],int RID);


//=======================  MAIN  =======================
int main()
{
	int selection = 0;
	int count = 0;
	int RID = 0;
	int empl_id = 0;
	INDEX_TYPE index[100];

	// build the index 
	build_index(index, count);

	while ( selection != 4 )
	{
		selection = main_menu();


		switch(selection)
		{
		// View employee record
		case 1: 
			empl_id = view_employee_menu(); 
			RID = return_rid(index,empl_id,100);
			if (RID != -99)
				print_record(index,RID);
			else
				cout << "No record was found, returning you to Main Menu\n\n"; 

			break;
		
		// Edit Employee Department #
		case 2: edit_employee_dept(index);  
			
			break;
		
		// List all Employees
		case 3: 
			if(list_employees() == 1)
				print_ascend_index(index);
			else if (list_employees() == 2)
			{
				cout << endl << "This feature is not implemented yet." << endl;
				break;
			}
			else
				break;

		// End the program
		case 4: break; 
			
		// Invalid choice
		default:
			cout << "Invalid choice, please select again." << endl << endl; 
			break;
		}
	}
	return 0;
}

// Functions ===========================================

// Main Menu gives user choices on what to do with records
int main_menu()
{
	int selection = 0;
	
	cout << "Main Menu" << endl << endl;
	cout << "1) View an Employee" << endl;
	cout << "2) Edit an Employee's Department Number" << endl;
	cout << "3) List all Employees" << endl;
	cout << "4) End the program" << endl << endl << endl;
	cout << "Please enter your selection from the menu [1-4] :  ";

	cin >> selection;
	cout << endl;
	clearscreen();

	return selection;
}

// View Employee allows user to view a record using empid
int view_employee_menu()
{
	int empid = 0;

	cout << "View Employee" << endl << endl;
	cout << "To view an employee record you will need " 
		"enter their Employee ID number." << endl;
	cout << "To return to the MAIN MENU : enter number 0" << endl;
	cout << endl << "Please enter the Employee ID number:  ";

	cin >> empid;
	cout << endl;
	clearscreen();

	return empid; // Return to main and next Call to get and view record
}

// Edit Employee department definition
int edit_employee_dept(INDEX_TYPE index[])
{
	int empid = 0;
	int newdeptnum = 0;

	cout << "Edit Employee" << endl << endl;
	cout << "To edit an employee department number you will need \n" 
		"to enter their Employee ID number.\n" << endl;
	cout << "To return to the MAIN MENU : enter number 0" << endl;
	cout << endl << "Please enter the Employee ID number:  ";

	cin >> empid;
	cout << endl;


	if ( empid == 0 )
	{
		clearscreen();
		return 0;
	}
	else
	{
		// Call to get record to veiw
		int RID = return_rid(index,empid,INDEXCOUNT);
		if ( RID < -1 )
		{
			clearscreen();
			cout << "Record Not Found Returning to Main Menu" << endl << endl;
			return 0;
		}
		EMPL_TYPE empl_rec = return_record(index, RID);
		
		cout << endl << "Employee: " << empl_rec.name;
		cout << endl << "\nCurrent Department Number: " << empl_rec.dept_num;
		cout << endl << "\nTo change the department number you will \n"
			"need to enter the new department number.\n";
		cout << endl << "To return to the MAIN MENU: enter 0\n";
		cout << endl << "Please enter the NEW department number:  ";
		cin >> newdeptnum;
		cout << endl;
		
		write_record(index,RID,newdeptnum); //writes the new dept number to file

		clearscreen();
		return newdeptnum;
	}
}
// List employees definition
int list_employees()
{
	int choice = 0;

	cout << "List all employees" << endl << endl;
	cout << "You may list all the employees ordered in several \n" 
		"different ways. All will be in ascending order.\n" << endl;
	cout << "1) List ordered by employee ID number" << endl;
	cout << "2) List ordered by employee name (currently unavailable)" << endl;
	cout << "3) Return to MAIN MENU" << endl;
	cout << endl << "Please enter the your selection [1-3] :  ";

	cin >> choice;
	cout << endl;

	//processing for ordering lists

	clearscreen();
	return choice;
}

//Function to clear screen due to system("cls") having issues with different OS/Compilers
void clearscreen()
{ 
	//Found this after doing some research... Seemed to be the best for an assignment
	// however not to be used in actual production code aparently.
	//Assuming Windows
	#ifdef _WIN32
	system ("cls");

	#else
	//Assuming POSIX
	system ("clear");
	#endif
}

//Builds the index
void build_index(INDEX_TYPE index[],int & count)
{
	EMPL_TYPE empl_rec;  // record to be read into from file
	fstream empl_infile;   // file variable (no mode set)
	empl_infile.open(BIN_FILENAME,ios::in|ios::binary); // open binary file for reading
	for (int i = 0; i < INDEXCOUNT; i++)
	{
		index[i].key = -1;
		index[i].RID = -1;
	}
	if(empl_infile.fail())
	{
		cout << "Open File Error" << endl;
		exit(1);
	}
	empl_infile.read((char *) &empl_rec,sizeof(empl_rec)); //attempt to read in a record
	count = 0;
	while (!empl_infile.fail() && !empl_infile.eof())  // while the last thing read was not the End Of File marker
	{
		index[count].key = empl_rec.emplid;
		index[count].RID = count;
		count++;

		empl_infile.read((char *) &empl_rec,sizeof(empl_rec)); // read in next record
	}

	empl_infile.close(); // close the file
}

// Pretty standard sort function for an array
void sort_index(INDEX_TYPE index[])
{
	int i = 0;
	int j = 0;
	int temp = 0;
	int RID_temp = 0;

	for( i = 0; i < INDEXCOUNT; i++ )
	{
		for( j = 0; j < INDEXCOUNT; j++ )
		{
			if( index[i].key < index[j].key )
			{
				temp = index[i].key;
				RID_temp = index[i].RID;
				index[i].key = index[j].key;
				index[i].RID = index[j].RID;
				index[j].key = temp;
				index[j].RID = RID_temp;
			}
		}
	}
/*  Testing function to check array is sorted properly
	for (int i = 0; i < 100; i++)
	{
		cout << index[i].key << endl;
	}
*/
}

// Function to print the array in ascending emplid order 
void print_ascend_index(INDEX_TYPE index[])
{
	sort_index(index);  // make sure index is sorted
	int RID = 0;
	EMPL_TYPE empl_rec;  // record to be read into from file
	fstream empl_infile;   // file variable (no mode set)
	empl_infile.open(BIN_FILENAME,ios::in|ios::binary); // open binary file for reading
	
	if(empl_infile.fail())  
	{
		cout << "Open File Error" << endl;
		exit(1);
	}

	for (int i = 0; i < INDEXCOUNT; i++)
	{
		if (index[i].RID != -1) 
		{
			RID = return_rid(index, index[i].key, INDEXCOUNT );
			empl_rec = return_record(index, RID);
			cout << "Dept: " << empl_rec.dept_num;
			cout << "  Name: " << empl_rec.name;
			cout << "  age : " << empl_rec.age;
			cout << "  emplid : " << empl_rec.emplid << endl;
		}
	}
	cout << endl << endl << endl;
	empl_infile.close(); // close the file
}

// Returns the RID when passed in the emplid
int return_rid(INDEX_TYPE index[],int empid,int count)
{
	int n = 0;

	while( n < count )
	{
		if (empid == index[n].key)
			return index[n].RID;
		n++;
	}
	return -99;
}

// returns the record given the Reference ID or RID
EMPL_TYPE return_record(INDEX_TYPE index[],int RID)
{
	EMPL_TYPE empl_rec;  // record to be read into from file
	fstream empl_infile;   // file variable (no mode set)
	empl_infile.open(BIN_FILENAME,ios::in|ios::binary); // open binary file for reading
	empl_infile.clear();
	empl_infile.seekg(0,ios::beg);

	if(empl_infile.fail())  
	{
		cout << "Open File Error" << endl;
		exit(1);
	}

	empl_infile.seekg(RID * sizeof(empl_rec));   // Seek to correct position in file
	empl_infile.read((char *) &empl_rec,sizeof(empl_rec));  // read in the record into the struct
	empl_infile.close(); // close the file
	return empl_rec; //return record structure.
}

// Function to print one record
void print_record(INDEX_TYPE index[],int RID)
{
	EMPL_TYPE empl_rec;  // record to be read into from file
	empl_rec = return_record(index, RID);

	cout << "Dept: " << empl_rec.dept_num;	
	cout << "  Name: " << empl_rec.name;	
	cout << "  age : " << empl_rec.age;		
	cout << "  emplid : " << empl_rec.emplid << endl << endl;
}

// Overwrites existing spot in binary file while changing record's dept number
void write_record(INDEX_TYPE index[],int RID,int newdeptnum)
{
	EMPL_TYPE empl_rec; //record to be filled and then written out
	empl_rec = return_record(index,RID);  // populate a temp record for insertion
	empl_rec.dept_num = newdeptnum; // changes the records department number
	fstream empl_outfile; // assign file variable for output
	empl_outfile.open(BIN_FILENAME,ios::out|ios::in|ios::binary); // open file an se mode for Binary file

	if(empl_outfile.fail())
	{
		cout << "Open File Error" << endl;
		exit(1);
	}
	
	empl_outfile.seekp(RID * sizeof(empl_rec));  // sets the next spot to write to in file
    empl_outfile.write((char *) &empl_rec, sizeof(empl_rec)); // write the record out to the Binary file
	
	empl_outfile.close(); // close the binary file
}

