#include <bits/stdc++.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <cstdio>
#include <thread>
#include <random>

using namespace std;

string key = "EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn";
string cleos_cmd = "docker exec -it rewardtoken_nodeosd_1 /opt/eosio/bin/cleos -u http://nodeosd:8888 --wallet-url http://keosd:8900";
string eosiocpp_cmd = "docker exec -it rewardtoken_nodeosd_1 eosiocpp";
map<string, double> equity_balances;
map<string, double> reward_balances;

string generate(int max_length){
    string possible_characters = "12345abcdefghijklmnopqrstuvwxyz";
    random_device rd;
    mt19937 engine(rd());
    uniform_int_distribution<> dist(0, possible_characters.size()-1);
    string ret = "";
    for(int i = 0; i < max_length; i++){
        int random_index = dist(engine); //get index between 0 and possible_characters.size()-1
        ret += possible_characters[random_index];
    }
    return ret;
}

template<typename Numeric, typename Generator = std::mt19937>
Numeric random(Numeric from, Numeric to)
{
    thread_local static Generator gen(std::random_device{}());

    using dist_type = typename std::conditional
    <
        std::is_integral<Numeric>::value
        , std::uniform_int_distribution<Numeric>
        , std::uniform_real_distribution<Numeric>
    >::type;

    thread_local static dist_type dist;

    return dist(gen, typename dist_type::param_type{from, to});
}

void exec (string command) {
    cout << command << endl;
    system (command.c_str());
}

void createacct (string acctname) {
    exec (cleos_cmd + " create account eosio " + acctname + " " + key + " " + key);
}

void issueequity (string acctname, double equity) {

    std::ostringstream out;
    out.precision(4);
    out << std::fixed << equity;
    
    exec (cleos_cmd + " push action equitytoken issue '[\"" + acctname + "\", \"" + out.str() + " E\", \"initial issue\"]\' -p issuer");
}

void issueutility (string acctname) {
    exec (cleos_cmd + " push action utilitytoken issue '[\"" + acctname + "\", \"15.0000 U\", \"initial issue\"]\' -p issuer");
}

int main(int argc, char *argv[])
{
    double duration;
    auto start = std::chrono::system_clock::now();
    srand(time(NULL));
    string issuer = "issuer";
    int count = stoi(argv[1]);

    vector<string> accounts;

    exec ( eosiocpp_cmd + " -g /eosdev/equitytoken/equitytoken.abi /eosdev/equitytoken/equitytoken.hpp");
    exec ( eosiocpp_cmd + " -o /eosdev/equitytoken/equitytoken.wast /eosdev/equitytoken/equitytoken.cpp");
    exec ( eosiocpp_cmd + " -g /eosdev/mintabletoken/mintabletoken.abi /eosdev/mintabletoken/mintabletoken.hpp");
    exec ( eosiocpp_cmd + " -o /eosdev/mintabletoken/mintabletoken.wast /eosdev/mintabletoken/mintabletoken.cpp");

    createacct ("utilitytoken");
    createacct ("equitytoken");
    createacct (issuer);

    exec (cleos_cmd + " push action eosio updateauth '{\"account\":\"equitytoken\",\"permission\":\"active\",\"parent\":\"owner\",\"auth\":{\"keys\":[{\"key\":\"EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn\", \"weight\":1}],\"threshold\":1,\"accounts\":[{\"permission\":{\"actor\":\"equitytoken\",\"permission\":\"eosio.code\"},\"weight\":1}],\"waits\":[]}}' -p equitytoken");
        
    exec (cleos_cmd + " set contract equitytoken /eosdev/equitytoken");
    exec (cleos_cmd + " set contract utilitytoken /eosdev/mintabletoken");

    exec (cleos_cmd + " push action equitytoken setconfig '[10000, 4, \"E\", 4, \"U\", \"utilitytoken\"]' -p equitytoken");
    exec (cleos_cmd + " push action utilitytoken create '[\"" + issuer + "\", \"U\", 4]' -p utilitytoken");
    exec (cleos_cmd + " push action equitytoken create '[\"" + issuer + "\", \"100000000.0000 E\"]' -p equitytoken");

    double issued_equity = 0;

    for (int i=0; i< count; i++) {
        string username = generate(12);
        createacct (username);
        double equity_amount = random<double>(25.0000, 12000.0000);
        issued_equity += equity_amount;
        issueequity (username, equity_amount);

        accounts.emplace_back(username);
        equity_balances.insert (std::make_pair(username, equity_amount));
        

      //  std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    double utilityissued = 150000.0000;
    std::ostringstream out;
    out.precision(4);
    out << std::fixed << utilityissued;
    
    exec (cleos_cmd + " push action utilitytoken issue '[\"equitytoken\", \"" + out.str() + " U \", \"issuance\"]' -p " + issuer);
    int counter = 0;
    double rewards_paid = 0;
    for (std::vector<string>::iterator it = accounts.begin() ; it != accounts.end(); ++it) {
        
        // if (counter == 3) {
        //     issueequity (*it, random<double>(25.0000, 1200.0000));
        // }

        double reward = equity_balances.at(*it) / issued_equity * utilityissued;
        rewards_paid += reward;
        reward_balances.insert (std::make_pair(*it, reward));
        exec (cleos_cmd + " push action equitytoken withrewards '[\"" + *it + "\"]' -p " + *it);
        cout << "\nPayment of reward to " << *it << " :  ";
        cout << fixed << reward << endl << endl;
        counter++;
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    cout << "::: Elapsed time: " << elapsed_seconds.count() << "s";
    cout << endl;
}
