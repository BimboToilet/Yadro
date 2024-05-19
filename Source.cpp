#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <list>
#include <vector>

int CompareDates(std::string firstDate, std::string secondDate) {
	if (stoi(firstDate.substr(0, 2)) * 60 + stoi(firstDate.substr(3, 2)) > stoi(secondDate.substr(0, 2)) * 60 + stoi(secondDate.substr(3, 2))) {
		return 0;
	}
	else if (stoi(firstDate.substr(0, 2)) * 60 + stoi(firstDate.substr(3, 2)) == stoi(secondDate.substr(0, 2)) * 60 + stoi(secondDate.substr(3, 2))) {
		return 1;
	}
	return 2;
}

int TimeToMins(std::string time) {
	return stoi(time.substr(0, 2)) * 60 + stoi(time.substr(3, 2));
}

std::string MinsToTime(int time) {
	std::string answer;
	int hours = time / 60;
	time -= (time / 60) * 60;
	int minutes = time;
	if (hours<10) {
		answer.push_back('0');
	}
	answer += std::to_string(hours);
	answer.push_back(':');
	if (minutes < 10) {
		answer.push_back('0');
	}
	answer += std::to_string(minutes);
	return answer;
}

class Table {
private:
	int total_time;
	std::string start_time;
	bool isOccupied;
	std::string visitor;
public:
	Table() {
		total_time = 0;
		isOccupied = false;
		visitor = "";
	}
	int getTime() {
		if (!isOccupied) {
			return total_time;
		}
		return -1;
	}
	std::string getVisitorName() {
		return visitor;
	}
	void setVisitor(std::string client, std::string time) {
		isOccupied = true;
		start_time = time;
		visitor = client;
	}
	bool getOccupation() {
		return isOccupied;
	}
	void visitorOut(std::string time) {
		isOccupied = false;
		total_time += TimeToMins(time) - TimeToMins(start_time);
		visitor = "";
	}
};

class Client {
private:
	std::string name;
	int table;
public:
	Client(std::string string) {
		name = string;
		table = 0;
	}
	std::string getName() {
		return name;
	}
	int getTable() {
		return table;
	}
	void setTable(int number) {
		table = number;
	}
};

class Event {
private:
	std::string time;
	int id;
	std::string body;

public:
	Event(std::string string, std::list<Client>& clientsArr, std::vector<Table>& tablesArr, std::list<Client>& queue, std::string start, std::string end, std::ofstream& out) {
		StringParser(string);
		Print(out);
		if (id == 1) {
			if (CompareDates(time, start) == 2 || CompareDates(time,end)<2) {
				Event(time + " " + std::to_string(13) + " " + "NotOpenYet", clientsArr, tablesArr, queue, start, end, out);
			}
			else {
				for (auto i = clientsArr.begin(); i != clientsArr.end(); i++) {
					if ((*i).getName() == body) {
						Event(time + " " + std::to_string(13) + " " + "YouShallNotPass", clientsArr, tablesArr, queue, start, end, out);
						break;
					}
				}
				clientsArr.push_back(Client(body));
			}
		}
		else if (id == 2) {
			bool isFine = false;
			std::stringstream ss(body);
			std::string client;
			int table;
			ss >> client;
			ss >> table;
			for (auto i = clientsArr.begin(); i != clientsArr.end(); i++) {
				if ((*i).getName() == client) {
					isFine = true;
					break;
				}
			}
			if (!isFine) {
				Event(time + " " + std::to_string(13) + " " + "ClientUnknown", clientsArr, tablesArr, queue, start, end, out);
			}
			else {
				if (tablesArr[table - 1].getOccupation()) {
					Event(time + " " + std::to_string(13) + " " + "PlaceIsBusy", clientsArr, tablesArr, queue, start, end, out);
				}
				else {
					for (auto j = clientsArr.begin(); j != clientsArr.end(); j++) {
						if ((*j).getName() == client) {
							if (j->getTable() != 0) {
								tablesArr[j->getTable()-1].visitorOut(time);
							}
							j->setTable(table);
							break;
						}
					}
					tablesArr[table - 1].setVisitor(client, time);
				}
			}
		}
		else if (id == 3) {
			bool isFine = true;
			for (size_t i = 0; i < tablesArr.size(); i++) {
				if (!tablesArr[i].getOccupation()) {
					isFine = false;
					break;
				}
			}
			if (!isFine) {
				Event(time + " " + std::to_string(13) + " " + "ICanWaitNoLonger!", clientsArr, tablesArr, queue, start, end, out);
			}
			else {
				if (queue.size() > tablesArr.size()) {
					Event(time + " " + std::to_string(11) + " " + body, clientsArr, tablesArr, queue, start, end, out);
				}
				else {
					queue.push_back(Client(body));
				}
			}
		}
		else if (id == 4) {
			bool isFine = false;
			int number = -1;
			for (auto i = clientsArr.begin(); i != clientsArr.end(); i++) {
				if ((*i).getName() == body) {
					isFine = true;
					break;
				}
			}
			if (!isFine) {
				Event(time + " " + std::to_string(13) + " " + "ClientUnknown", clientsArr, tablesArr, queue, start, end, out);
			}
			else {
				for (int i = 0; i < tablesArr.size(); i++) {
					if (tablesArr[i].getVisitorName() == body) {
						tablesArr[i].visitorOut(time);
						number = i;
						break;
					}
				}
				for (auto i = clientsArr.begin(); i != clientsArr.end(); i++) {
					if (i->getName() == body) {
						clientsArr.erase(i);
						break;
					}
				}
				if (!queue.empty() && number!=-1) {
					Event(time + " " + std::to_string(12) + " " + queue.begin()->getName() + " " + std::to_string(number+1), clientsArr, tablesArr, queue, start, end, out);
				}
			}
		}
		else if (id == 11) {
			for (size_t i = 0; i < tablesArr.size(); i++) {
				if (tablesArr[i].getVisitorName() == body) {
					tablesArr[i].visitorOut(time);
					break;
				}
			}
			for (auto i = clientsArr.begin(); i != clientsArr.end(); i++) {
				if (i->getName() == body) {
					clientsArr.erase(i);
					break;
				}
			}
		}
		else if (id == 12) {
			std::stringstream ss(body);
			std::string client;
			int table;
			ss >> client;
			ss >> table;
			tablesArr[table - 1].setVisitor(client, time);
			for (auto j = clientsArr.begin(); j != clientsArr.end(); j++) {
				if ((*j).getName() == client) {
					j->setTable(table);
					break;
				}
			}
			queue.pop_front();
		}
	}
	void StringParser(std::string input) {
		std::stringstream ss(input);
		ss >> time;
		ss >> id;
		body = input.substr((time+std::to_string(id)).length()+2);
	}
	void Print(std::ofstream& out) {
		out << time << " " << id << " " << body << std::endl;
	}
};

bool cmp(Client& a, Client& b) {
	if (a.getName()<b.getName()) {
		return true;
	}
	return false;
}

int main(int argc, char** argv) {
	if (argc<2) {
		return -1;
	}
	std::ifstream fin(argv[1]);
	std::ofstream fout;
	fout.open("answer.txt");
	if (fin.is_open() && fout.is_open())
	{
		std::string line;
		std::getline(fin, line);
		int tables_count = stoi(line);
		std::vector<Table> tablesArr = std::vector<Table>();
		for (int i = 0; i < tables_count;i++) {
			tablesArr.push_back(Table());
		}
		std::string start;
		std::string end;
		std::getline(fin, line);
		std::stringstream ss(line);
		ss >> start;
		ss >> end;
		std::getline(fin, line);
		int payment = stoi(line);
		std::list<Client> clientsArr;
		std::list<Client> queue;
		fout << start << std::endl;
		while (std::getline(fin, line))
		{
			Event(line, clientsArr, tablesArr, queue, start, end, fout);
		}
		clientsArr.sort(cmp);
		while (!clientsArr.empty()) {
			Event(end + " " + std::to_string(11) + " " + clientsArr.begin()->getName(), clientsArr, tablesArr, queue, start, end, fout);
		}
		fout << end << std::endl;
		for (size_t i = 0; i < tablesArr.size(); i++) {
			fout << i+1 << " " << ((tablesArr[i].getTime() / 60) + ((tablesArr[i].getTime() - (tablesArr[i].getTime() / 60) * 60) % 60 > 0)) * payment << " " << MinsToTime(tablesArr[i].getTime()) << std::endl;
		}
	}
	fin.close();
	fout.close();
	return 0;
}