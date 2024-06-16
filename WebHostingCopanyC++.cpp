#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <fstream>
#include <sstream>

class HostingPlan {
public:
    int id;
    std::string name;
    double price;

    HostingPlan(int id, const std::string &name, double price) : id(id), name(name), price(price) {}

    std::string serialize() const {
        std::ostringstream oss;
        oss << id << " " << name << " " << price;
        return oss.str();
    }

    static HostingPlan deserialize(const std::string &data) {
        std::istringstream iss(data);
        int id;
        std::string name;
        double price;
        iss >> id >> name >> price;
        return HostingPlan(id, name, price);
    }
};

class User {
public:
    int id;
    std::string username;
    std::string password;
    std::vector<HostingPlan> purchasedPlans;

    User(int id, const std::string &username, const std::string &password)
        : id(id), username(username), password(password) {}

    std::string serialize() const {
        std::ostringstream oss;
        oss << id << " " << username << " " << password;
        for (const auto &plan : purchasedPlans) {
            oss << " " << plan.id;
        }
        return oss.str();
    }

    static User deserialize(const std::string &data, const std::vector<HostingPlan> &allPlans) {
        std::istringstream iss(data);
        int id;
        std::string username, password;
        iss >> id >> username >> password;
        User user(id, username, password);
        int planId;
        while (iss >> planId) {
            auto it = std::find_if(allPlans.begin(), allPlans.end(), [planId](const HostingPlan &plan) {
                return plan.id == planId;
            });
            if (it != allPlans.end()) {
                user.purchasedPlans.push_back(*it);
            }
        }
        return user;
    }
};

class WebHostingCompany {
private:
    std::vector<User> users;
    std::vector<HostingPlan> hostingPlans;
    std::stack<int> loggedInUsers;
    std::queue<int> purchaseQueue;

    int userIdCounter = 1;
    int planIdCounter = 1;
    const std::string dataFileName = "company_data.txt";

public:
    WebHostingCompany() {
        loadData();
    }

    ~WebHostingCompany() {
        saveData();
    }

    void registerUser(const std::string &username, const std::string &password) {
        users.emplace_back(userIdCounter++, username, password);
        std::cout << "User registered successfully.\n";
    }

    bool loginUser(const std::string &username, const std::string &password) {
        for (const auto &user : users) {
            if (user.username == username && user.password == password) {
                loggedInUsers.push(user.id);
                std::cout << "Login successful.\n";
                return true;
            }
        }
        std::cout << "Invalid username or password.\n";
        return false;
    }

    void logoutUser() {
        if (!loggedInUsers.empty()) {
            loggedInUsers.pop();
            std::cout << "Logout successful.\n";
        } else {
            std::cout << "No user is currently logged in.\n";
        }
    }

    void addHostingPlan(const std::string &name, double price) {
        hostingPlans.emplace_back(planIdCounter++, name, price);
        std::cout << "Hosting plan added successfully.\n";
    }

    void viewHostingPlans() const {
        std::cout << "Available Hosting Plans:\n";
        for (const auto &plan : hostingPlans) {
            std::cout << "ID: " << plan.id << ", Name: " << plan.name << ", Price: $" << plan.price << "\n";
        }
    }

    void purchasePlan(int planId) {
        if (loggedInUsers.empty()) {
            std::cout << "You need to login first.\n";
            return;
        }

        int userId = loggedInUsers.top();
        auto userIt = std::find_if(users.begin(), users.end(), [userId](const User &user) {
            return user.id == userId;
        });

        if (userIt != users.end()) {
            auto planIt = std::find_if(hostingPlans.begin(), hostingPlans.end(), [planId](const HostingPlan &plan) {
                return plan.id == planId;
            });

            if (planIt != hostingPlans.end()) {
                userIt->purchasedPlans.push_back(*planIt);
                purchaseQueue.push(planIt->id);
                std::cout << "Hosting plan purchased successfully.\n";
            } else {
                std::cout << "Hosting plan not found.\n";
            }
        } else {
            std::cout << "User not found.\n";
        }
    }

    void viewPurchasedPlans() const {
        if (loggedInUsers.empty()) {
            std::cout << "You need to login first.\n";
            return;
        }

        int userId = loggedInUsers.top();
        auto userIt = std::find_if(users.begin(), users.end(), [userId](const User &user) {
            return user.id == userId;
        });

        if (userIt != users.end()) {
            std::cout << "Purchased Hosting Plans:\n";
            for (const auto &plan : userIt->purchasedPlans) {
                std::cout << "ID: " << plan.id << ", Name: " << plan.name << ", Price: $" << plan.price << "\n";
            }
        } else {
            std::cout << "User not found.\n";
        }
    }

    void viewAllUsers() const {
        std::cout << "Registered Users:\n";
        for (const auto &user : users) {
            std::cout << "ID: " << user.id << ", Username: " << user.username << "\n";
        }
    }

    void editUser(int userId, const std::string &newUsername, const std::string &newPassword) {
        auto userIt = std::find_if(users.begin(), users.end(), [userId](const User &user) {
            return user.id == userId;
        });

        if (userIt != users.end()) {
            userIt->username = newUsername;
            userIt->password = newPassword;
            std::cout << "User details updated successfully.\n";
        } else {
            std::cout << "User not found.\n";
        }
    }

    void deleteUser(int userId) {
        auto userIt = std::remove_if(users.begin(), users.end(), [userId](const User &user) {
            return user.id == userId;
        });

        if (userIt != users.end()) {
            users.erase(userIt, users.end());
            std::cout << "User deleted successfully.\n";
        } else {
            std::cout << "User not found.\n";
        }
    }

private:
    void saveData() {
        std::ofstream outFile(dataFileName);
        outFile << userIdCounter << "\n" << planIdCounter << "\n";

        for (const auto &user : users) {
            outFile << user.serialize() << "\n";
        }

        for (const auto &plan : hostingPlans) {
            outFile << plan.serialize() << "\n";
        }

        outFile.close();
    }

    void loadData() {
        std::ifstream inFile(dataFileName);
        if (!inFile) {
            return;
        }

        inFile >> userIdCounter >> planIdCounter;
        inFile.ignore();

        std::string line;
        while (std::getline(inFile, line)) {
            if (line.find(' ') == std::string::npos) {
                continue;
            }
            std::istringstream iss(line);
            int id;
            std::string name;
            double price;

            if (iss >> id >> name >> price) {
                hostingPlans.emplace_back(id, name, price);
            } else {
                users.push_back(User::deserialize(line, hostingPlans));
            }
        }

        inFile.close();
    }
};

int main() {
    WebHostingCompany company;
    int choice;
    std::string username, password, planName;
    double planPrice;
    int planId, userId;
    std::string newUsername, newPassword;

    while (true) {
        std::cout << "\n=== Web Hosting Company Management ===\n";
        std::cout << "1. Register\n2. Login\n3. Logout\n4. Add Hosting Plan\n5. View Hosting Plans\n";
        std::cout << "6. Purchase Hosting Plan\n7. View Purchased Plans\n8. View All Users\n";
        std::cout << "9. Edit User\n10. Delete User\n11. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                std::cout << "Enter username: ";
                std::cin >> username;
                std::cout << "Enter password: ";
                std::cin >> password;
                company.registerUser(username, password);
                break;
            case 2:
                std::cout << "Enter username: ";
                std::cin >> username;
                std::cout << "Enter password: ";
                std::cin >> password;
                company.loginUser(username, password);
                break;
            case 3:
                company.logoutUser();
                break;
            case 4:
                std::cout << "Enter hosting plan name: ";
                std::cin >> planName;
                std::cout << "Enter hosting plan price: ";
                std::cin >> planPrice;
                company.addHostingPlan(planName, planPrice);
                break;
            case 5:
                company.viewHostingPlans();
                break;
            case 6:
                std::cout << "Enter hosting plan ID to purchase: ";
                std::cin >> planId;
                company.purchasePlan(planId);
                break;
            case 7:
                company.viewPurchasedPlans();
                break;
            case 8:
                company.viewAllUsers();
                break;
            case 9:
                std::cout << "Enter user ID to edit: ";
                std::cin >> userId;
                std::cout << "Enter new username: ";
                std::cin >> newUsername;
                std::cout << "Enter new password: ";
                std::cin >> newPassword;
                company.editUser(userId, newUsername, newPassword);
                break;
            case 10:
                std::cout << "Enter user ID to delete: ";
                std::cin >> userId;
                company.deleteUser(userId);
                break;
            case 11:
                return 0;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
