#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <exception>  // For exception handling
using namespace std;

#define MAX_ROOMS 100

// Customer class to store customer details
class Customer {
public:
    string name;
    string address;
    string phone;
    string from_date;
    string to_date;
    int days;
    float payment_advance;
    int booking_id;

    Customer() : name(""), address(""), phone(""), from_date(""), to_date(""), days(0), payment_advance(0), booking_id(0) {}
};

// Base Room class
class Room {
public:
    virtual void displayRoomDetails() const { 
        cout << "\nRoom Number: " << roomNumber;
        cout << "\nAC Type: " << ac;
        cout << "\nComfort Type: " << type;
        cout << "\nRent: " << rent;
        cout << "\nStatus: " << (status == 0 ? "Available" : "Reserved") << endl;
    }

    int roomNumber;
    int rent;
    int status;  // 0 for available, 1 for booked
    char ac;     // AC Type: 'A' for AC, 'N' for Non-AC
    char type;   // Comfort Type: 'S' for Standard, 'L' for Luxury

    Customer cust;

    Room() : roomNumber(0), rent(0), status(0), ac('N'), type('S') {}
};

// Derived StandardRoom class
class StandardRoom : public Room {
public:
    StandardRoom() {
        type = 'S';
    }
    void displayRoomDetails() const override {
        Room::displayRoomDetails();
        cout << "This is a Standard Room." << endl;
    }
};

// Derived LuxuryRoom class
class LuxuryRoom : public Room {
public:
    LuxuryRoom() {
        type = 'L';
    }
    void displayRoomDetails() const override {
        Room::displayRoomDetails();
        cout << "This is a Luxury Room with additional amenities!" << endl;
    }
};

// Template class to store a list of items (Rooms or Customers)
template <typename T>
class HotelList {
private:
    vector<T> list;
public:
    void addItem(const T& item) {
        list.push_back(item);
    }

    void displayItems() const {
        for (const auto& item : list) {
            item.displayRoomDetails();
        }
    }

    size_t size() const {
        return list.size();
    }

    T& operator[](size_t index) {
        return list[index];
    }
};

// Hotel Management class to handle room and customer management
class HotelMgnt {
private:
    HotelList<Room> rooms;
    unordered_map<string, int> customerVisitCount;  // Maps phone number to visit count

public:
    HotelMgnt() {}

    void addRoom();
    void searchRoom(int rno);
    void getAvailRoom();
    void checkIn();
    void checkOut(int roomNum);
    void searchCustomer(const string&);
    void guestSummaryReport();
    void applyDiscount(string& phone, float& billAmount);
    void saveRoomsToFile();
    void loadRoomsFromFile();
};

// Hotel Management methods
void HotelMgnt::addRoom() {
    int roomNumber;
    char roomType;

    // Ask for the room number first
    cout << "\nEnter Room Number: ";
    cin >> roomNumber;

    // Ask for the room type (Standard or Luxury)
    cout << "\nEnter Room Type (S for Standard / L for Luxury): ";
    cin >> roomType;

    // Create the appropriate room based on user input
    Room* newRoom = nullptr;
    if (roomType == 'S') {
        newRoom = new StandardRoom();
    } else if (roomType == 'L') {
        newRoom = new LuxuryRoom();
    } else {
        cout << "Invalid room type!" << endl;
        return;
    }

    // Set the room number and the initial status
    newRoom->roomNumber = roomNumber;
    cout << "Enter AC Type (A for AC / N for Non-AC): ";
    cin >> newRoom->ac;
    cout << "Enter Rent per Day: ";
    cin >> newRoom->rent;
    newRoom->status = 0;  // Initially available

    // Add the room to the hotel rooms list
    rooms.addItem(*newRoom);
    cout << "\nRoom Added Successfully!" << endl;
}

void HotelMgnt::searchRoom(int rno) {
    for (size_t i = 0; i < rooms.size(); ++i) {
        if (rooms[i].roomNumber == rno) {
            rooms[i].displayRoomDetails();
            return;
        }
    }
    cout << "\nRoom not found!" << endl;
}

void HotelMgnt::getAvailRoom() {
    bool found = false;
    for (size_t i = 0; i < rooms.size(); ++i) {
        if (rooms[i].status == 0) {
            rooms[i].displayRoomDetails();
            found = true;
        }
    }
    if (!found) {
        cout << "\nNo Available Rooms." << endl;
    }
}

void HotelMgnt::checkIn() {
    int rno;
    cout << "\nEnter Room Number to Check-in: ";
    cin >> rno;
    for (size_t i = 0; i < rooms.size(); ++i) {
        if (rooms[i].roomNumber == rno) {
            if (rooms[i].status == 1) {
                cout << "\nRoom is already Booked!" << endl;
                return;
            }

            cout << "\nEnter Customer Name: ";
            cin.ignore();
            getline(cin, rooms[i].cust.name);
            cout << "Enter Address: ";
            getline(cin, rooms[i].cust.address);
            cout << "Enter Phone Number: ";
            cin >> rooms[i].cust.phone;

            // Validate phone number (basic check for 10-digit length)
            while (rooms[i].cust.phone.length() != 10) {
                cout << "Invalid phone number. Enter a 10-digit phone number: ";
                cin >> rooms[i].cust.phone;
            }

            cout << "Enter From Date (YYYY-MM-DD): ";
            cin >> rooms[i].cust.from_date;
            cout << "Enter To Date (YYYY-MM-DD): ";
            cin >> rooms[i].cust.to_date;

            cout << "Enter Number of Days: ";
            cin >> rooms[i].cust.days;

            // Apply discount if eligible
            customerVisitCount[rooms[i].cust.phone]++;
            float billAmount = rooms[i].rent * rooms[i].cust.days;
            applyDiscount(rooms[i].cust.phone, billAmount);
            cout << "Total Bill: " << billAmount << endl;

            cout << "Enter Advance Payment: ";
            cin >> rooms[i].cust.payment_advance;

            rooms[i].status = 1;  // Mark room as reserved
            cout << "\nCustomer Checked-in Successfully!" << endl;
            return;
        }
    }
    cout << "\nRoom not found!" << endl;
}

void HotelMgnt::checkOut(int roomNum) {
    for (size_t i = 0; i < rooms.size(); ++i) {
        if (rooms[i].roomNumber == roomNum) {
            if (rooms[i].status == 0) {
                cout << "\nRoom is already vacant!" << endl;
                return;
            }

            float billAmount = rooms[i].rent * rooms[i].cust.days;
            applyDiscount(rooms[i].cust.phone, billAmount);
            cout << "\nCheckout Details:" << endl;
            cout << "Customer Name: " << rooms[i].cust.name << endl;
            cout << "Room Number: " << rooms[i].roomNumber << endl;
            cout << "Amount to Pay: " << billAmount - rooms[i].cust.payment_advance << endl;
            cout << "Advance Paid: " << rooms[i].cust.payment_advance << endl;

            rooms[i].status = 0;  // Room is now available
            cout << "Customer Checked-out Successfully!" << endl;
            return;
        }
    }
    cout << "\nRoom not found!" << endl;
}

void HotelMgnt::searchCustomer(const string& pname) {
    bool found = false;
    for (size_t i = 0; i < rooms.size(); ++i) {
        if (rooms[i].cust.name == pname) {
            cout << "\nCustomer Details:" << endl;
            cout << "Name: " << rooms[i].cust.name << endl;
            cout << "Room Number: " << rooms[i].roomNumber << endl;
            cout << "Phone number: " << rooms[i].cust.phone << endl;
            cout << "Address: " << rooms[i].cust.address << endl;
            cout << "Stay (Days): " << rooms[i].cust.days << endl;
            found = true;
        }
    }
    if (!found) {
        cout << "\nCustomer not found!" << endl;
    }
}

void HotelMgnt::guestSummaryReport() {
    if (rooms.size() == 0) {
        cout << "\nNo Guests in Hotel!" << endl;
    } else {
        for (size_t i = 0; i < rooms.size(); ++i) {
            if (rooms[i].status == 1) {
                cout << "\nCustomer Name: " << rooms[i].cust.name << endl;
                cout << "Room Number: " << rooms[i].roomNumber << endl;
                cout << "Address: " << rooms[i].cust.address << endl;
                cout << "Phone: " << rooms[i].cust.phone << endl;
                cout << "Stay (Days): " << rooms[i].cust.days << endl;
                cout << "------------------------------------" << endl;
            }
        }
    }
}

void HotelMgnt::applyDiscount(string& phone, float& billAmount) {
    if (customerVisitCount[phone] >= 2) {
        float discount = billAmount * 0.10;
        billAmount -= discount;
        cout << "\nDiscount Applied: 10%!" << endl;
    }
}

void HotelMgnt::saveRoomsToFile() {
    ofstream file("rooms.txt");
    for (size_t i = 0; i < rooms.size(); ++i) {
        file << rooms[i].roomNumber << " "
             << rooms[i].ac << " "
             << rooms[i].type << " "
             << rooms[i].rent << " "
             << rooms[i].status << endl;
    }
    file.close();
}

void HotelMgnt::loadRoomsFromFile() {
    ifstream file("rooms.txt");
    int rno, rent, status;
    char ac, type;
    while (file >> rno >> ac >> type >> rent >> status) {
        Room* room = nullptr;
        if (type == 'S') {
            room = new StandardRoom();
        } else if (type == 'L') {
            room = new LuxuryRoom();
        }

        if (room) {
            room->roomNumber = rno;
            room->ac = ac;
            room->type = type;
            room->rent = rent;
            room->status = status;
            rooms.addItem(*room);
        }
    }
    file.close();
}

// Main function to run the program
int main() {
    HotelMgnt hm;
    hm.loadRoomsFromFile();  // Load rooms from file at startup
    int choice;
    while (true) {
        cout << "\nHotel Management System";
        cout << "\n1. Add Room";
        cout << "\n2. Search Room";
        cout << "\n3. Show Available Rooms";
        cout << "\n4. Check-in";
        cout << "\n5. Check-out";
        cout << "\n6. Search Customer";
        cout << "\n7. Show Guest Summary";
        cout << "\n8. Exit";
        cout << "\nEnter your choice: ";
        cin >> choice;

        try {
            switch (choice) {
                case 1:
                    hm.addRoom();
                    break;
                case 2: {
                    int rno;
                    cout << "\nEnter Room Number: ";
                    cin >> rno;
                    hm.searchRoom(rno);
                    break;
                }
                case 3:
                    hm.getAvailRoom();
                    break;
                case 4:
                    hm.checkIn();
                    break;
                case 5: {
                    int rno;
                    cout << "\nEnter Room Number for Check-out: ";
                    cin >> rno;
                    hm.checkOut(rno);
                    break;
                }
                case 6: {
                    string pname;
                    cout << "\nEnter Customer Name to Search: ";
                    cin.ignore();
                    getline(cin, pname);
                    hm.searchCustomer(pname);
                    break;
                }
                case 7:
                    hm.guestSummaryReport();
                    break;
                case 8:
                    hm.saveRoomsToFile();  // Save room data before exiting
                    cout << "\nExiting...\n";
                    return 0;
                default:
                    cout << "\nInvalid choice. Please try again." << endl;
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
}