

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

cleos push action equitytoken addconfig '[3000, 4, "E", 4, "U", "utilitytoken"]' -p equitytoken

# Setup U
cleos push action utilitytoken create '["user", "1000000000.0000 U"]' -p utilitytoken
cleos push action utilitytoken issue '["user", "100000000.0000 U", "memo"]' -p user

cleos push action equitytoken create '["user", "1000000000.0000 E"]' -p equitytoken

cleos get table equitytoken user accounts

cleos push action eosio updateauth '{"account":"equitytoken","permission":"active","parent":"owner","auth":{"keys":[{"key":"EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn", "weight":1}],"threshold":1,"accounts":[{"permission":{"actor":"equitytoken","permission":"eosio.code"},"weight":1}],"waits":[]}}' -p equitytoken


# Week 1
cleos push action equitytoken issue '["user1", "100000.0000 E"]' -p user
cleos push action equitytoken issue '["user2", "200000.0000 E"]' -p user
cleos push action equitytoken issue '["user3", "5000.0000 E"]' -p user

cleos push action utilitytoken transfer '["user", "equitytoken", "20000.0000 U", "memo"]' -p user

# Week 2
cleos push action equitytoken issue '["user1", "2400000.0000 E"]' -p user
cleos push action equitytoken issue '["user3", "10000.0000 E"]' -p user

cleos push action utilitytoken transfer '["user", "equitytoken", "2500.0000 U", "memo"]' -p user


# Week 3
cleos push action equitytoken issue '["user2", "100000.0000 E"]' -p user
cleos push action equitytoken issue '["user3", "10000.0000 E"]' -p user

cleos push action utilitytoken transfer '["user", "equitytoken", "7000.0000 U", "memo"]' -p user


cleos push action equitytoken withrewards '["user1"]' -p user
cleos push action equitytoken withrewards '["user2"]' -p user2
cleos push action equitytoken withrewards '["user3"]' -p user3


