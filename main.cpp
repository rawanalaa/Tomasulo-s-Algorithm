#include<iostream>
#include<fstream>
#include<string>
#include<unordered_map>
#include<map>
#include<vector>
#include<sstream>
#include <queue>
#include<iomanip>



//create reservation we check how to issue 
using namespace std;

map<string,string> registers , previousregisters;
map<int, string> program;
map<int, string>datamemory;
int cycle=1;
string current_instruction, currentline;
queue<string> ROB, lastadded;
map<string, string>instructionStatus;
unordered_map<string, vector<int>>output;
vector<string>order;
int arith = 0, load = 0 , stor = 0, Jays = 0, beq = 0, nand= 0, mul = 0 ;
int address , numofinst=0;
float IPC;
map<string,int> cycleissuing;
map<string, int> cyclewriting;
bool reading, stallissue = false, stallexecute;






void issue()
{
	numofinst += 1;
	if (current_instruction== "ADD" || current_instruction == "SUB" || current_instruction == "ADDI")
	{
		if (arith % 4 == 3 || ROB.size()==8)
		{
			//skip
			
			stallissue = true;
			address --;
		}
		else
		{

			stallissue = false;
			ROB.push(currentline);
			lastadded.push(current_instruction);
			output.insert({ currentline , {cycle,0,0,0,0} });
			order.push_back(currentline);
			arith++;
		}
	}
	else if (current_instruction == "NAND" )
	{
		if (nand % 3 == 2 || ROB.size() == 8)
		{
			//skip
	
			stallissue = true;
			address--;
		}
		else
		{

			stallissue = false;
			ROB.push(currentline);
			lastadded.push(current_instruction);
			order.push_back(currentline);
			output.insert({ currentline , {cycle,0,0,0,0} });
			nand++;
		}
	}
	else if (current_instruction == "MUL")
	{
		if (mul % 3 == 2 || ROB.size() == 8)
		{
			//skip
			
			stallissue = true;
			address--;
		}
		else
		{
		
			stallissue = false;
			ROB.push(currentline);
			mul++;
			lastadded.push(current_instruction);
			output.insert({ currentline , {cycle,0,0,0,0} });
			order.push_back(currentline);
		}
	}
	else if (current_instruction == "LOAD")
	{
		if (load % 3 == 2 || ROB.size() == 8)
		{
			//skip
	
			stallissue = true;
			address--;
		}
		else
		{
		
			stallissue = false;
			ROB.push(currentline);
			load++;
			lastadded.push(current_instruction);
			output.insert({ currentline , {cycle,0,0,0,0} });
			order.push_back(currentline);
		}
	}
	else if (current_instruction == "STORE")
	{
		if (stor % 3 == 2 || ROB.size() == 8)
		{
			//skip

			stallissue = true;
			address--;

		}
		else
		{
	
			stallissue = false;
			ROB.push(currentline);
			stor++;
			lastadded.push(current_instruction);
			output.insert({ currentline , {cycle,0,0,0,0} });
			order.push_back(currentline);
		}
	}
	else if (current_instruction == "JMP" || current_instruction == "JAL" || current_instruction == "RET"  )
	{
		if (Jays % 3 == 2 || ROB.size() == 8)
		{
			//skip
	
			stallissue = true;
			address--;
		}
		else
		{
		
			stallissue = false;
			ROB.push(currentline);
			lastadded.push(current_instruction);
			output.insert({ currentline , {cycle,0,0,0,0} });
			Jays++;
			order.push_back(currentline);
		}
	}
	else if (current_instruction == "BEQ" )
	{
		if (beq % 3 == 2 || ROB.size() == 8)
		{
			//skip
			stallissue = true;
			address--;
		}
		else
		{
			stallissue = false;
			ROB.push(currentline);
			beq++;
			output.insert({ currentline , {cycle,0,0,0,0} });
			order.push_back(currentline);

		}
	}
}
void execute()
{
	//updating status
	
	for (auto it2 = cycleissuing.begin(); it2 != cycleissuing.end(); it2++)
	{
		if (it2->second == cycle)
			instructionStatus[current_instruction] = "Executed";
	}
			

	if (reading && !stallissue)
	{
		if (current_instruction == "ADD")
		{
			string rd, rs1, rs2, inst;
			stringstream s(currentline);
			getline(s, inst, ' ');
			getline(s, rd, ',');
			getline(s, rs1, ',');
			getline(s, rs2);

			//do the adding
			auto it = registers.find(rs1);
			int temp1 = stoi(it->second);
			it = registers.find(rs2);
			int temp2 = stoi(it->second);
			int temp3 = temp2 + temp1;
			it = registers.find(rd);
			previousregisters[it->first] = it->second; // used in case of not writtingback 
			it->second = to_string(temp3);

			//start and end time 
			output[currentline][1] = cycle + 1;
			output[currentline][2] = cycle + 3;

			//cycleissuing
			cycleissuing.insert({ currentline , cycle + 3 });
		}
		else if (current_instruction == "SUB")
		{
			string rd, rs1, rs2, inst;
			stringstream s(currentline);
			getline(s, inst, ' ');
			getline(s, rd, ',');
			getline(s, rs1, ',');
			getline(s, rs2);

			//do the sub
			auto it = registers.find(rs1);
			int temp1 = stoi(it->second);
			it = registers.find(rs2);
			int temp2 = stoi(it->second);
			int temp3 = temp2 - temp1;
			it = registers.find(rd);
			previousregisters[it->first] = it->second; // used in case of not writtingback 
			it->second = to_string(temp3);

			//start and end time 
			output[currentline][1] = cycle + 1;
			output[currentline][2] = cycle + 3;

			//cycleissuing
			cycleissuing.insert({ currentline , cycle + 3 });
		}
		else if (current_instruction == "ADDI")
		{
			string rd, rs1, imm, inst;
			stringstream s(currentline);
			getline(s, inst, ' ');
			getline(s, rd, ',');
			getline(s, rs1, ',');
			getline(s, imm);

			//do the adding
			auto it = registers.find(rs1);
			int temp1 = stoi(it->second);
			int temp2 = stoi(imm);
			int temp3 = temp2 + temp1;
			it = registers.find(rd);
			previousregisters[it->first] = it->second;
			it->second = to_string(temp3);

			//start and end time 
			output[currentline][1] = cycle + 1;
			output[currentline][2] = cycle + 3;

			//cycleissuing
			cycleissuing.insert({ currentline , cycle + 3 });
		}
		else if (current_instruction == "NAND")
		{
			string rd, rs1, rs2, inst;
			stringstream s(currentline);
			getline(s, inst, ' ');
			getline(s, rd, ',');
			getline(s, rs1, ',');
			getline(s, rs2);

			//do the adding
			auto it = registers.find(rs1);
			int temp1 = stoi(it->second);
			it = registers.find(rs2);
			int temp2 = stoi(it->second);
			int temp3 = !(temp2 && temp1);
			it = registers.find(rd);
			previousregisters[it->first] = it->second; // used in case of not writtingback 
			it->second = to_string(temp3);

			//start and end time 
			output[currentline][1] = cycle + 1;
			output[currentline][2] = cycle + 2;

			//cycleissuing
			cycleissuing.insert({ currentline , cycle + 2 });
		}
		else if (current_instruction == "MUL")
		{
			string rd, rs1, rs2, inst;
			stringstream s(currentline);
			getline(s, inst, ' ');
			getline(s, rd, ',');
			getline(s, rs1, ',');
			getline(s, rs2);

			//do the sub
			auto it = registers.find(rs1);
			int temp1 = stoi(it->second);
			it = registers.find(rs2);
			int temp2 = stoi(it->second);
			int temp3 = temp2 * temp1;
			it = registers.find(rd);
			previousregisters[it->first] = it->second; // used in case of not writtingback 
			it->second = to_string(temp3);

			//start and end time 
			output[currentline][1] = cycle + 1;
			output[currentline][2] = cycle + 11;

			//cycleissuing
			cycleissuing.insert({ currentline , cycle + 11 });
		}
		else if (current_instruction == "LOAD")
		{
		string source, destination, offset ,inst;
		stringstream s(currentline);
		getline(s, inst, ' ');
		getline(s, destination, ',');
		getline(s, offset, '(');
		getline(s, source, ')');
		int resgister_address_value;
		for (auto i : registers) {
			if (i.first == source)
			{
				resgister_address_value = stoi(i.second);
				break;
			}
		}
		int destination_address_value = stoi(offset) + resgister_address_value;
		int value;
		for (auto i : datamemory) {
			if (i.first == destination_address_value)
			{
				value = stoi(i.second);
				break;
			}
		}
		auto it = registers.find(destination);
		previousregisters[it->first] = it->second;
		it->second = to_string(value);
		
		//start and end time 
		output[currentline][1] = cycle + 1;
		output[currentline][2] = cycle + 4;

		//cycleissuing
		cycleissuing.insert({ currentline , cycle + 4 });

        }
		else if (current_instruction == "STORE")
		{
		string source, destination, offset, inst;
		stringstream s(currentline);
		getline(s, inst, ' ');
		getline(s, source, ',');
		getline(s, offset, '(');
		getline(s, destination, ')');
		
		int source_value, destination_value;
		for (auto i : registers) {
			if (i.first == source)
			{
				source_value = stoi(i.second);
				break;
			}

		}
		for (auto i : registers) {
			if (i.first == destination)
			{
				destination_value = stoi(i.second);
				break;
			}

		}
		destination_value = destination_value + stoi(offset);
		map<int, string>::iterator i = datamemory.find(destination_value);

		if (i == datamemory.end())
			datamemory.insert({ destination_value,{to_string(source_value)}});
		else 
			i->second = to_string(source_value); //doesn't store old value of memory 


		//start and end time 
		output[currentline][1] = cycle + 1;
		output[currentline][2] = cycle + 4;

		//cycleissuing
		cycleissuing.insert({ currentline , cycle + 4 });

		}
		else if (current_instruction == "JMP")
		{
		    string inst,imm;
			stringstream s(currentline);
			getline(s, inst, ' ');
			getline(s, imm);
			address = address + stoi(imm) + 1; //doesn't store old address

			//start and end time 
			output[currentline][1] = cycle + 1;
			output[currentline][2] = cycle + 2;

			//cycleissuing
			cycleissuing.insert({ currentline , cycle + 2 });


		}
		else if (current_instruction == "BEQ")
		{
		string rd, rs1, rs2, imm, inst;
		stringstream s(currentline);
		getline(s, inst, ' ');
		getline(s, rs1, ',');
		getline(s, rs2,',');
		getline(s, imm);
		
		auto it = registers.find(rs1);
		int temp1 = stoi(it->second);
		it = registers.find(rs2);
		int temp2 = stoi(it->second);
		if (temp1 == temp2)
			address += stoi(imm) + 1;
	
		//start and end time 
		output[currentline][1] = cycle + 1;
		output[currentline][2] = cycle + 2;

		//cycleissuing
		cycleissuing.insert({ currentline , cycle + 2 });


		}
		else if (current_instruction == "JAL")
		{
		string inst, imm;
		stringstream s(currentline);
		getline(s, inst, ' ');
		getline(s, imm);
		
		
		previousregisters["R1"] = registers["R1"]; // used in case of not writtingback 
		registers["R1"] = to_string(address+1);

		address = address + stoi(imm) + 1; //doesn't store old address

		//start and end time 
		output[currentline][1] = cycle + 1;
		output[currentline][2] = cycle + 2;

		//cycleissuing
		cycleissuing.insert({ currentline , cycle + 2 });


		}
		else if (current_instruction == "RET")
		{
		 address =stoi( registers["R1"]);

		 //start and end time 
		 output[currentline][1] = cycle + 1;
		 output[currentline][2] = cycle + 2;

		 //cycleissuing
		 cycleissuing.insert({ currentline , cycle + 2 });


        }
		
	}
}
void writeback()
{
	//case of no writting back ?

	//update status 
	for (auto it = cyclewriting.begin(); it != cyclewriting.end(); it++)
	{
		if (it->second == cycle-1)
			instructionStatus[it->first] = "Written";
	}

	if (reading && !stallissue)
	{
		//cycle number 
		output[currentline][3] = output[currentline][2] + 1;

		//cyclewriting
		cyclewriting.insert({ currentline ,output[currentline][2] + 1 });
	}
	
}
void commit()
{
	while(!ROB.empty())
	{
		if (instructionStatus[ROB.front()] == "Written")
		{
			output[ROB.front()][4] = cycle ;
			if (lastadded.front() == "ADD" || lastadded.front() == "SUB" || lastadded.front() == "ADDI")
				arith--;
			else if (lastadded.front() == "NAND")
				nand--;
			else if (lastadded.front() == "MUL")
				mul--;
			else if (lastadded.front() == "LOAD")
				load--;
			else if (lastadded.front() == "STORE")
				stor--;
			else if (lastadded.front() == "JMP" || lastadded.front() == "JAL" || lastadded.front() == "RET")
				Jays--;
			else if (lastadded.front() == "BEQ")
				beq--;
	
			ROB.pop();
			lastadded.pop();
		}
		else
			break;


	}
		
}
void printResult()
{
	cout << "Instruction"<<setw(20)<<"Issue"<<setw(20)<<"Execute Start"<< setw(20) << "Execute End" << setw(20) << "Write" << setw(20)<<"Commite"<< endl;
	for (auto it = order.begin(); it != order.end(); it++)
	{

		cout << *it << " :"<<setw(15)<< output[*it][0] << setw(19) << output[*it][1] << setw(19) << output[*it][2] << setw(19) << output[*it][3] << setw(19) << output[*it][4] << endl;
	}
	cout << endl;
	cout << "Number of Cycles :  " << cycle+1 << endl;
	cout << "IPC :  " << IPC << endl;

}

int main()
{
	//prepare registers map 
	for (int i = 0; i < 8; i++)
	{
		string name = "R" + to_string(i);
		registers.insert(pair <string, string>(name, "0"));
		previousregisters.insert(pair <string, string>(name, "0"));
	}


	string instructionfile, memoryfile ,line;
	ifstream instructions ,memory;

	//get required data 
	cout << "Enter Assembly Program File name" << endl;
	cin >> instructionfile;
	cout << "Enter the starting address: " << endl;
	cin >> address;
	cout << "Enter Memory Data File name" << endl;
	cin >> memoryfile;

	//read and store instructions
	instructions.open(instructionfile);
	while (getline(instructions, line))
	{
		program.insert({ address ,line });
		address ++;
	}

	instructions.close();

	//prepare memory 
	memory.open(memoryfile);
	while (!memory.eof()) {
		string address, value;
		getline(memory, line);
		stringstream s(line);
		getline(s, address, ',');
		getline(s, value, ',');
		datamemory.insert({ stoi(address),value });
	}
	memory.close();



	auto pro = program.begin();

	while (pro != program.end()  || !ROB.empty())
	{

		if (pro != program.end())
		{
			reading = true;
			currentline = pro->second;
			stringstream s(currentline);
			getline(s, current_instruction, ' ');
			address = pro->first + 1;
			

			issue();
			pro = program.find(address);
		}
		else
			reading = false;

		
		execute();
		writeback();

		//2nd instruction same cycle
		if (pro != program.end())
		{
			reading = true;
			currentline = pro->second;
			stringstream s(currentline);
			getline(s, current_instruction, ' ');
			address = pro->first + 1;

			issue();
			pro = program.find(address);
		}
		else
			reading = false;


		execute();
		writeback();
		commit();
		
		
			cycle += 1;

	}
	IPC = float(numofinst) / float(cycle);
	printResult();

	return 0;
}





































