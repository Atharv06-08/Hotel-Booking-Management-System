#include <cctype>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Room {
private:
    int roomNumber;
    string roomType;
    bool occupied;
    double pricePerNight;

public:
    Room(int number, const string &type, double price)
        : roomNumber(number), roomType(type), occupied(false), pricePerNight(price) {}

    int getRoomNumber() const { return roomNumber; }
    string getRoomType() const { return roomType; }
    bool isOccupied() const { return occupied; }
    double getPricePerNight() const { return pricePerNight; }

    void setOccupied(bool status) { occupied = status; }
};

class Customer {
private:
    int customerId;
    string name;
    string contact;
    int stays;
    double totalSpent;

public:
    Customer(int id, const string &customerName, const string &customerContact)
        : customerId(id), name(customerName), contact(customerContact), stays(0), totalSpent(0.0) {}

    int getCustomerId() const { return customerId; }
    string getName() const { return name; }
    string getContact() const { return contact; }
    int getStays() const { return stays; }
    double getTotalSpent() const { return totalSpent; }
    void recordCompletedStay(double amountPaid) {
        ++stays;
        totalSpent += amountPaid;
    }
    void setHistory(int totalStays, double totalAmountSpent) {
        stays = totalStays;
        totalSpent = totalAmountSpent;
    }
};

class Booking {
private:
    int bookingId;
    int roomNumber;
    int customerId;
    string customerName;
    time_t checkInTime;
    bool active;

public:
    Booking(int id, int roomNo, int custId, const string &custName)
        : bookingId(id),
          roomNumber(roomNo),
          customerId(custId),
          customerName(custName),
          checkInTime(time(nullptr)),
          active(true) {}

    int getBookingId() const { return bookingId; }
    int getRoomNumber() const { return roomNumber; }
    int getCustomerId() const { return customerId; }
    string getCustomerName() const { return customerName; }
    time_t getCheckInTime() const { return checkInTime; }
    bool isActive() const { return active; }
    void close() { active = false; }
    void setCheckInTime(time_t timestamp) { checkInTime = timestamp; }
    void setActive(bool status) { active = status; }
};

class HotelBookingSystem {
private:
    static const int contentWidth = 58;
    vector<Room> rooms;
    vector<Customer> customers;
    vector<Booking> bookings;
    int nextCustomerId = 1;
    int nextBookingId = 1;
    const string roomsFile = "rooms_data.txt";
    const string customersFile = "customers_data.txt";
    const string bookingsFile = "bookings_data.txt";

    string normalizeRoomType(string type) const {
        for (char &ch : type) {
            ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
        }

        if (type == "standard") {
            return "Standard";
        }
        if (type == "deluxe") {
            return "Deluxe";
        }
        if (type == "suite") {
            return "Suite";
        }
        return "";
    }

    void printSectionHeader(const string &title) const {
        cout << "\n+----------------------------------------------------------+\n";
        string centeredTitle = title;
        if (static_cast<int>(title.size()) < 56) {
            int leftPadding = (56 - static_cast<int>(title.size())) / 2;
            centeredTitle = string(leftPadding, ' ') + title;
        }
        cout << "| " << setw(56) << left << centeredTitle << "|\n";
        cout << "+----------------------------------------------------------+\n";
    }

    void printDivider() const {
        cout << "------------------------------------------------------------\n";
    }

    string centeredLine(const string &text) const {
        if (static_cast<int>(text.size()) >= contentWidth) {
            return text;
        }
        int leftPadding = (contentWidth - static_cast<int>(text.size())) / 2;
        return string(leftPadding, ' ') + text;
    }

    void printCentered(const string &text) const { cout << centeredLine(text) << '\n'; }

    void printKeyValue(const string &label, const string &value) const {
        printCentered(label + ": " + value);
    }

    void printSpacer() const { cout << '\n'; }

    string formatMoney(double amount) const {
        ostringstream out;
        out << "Rs. " << fixed << setprecision(2) << amount;
        return out.str();
    }

    void showDashboard() const {
        int standardTotal = 0, deluxeTotal = 0, suiteTotal = 0;
        int standardFree = 0, deluxeFree = 0, suiteFree = 0;

        for (const Room &room : rooms) {
            if (room.getRoomType() == "Standard") {
                ++standardTotal;
                if (!room.isOccupied()) {
                    ++standardFree;
                }
            } else if (room.getRoomType() == "Deluxe") {
                ++deluxeTotal;
                if (!room.isOccupied()) {
                    ++deluxeFree;
                }
            } else if (room.getRoomType() == "Suite") {
                ++suiteTotal;
                if (!room.isOccupied()) {
                    ++suiteFree;
                }
            }
        }

        int totalRooms = static_cast<int>(rooms.size());
        int totalAvailable = standardFree + deluxeFree + suiteFree;
        int totalOccupied = totalRooms - totalAvailable;

        printSectionHeader("Front Desk Dashboard");
        printKeyValue("Total Rooms", to_string(totalRooms));
        printKeyValue("Available Rooms", to_string(totalAvailable));
        printKeyValue("Occupied Rooms", to_string(totalOccupied));
        int activeCount = 0;
        for (const Booking &booking : bookings) {
            if (booking.isActive()) {
                ++activeCount;
            }
        }
        printKeyValue("Active Bookings", to_string(activeCount));

        printCentered("Room Type Availability");
        printCentered("Type         Available   Total");
        printCentered("Standard     " + to_string(standardFree) + "          " +
                      to_string(standardTotal));
        printCentered("Deluxe       " + to_string(deluxeFree) + "          " + to_string(deluxeTotal));
        printCentered("Suite        " + to_string(suiteFree) + "          " + to_string(suiteTotal));
        printSpacer();
    }

    int getRoomIndexByNumber(int roomNumber) const {
        for (size_t i = 0; i < rooms.size(); ++i) {
            if (rooms[i].getRoomNumber() == roomNumber) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    int getActiveBookingIndexByRoom(int roomNumber) const {
        for (size_t i = 0; i < bookings.size(); ++i) {
            if (bookings[i].getRoomNumber() == roomNumber && bookings[i].isActive()) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    int findAvailableRoomByType(const string &type) const {
        for (size_t i = 0; i < rooms.size(); ++i) {
            if (!rooms[i].isOccupied() && rooms[i].getRoomType() == type) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    int getCustomerIndexById(int customerId) const {
        for (size_t i = 0; i < customers.size(); ++i) {
            if (customers[i].getCustomerId() == customerId) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    int getCustomerIndexByContact(const string &contact) const {
        for (size_t i = 0; i < customers.size(); ++i) {
            if (customers[i].getContact() == contact) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    string loyaltyLevelForStays(int stays) const {
        if (stays >= 6) {
            return "Gold";
        }
        if (stays >= 3) {
            return "Silver";
        }
        return "Normal";
    }

    double discountRateForStays(int stays) const {
        if (stays >= 6) {
            return 0.10;
        }
        if (stays >= 3) {
            return 0.05;
        }
        return 0.0;
    }

    int safeReadInt(const string &prompt) const {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            }
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    string safeReadLine(const string &prompt) const {
        string input;
        cout << prompt;
        getline(cin, input);
        return input;
    }

    int calculateNightsStayed(time_t checkIn) const {
        const int secondsPerDay = 60 * 60 * 24;
        time_t now = time(nullptr);
        int nights = static_cast<int>((now - checkIn) / secondsPerDay);
        return nights <= 0 ? 1 : nights;
    }

    bool parseLine(const string &line, vector<string> &parts, int expectedParts) const {
        parts.clear();
        string token;
        stringstream ss(line);
        while (getline(ss, token, '|')) {
            parts.push_back(token);
        }
        return static_cast<int>(parts.size()) == expectedParts;
    }

    bool loadRooms() {
        ifstream inFile(roomsFile);
        if (!inFile.is_open()) {
            return false;
        }

        vector<Room> loadedRooms;
        string line;
        vector<string> parts;
        while (getline(inFile, line)) {
            if (line.empty()) {
                continue;
            }
            if (!parseLine(line, parts, 4)) {
                continue;
            }

            int roomNumber = stoi(parts[0]);
            const string roomType = parts[1];
            double pricePerNight = stod(parts[2]);
            bool occupied = (parts[3] == "1");

            loadedRooms.emplace_back(roomNumber, roomType, pricePerNight);
            loadedRooms.back().setOccupied(occupied);
        }

        if (!loadedRooms.empty()) {
            rooms = loadedRooms;
            return true;
        }
        return false;
    }

    void loadCustomers() {
        ifstream inFile(customersFile);
        if (!inFile.is_open()) {
            return;
        }

        vector<Customer> loadedCustomers;
        string line;
        vector<string> parts;
        while (getline(inFile, line)) {
            if (line.empty()) {
                continue;
            }
            if (!parseLine(line, parts, 5)) {
                continue;
            }

            int customerId = stoi(parts[0]);
            const string customerName = parts[1];
            const string customerContact = parts[2];
            int stays = stoi(parts[3]);
            double totalSpent = stod(parts[4]);

            loadedCustomers.emplace_back(customerId, customerName, customerContact);
            loadedCustomers.back().setHistory(stays, totalSpent);
        }

        customers = loadedCustomers;
        nextCustomerId = 1;
        for (const Customer &customer : customers) {
            if (customer.getCustomerId() >= nextCustomerId) {
                nextCustomerId = customer.getCustomerId() + 1;
            }
        }
    }

    void loadBookings() {
        ifstream inFile(bookingsFile);
        if (!inFile.is_open()) {
            return;
        }

        vector<Booking> loadedBookings;
        string line;
        vector<string> parts;
        while (getline(inFile, line)) {
            if (line.empty()) {
                continue;
            }
            if (!parseLine(line, parts, 6)) {
                continue;
            }

            int bookingId = stoi(parts[0]);
            int roomNumber = stoi(parts[1]);
            int customerId = stoi(parts[2]);
            const string customerName = parts[3];
            time_t checkInTime = static_cast<time_t>(stoll(parts[4]));
            bool active = (parts[5] == "1");

            Booking booking(bookingId, roomNumber, customerId, customerName);
            booking.setCheckInTime(checkInTime);
            booking.setActive(active);
            loadedBookings.push_back(booking);
        }

        bookings = loadedBookings;
        nextBookingId = 1;
        for (const Booking &booking : bookings) {
            if (booking.getBookingId() >= nextBookingId) {
                nextBookingId = booking.getBookingId() + 1;
            }
        }
    }

    void saveRooms() const {
        ofstream outFile(roomsFile);
        for (const Room &room : rooms) {
            outFile << room.getRoomNumber() << "|" << room.getRoomType() << "|" << fixed
                    << setprecision(2) << room.getPricePerNight() << "|"
                    << (room.isOccupied() ? 1 : 0) << "\n";
        }
    }

    void saveCustomers() const {
        ofstream outFile(customersFile);
        for (const Customer &customer : customers) {
            outFile << customer.getCustomerId() << "|" << customer.getName() << "|"
                    << customer.getContact() << "|" << customer.getStays() << "|" << fixed
                    << setprecision(2) << customer.getTotalSpent() << "\n";
        }
    }

    void saveBookings() const {
        ofstream outFile(bookingsFile);
        for (const Booking &booking : bookings) {
            outFile << booking.getBookingId() << "|" << booking.getRoomNumber() << "|"
                    << booking.getCustomerId() << "|" << booking.getCustomerName() << "|"
                    << static_cast<long long>(booking.getCheckInTime()) << "|"
                    << (booking.isActive() ? 1 : 0) << "\n";
        }
    }

    void loadData() {
        bool loadedRooms = loadRooms();
        if (!loadedRooms) {
            seedRooms();
        }
        loadCustomers();
        loadBookings();
    }

    void saveData() const {
        saveRooms();
        saveCustomers();
        saveBookings();
    }

public:
    void seedRooms() {
        // Standard rooms
        rooms.emplace_back(101, "Standard", 2200.0);
        rooms.emplace_back(102, "Standard", 2200.0);
        rooms.emplace_back(103, "Standard", 2200.0);
        rooms.emplace_back(104, "Standard", 2200.0);
        rooms.emplace_back(105, "Standard", 2200.0);
        rooms.emplace_back(106, "Standard", 2200.0);
        rooms.emplace_back(107, "Standard", 2200.0);
        rooms.emplace_back(108, "Standard", 2200.0);

        // Deluxe rooms
        rooms.emplace_back(201, "Deluxe", 3500.0);
        rooms.emplace_back(202, "Deluxe", 3500.0);
        rooms.emplace_back(203, "Deluxe", 3500.0);
        rooms.emplace_back(204, "Deluxe", 3500.0);
        rooms.emplace_back(205, "Deluxe", 3500.0);
        rooms.emplace_back(206, "Deluxe", 3500.0);

        // Suite rooms
        rooms.emplace_back(301, "Suite", 6000.0);
        rooms.emplace_back(302, "Suite", 6000.0);
        rooms.emplace_back(303, "Suite", 6000.0);
        rooms.emplace_back(304, "Suite", 6000.0);
    }

    void showRooms() const {
        printSectionHeader("Room Information");
        printCentered("Room No    Type         Price/Night    Status");
        for (const Room &room : rooms) {
            ostringstream row;
            row << left << setw(10) << room.getRoomNumber() << setw(12) << room.getRoomType()
                << setw(13) << fixed << setprecision(2) << room.getPricePerNight()
                << (room.isOccupied() ? "Occupied" : "Available");
            printCentered(row.str());
        }
        printSpacer();
    }

    void showAvailableRooms() const {
        printSectionHeader("Available Rooms");
        bool found = false;
        printCentered("Room No    Type         Rate/Night");
        for (const Room &room : rooms) {
            if (!room.isOccupied()) {
                found = true;
                ostringstream row;
                row << left << setw(10) << room.getRoomNumber() << setw(12) << room.getRoomType()
                    << "Rs. " << fixed << setprecision(2) << room.getPricePerNight();
                printCentered(row.str());
            }
        }
        if (!found) {
            printCentered("No available rooms at the moment.");
        }
        printSpacer();
    }

    void checkIn() {
        printSectionHeader("Check-In");
        showAvailableRooms();
        printCentered("Please enter guest details to proceed with check-in.");

        string guestName = safeReadLine("Enter customer name (as per ID): ");
        string guestContact = safeReadLine("Enter contact number: ");
        string requestedTypeInput = safeReadLine("Enter room type (Standard/Deluxe/Suite): ");
        string requestedType = normalizeRoomType(requestedTypeInput);

        if (requestedType.empty()) {
            cout << "Invalid room type selected. Please choose Standard, Deluxe, or Suite.\n";
            return;
        }

        int roomIndex = findAvailableRoomByType(requestedType);
        if (roomIndex == -1) {
            cout << "No available rooms for the selected room type.\n";
            return;
        }

        int customerIndex = getCustomerIndexByContact(guestContact);
        int customerId;
        if (customerIndex == -1) {
            customerId = nextCustomerId++;
            customers.emplace_back(customerId, guestName, guestContact);
            printCentered("New customer profile created.");
        } else {
            customerId = customers[customerIndex].getCustomerId();
            printCentered("Welcome back, " + customers[customerIndex].getName() + "! You are a " +
                          loyaltyLevelForStays(customers[customerIndex].getStays()) + " member.");
            printKeyValue("Previous stays", to_string(customers[customerIndex].getStays()));
        }

        int bookingId = nextBookingId++;
        bookings.emplace_back(bookingId, rooms[roomIndex].getRoomNumber(), customerId, guestName);
        rooms[roomIndex].setOccupied(true);
        saveData();

        printSectionHeader("Check-In Confirmation");
        printKeyValue("Status", "Check-in successful");
        printKeyValue("Booking ID", to_string(bookingId));
        printKeyValue("Customer ID", to_string(customerId));
        printKeyValue("Assigned Room", to_string(rooms[roomIndex].getRoomNumber()) + " (" +
                                           rooms[roomIndex].getRoomType() + ")");
        printCentered("Check-in successful. Room " + to_string(rooms[roomIndex].getRoomNumber()) +
                      " assigned.");
        printSpacer();
    }

    void checkOut() {
        printSectionHeader("Check-Out");
        printCentered("Please provide booking details to complete check-out.");
        int roomNumber = safeReadInt("Enter room number for check-out: ");

        int roomIndex = getRoomIndexByNumber(roomNumber);
        if (roomIndex == -1) {
            printCentered("Invalid room number. Room not found in records.");
            return;
        }
        if (!rooms[roomIndex].isOccupied()) {
            printCentered("This room is already marked as available.");
            return;
        }

        int bookingIndex = getActiveBookingIndexByRoom(roomNumber);
        if (bookingIndex == -1) {
            printCentered("No active booking found for this room.");
            return;
        }

        int actualNights = calculateNightsStayed(bookings[bookingIndex].getCheckInTime());

        double ratePerNight = rooms[roomIndex].getPricePerNight();
        double baseBill = actualNights * ratePerNight;
        int customerIndex = getCustomerIndexById(bookings[bookingIndex].getCustomerId());
        if (customerIndex == -1) {
            printCentered("Customer record not found for this booking.");
            return;
        }

        int updatedStayCount = customers[customerIndex].getStays() + 1;
        string loyaltyTier = loyaltyLevelForStays(updatedStayCount);
        double discountRate = discountRateForStays(updatedStayCount);
        double discountAmount = baseBill * discountRate;
        double finalBill = baseBill - discountAmount;

        printSectionHeader("Billing");
        printKeyValue("Booking ID", to_string(bookings[bookingIndex].getBookingId()));
        printKeyValue("Guest Name", bookings[bookingIndex].getCustomerName());
        printKeyValue("Customer ID", to_string(bookings[bookingIndex].getCustomerId()));
        printKeyValue("Room Number", to_string(roomNumber));
        printKeyValue("Room Type", rooms[roomIndex].getRoomType());
        printKeyValue("Nights Stayed", to_string(actualNights));
        printKeyValue("Rate/Night", formatMoney(ratePerNight));
        printKeyValue("Base Bill", formatMoney(baseBill));
        printKeyValue("Loyalty Tier", loyaltyTier);
        printKeyValue("Discount", to_string(static_cast<int>(discountRate * 100)) + "% (" +
                                    formatMoney(discountAmount) + ")");
        printKeyValue("Total Bill", formatMoney(finalBill));
        printSpacer();

        customers[customerIndex].recordCompletedStay(finalBill);

        rooms[roomIndex].setOccupied(false);
        bookings[bookingIndex].close();
        saveData();

        printSectionHeader("Check-Out Status");
        printKeyValue("Status", "Check-out completed successfully");
        printKeyValue("Room Status", "Room is now available");
        printCentered("Thank you for staying with us.");
        printSpacer();
    }

    void showCustomers() const {
        printSectionHeader("Customer Information");
        if (customers.empty()) {
            printCentered("No customers recorded yet.");
            return;
        }
        printCentered("Customer ID   Name                 Contact         Stays  Tier      Total Spent");
        for (const Customer &customer : customers) {
            ostringstream row;
            row << left << setw(12) << customer.getCustomerId() << setw(20) << customer.getName()
                << setw(16) << customer.getContact() << setw(7) << customer.getStays()
                << setw(10) << loyaltyLevelForStays(customer.getStays()) << fixed << setprecision(2)
                << customer.getTotalSpent();
            printCentered(row.str());
        }
        printSpacer();
    }

    void showActiveBookings() const {
        printSectionHeader("Active Bookings");
        bool found = false;
        printCentered("Booking ID   Room      Customer               Customer ID");
        for (const Booking &booking : bookings) {
            if (booking.isActive()) {
                found = true;
                ostringstream row;
                row << left << setw(12) << booking.getBookingId() << setw(10)
                    << booking.getRoomNumber() << setw(22) << booking.getCustomerName()
                    << booking.getCustomerId();
                printCentered(row.str());
            }
        }
        if (!found) {
            printCentered("No active bookings currently.");
        }
        printSpacer();
    }

    void run() {
        loadData();
        printSectionHeader("Welcome");
        printCentered("Welcome to the Hotel Booking Management System.");
        printSpacer();
        while (true) {
            showDashboard();
            printSectionHeader("Main Menu");
            printCentered("1. Show All Rooms");
            printCentered("2. Show Available Rooms");
            printCentered("3. Check-In Customer");
            printCentered("4. Check-Out Customer");
            printCentered("5. View Customer Records");
            printCentered("6. View Active Bookings");
            printCentered("0. Exit");

            int choice = safeReadInt("Enter your choice (0-6): ");

            switch (choice) {
            case 1:
                showRooms();
                break;
            case 2:
                showAvailableRooms();
                break;
            case 3:
                checkIn();
                break;
            case 4:
                checkOut();
                break;
            case 5:
                showCustomers();
                break;
            case 6:
                showActiveBookings();
                break;
            case 0:
                saveData();
                printCentered("Exiting system. Goodbye.");
                return;
            default:
                printCentered("Invalid choice. Please select a valid option (0-6).");
            }
        }
    }
};

int main() {
    HotelBookingSystem system;
    system.run();
    return 0;
}
