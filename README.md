
# Reward Token
The reward token is able to distribute revenue from a utility token equally to the holders of the equity token.

# To build Docker
docker-compose build

# To run containers
docker-compose up -d

# To unlock wallet (use your password)
docker exec -it rewardtoken_nodeosd_1 /opt/eosio/bin/cleos -u http://nodeosd:8888 --wallet-url http://keosd:8900 wallet unlock --password 

# To view live logs
docker logs -f rewardtoken_nodeosd_1

# Setup the following aliases
alias cleos='docker exec -it rewardtoken_nodeosd_1 /opt/eosio/bin/cleos -u http://nodeosd:8888 --wallet-url http://keosd:8900'
alias eosiocpp='docker exec -it rewardtoken_nodeosd_1 eosiocpp'

# Here is the command to compile
eosiocpp -g /eosdev/equitytoken/equitytoken.abi /eosdev/equitytoken/equitytoken.hpp && eosiocpp -o /eosdev/equitytoken/equitytoken.wast /eosdev/equitytoken/equitytoken.cpp
eosiocpp -g /eosdev/utilitytoken/utilitytoken.abi /eosdev/utilitytoken/utilitytoken.hpp && eosiocpp -o /eosdev/utilitytoken/utilitytoken.wast /eosdev/utilitytoken/utilitytoken.cpp
