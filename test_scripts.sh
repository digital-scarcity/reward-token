

cleos create account eosio utilitytoken EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio equitytoken EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio user EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio user1 EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio user2 EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio user3 EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn

eosiocpp -g /eosdev/equitytoken/equitytoken.abi /eosdev/equitytoken/equitytoken.hpp && eosiocpp -o /eosdev/equitytoken/equitytoken.wast /eosdev/equitytoken/equitytoken.cpp
eosiocpp -g /eosdev/utilitytoken/utilitytoken.abi /eosdev/utilitytoken/utilitytoken.hpp && eosiocpp -o /eosdev/utilitytoken/utilitytoken.wast /eosdev/utilitytoken/utilitytoken.cpp

cleos set contract equitytoken /eosdev/equitytoken
cleos set contract utilitytoken /eosdev/utilitytoken

cleos push action equitytoken setconfig '[10000, 4, "E", 4, "U", "utilitytoken"]' -p equitytoken

# Setup U
cleos push action utilitytoken create '["user", "1000000000.0000 U"]' -p utilitytoken
cleos push action equitytoken create '["user", "1000000000.0000 E"]' -p equitytoken
cleos push action utilitytoken issue '["user", "10000000.0000 U", "memo"]' -p user

cleos get table equitytoken user accounts

cleos push action eosio updateauth '{"account":"equitytoken","permission":"active","parent":"owner","auth":{"keys":[{"key":"EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn", "weight":1}],"threshold":1,"accounts":[{"permission":{"actor":"equitytoken","permission":"eosio.code"},"weight":1}],"waits":[]}}' -p equitytoken

# Balance check
cleos get table equitytoken user1 accounts
cleos get table equitytoken user2 accounts
cleos get table equitytoken user3 accounts

cleos get table utilitytoken user1 accounts
cleos get table utilitytoken user2 accounts
cleos get table utilitytoken user3 accounts

# Round 1
## Issue first
cleos push action equitytoken issue '["user1", "250.0000 E"]' -p user
cleos push action equitytoken issue '["user2", "250.0000 E"]' -p user
cleos push action utilitytoken transfer '["user", "equitytoken", "500000.0000 U", "memo"]' -p user
cleos push action equitytoken update '["user1"]' -p user1
cleos push action equitytoken update '["user2"]' -p user2
cleos push action equitytoken withrewards '["user1"]' -p user1
cleos push action equitytoken withrewards '["user2"]' -p user2

cleos get table utilitytoken user1 accounts
cleos get table utilitytoken user2 accounts
cleos get table utilitytoken user3 accounts


# Round 2
cleos push action equitytoken issue '["user2", "250.0000 E"]' -p user
cleos push action utilitytoken transfer '["user", "equitytoken", "500000.0000 U", "memo"]' -p user
cleos push action equitytoken update '["user1"]' -p user1
cleos push action equitytoken update '["user2"]' -p user2
cleos push action equitytoken withrewards '["user1"]' -p user1
cleos push action equitytoken withrewards '["user2"]' -p user2

cleos get table utilitytoken user1 accounts
cleos get table utilitytoken user2 accounts
cleos get table utilitytoken user3 accounts



# Round 3
cleos push action equitytoken issue '["user1", "350.0000 E"]' -p user
cleos push action equitytoken issue '["user2", "250.0000 E"]' -p user
cleos push action utilitytoken transfer '["user", "equitytoken", "450000.0000 U", "memo"]' -p user
cleos push action equitytoken update '["user1"]' -p user1
cleos push action equitytoken update '["user2"]' -p user2

cleos push action equitytoken withrewards '["user1"]' -p user1
cleos push action equitytoken withrewards '["user2"]' -p user2

cleos get table utilitytoken user1 accounts
cleos get table utilitytoken user2 accounts
cleos get table utilitytoken user3 accounts



# Round 4
cleos push action equitytoken issue '["user2", "75000.0000 E"]' -p user
cleos push action utilitytoken transfer '["user", "equitytoken", "400000.0000 U", "memo"]' -p user
cleos push action equitytoken update '["user1"]' -p user1
cleos push action equitytoken update '["user2"]' -p user2

cleos push action equitytoken withrewards '["user1"]' -p user1
cleos push action equitytoken withrewards '["user2"]' -p user2

cleos get table utilitytoken user1 accounts
cleos get table utilitytoken user2 accounts
cleos get table utilitytoken user3 accounts



# Round 5

# Round 6

# Round 7

# Round 8

# Round 9

# Round 10

# Round 11

# Round 12

