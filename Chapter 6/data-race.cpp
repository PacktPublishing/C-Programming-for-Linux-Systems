#include <iostream>
#include <thread>
#include <syncstream>

using namespace std::literals::chrono_literals;

void Print(std::string_view what, std::string_view who, unsigned amount)
{
    std::osyncstream{std::cout} << std::this_thread::get_id() << " " << who
                                << what 
                                << amount << '\n';
}

struct Account {
    Account(std::string_view the_owner, unsigned the_amount) noexcept : 
        balance{the_amount}, owner{the_owner} {} 
    
    Account& operator+=(unsigned amount) noexcept {
        Print(" balance before depositing: ", owner, balance);
        auto temp{balance};
        std::this_thread::sleep_for(1ms);
        balance = temp + amount;
        Print(" balance after depositing: ", owner, balance);
        return *this;
    }

    Account& operator-=(unsigned amount) noexcept {
        Print(" balance before withdrawing: ", owner, balance);
        auto temp{balance};
        balance = temp - amount;
        Print(" balance after withdrawing: ", owner, balance);
        return *this;
    }

    std::string GetBalance() const {
        return "Current account balance of " + owner + 
                " is " + std::to_string(balance) + '\n';
    }
private:
    unsigned balance;
    std::string owner;
};

void TransferMoney(unsigned amount, Account& from, Account& to) {
    from -= amount;
    to += amount;
}

int main() {
    Account bill_account{"Bill", 100};
    Account john_account{"John", 50};

    std::jthread first_transfer{[&](){ TransferMoney(10, bill_account, john_account); }};
    std::jthread second_transfer{[&](){ TransferMoney(20, bill_account, john_account); }};

    std::this_thread::sleep_for(100ms);
    std::cout << bill_account.GetBalance();
    std::cout << john_account.GetBalance();
    
    return 0;
}