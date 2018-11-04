
#pragma once
#include <eosiolib/currency.hpp>

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>
#include <math.h>

using std::string;
using namespace eosio;

class equitytoken : public contract
{
  public:
    equitytoken(account_name self) : contract(self) {}

    // @abi action
    void create(account_name issuer,
                asset maximum_supply);

    // @abi action
    void issue(account_name to, asset quantity, string memo);

    // @abi action
    void transfer(account_name from,
                  account_name to,
                  asset quantity,
                  string memo);

    // @abi action
    void setconfig (const uint16_t  earnings_rate100,
                    const uint8_t   equity_symbol_precision,
                    const string    equity_symbol,
                    const uint8_t   reward_symbol_precision,
                    const string    reward_symbol,
                    const account_name  rewards_contract);
  
    // @abi action
    void setearnrate (const uint16_t  earnings_ratex100);

    // @abi action 
    void update (const account_name    account);

    // @abi action 
    void withrewards (const account_name    account);

    inline asset get_supply(symbol_name sym) const;

    inline asset get_balance(account_name owner, symbol_name sym) const;

    void apply(const account_name contract, const account_name act);

    void transferReceived(const currency::transfer &transfer, const account_name code);

  private:

    const uint128_t       SCALER = 1000000000000000000;

    // @abi table configs i64
    struct config
    {
        uint64_t        config_id;
        symbol_type     equity_symbol;
        account_name    rewards_contract;
        symbol_type     rewards_symbol;
        uint128_t       scaled_earnings_per_token;
        uint128_t       scaled_remainder;
        uint16_t        earnings_ratex100;
        //account_name    xferqueue;
     
        uint64_t primary_key() const { return config_id; }
    };

    typedef eosio::multi_index<N(configs), config> config_table;

    // ---- MAY NOT NEEDS THIS 
    // @abi table tholders i64
    struct tholder {
        account_name    tholder_account;
        account_name    primary_key () const { return tholder_account; }
    };

    typedef eosio::multi_index<N(tholders), tholder> tholder_table;

    // @abi table accounts i64
    struct account
    {
        asset balance;
        uint128_t scaled_earnings_balance;
        uint128_t scaled_earnings_credited;

        uint64_t primary_key() const { return balance.symbol.name(); }
    };

    typedef eosio::multi_index<N(accounts), account> accounts;

    // @abi table stat i64
    struct currencystat
    {
        asset supply;
        asset max_supply;
        account_name issuer;

        uint64_t primary_key() const { return supply.symbol.name(); }
    };

    typedef eosio::multi_index<N(stat), currencystat> stats;

    void sub_balance(account_name owner, asset value);
    void add_balance(account_name owner, asset value, account_name ram_payer);

     void payreward(const account_name from,
                  const account_name to,
                  const asset token_amount,
                  const string memo)
    {

        if (from == to) return;

        config_table config(_self, _self);
        auto itr = config.begin();
        eosio_assert(itr != config.end(), "Configuration is not set.");

        print("---------- Transfer -----------\n");
        print("Token Contract   :", name{itr->rewards_contract}, "\n");
        print("From             :", name{from}, "\n");
        print("To               :", name{to}, "\n");
        print("Amount           :", token_amount, "\n");
        print("Memo             :", memo, "\n");

        action(
            permission_level{from, N(active)},
            itr->rewards_contract, N(transfer),
            std::make_tuple(from, to, token_amount, memo))
            .send();

        print("---------- End Transfer -------\n");
    }

  public:
    struct transfer_args
    {
        account_name from;
        account_name to;
        asset quantity;
        string memo;
    };
};

asset equitytoken::get_supply(symbol_name sym) const
{
    stats statstable(_self, sym);
    const auto &st = statstable.get(sym);
    return st.supply;
}

asset equitytoken::get_balance(account_name owner, symbol_name sym) const
{
    accounts accountstable(_self, owner);
    const auto &ac = accountstable.get(sym);
    return ac.balance;
}



//EOSIO_ABI(equitytoken, (create)(issue)(transfer))