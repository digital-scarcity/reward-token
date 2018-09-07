
#include "equitytoken.hpp"

using namespace std;
using namespace eosio;

void equitytoken::create(account_name issuer,
                       asset maximum_supply)
{
    require_auth(_self);

    auto sym = maximum_supply.symbol;
    eosio_assert(sym.is_valid(), "invalid symbol name");
    eosio_assert(maximum_supply.is_valid(), "invalid supply");
    eosio_assert(maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable(_self, sym.name());
    auto existing = statstable.find(sym.name());
    eosio_assert(existing == statstable.end(), "equitytoken with symbol already exists");

    statstable.emplace(_self, [&](auto &s) {
        s.supply.symbol = maximum_supply.symbol;
        s.max_supply = maximum_supply;
        s.issuer = issuer;
    });
}

void equitytoken::issue(account_name to, asset quantity, string memo)
{
    auto sym = quantity.symbol;
    eosio_assert(sym.is_valid(), "invalid symbol name");
    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    auto sym_name = sym.name();
    stats statstable(_self, sym_name);
    auto existing = statstable.find(sym_name);
    eosio_assert(existing != statstable.end(), "equitytoken with symbol does not exist, create equitytoken before issue");
    const auto &st = *existing;

    require_auth(st.issuer);
    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must issue positive quantity");

    eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
    eosio_assert(quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

    statstable.modify(st, 0, [&](auto &s) {
        s.supply += quantity;
    });

    add_balance(st.issuer, quantity, st.issuer);

    if (to != st.issuer)
    {
        SEND_INLINE_ACTION(*this, transfer, {st.issuer, N(active)}, {st.issuer, to, quantity, memo});
    }
}

void equitytoken::transfer(account_name from,
                         account_name to,
                         asset quantity,
                         string memo)
{
    eosio_assert(from != to, "cannot transfer to self");
    require_auth(from);
    eosio_assert(is_account(to), "to account does not exist");
    auto sym = quantity.symbol.name();
    stats statstable(_self, sym);
    const auto &st = statstable.get(sym);

    update (from);
    update (to);

    require_recipient(from);
    require_recipient(to);

    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must transfer positive quantity");
    eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    sub_balance(from, quantity);
    add_balance(to, quantity, from);
}

void equitytoken::sub_balance(account_name owner, asset value)
{
    accounts from_acnts(_self, owner);

    const auto &from = from_acnts.get(value.symbol.name(), "no balance object found");
    eosio_assert(from.balance.amount >= value.amount, "overdrawn balance");

    if (from.balance.amount == value.amount)
    {
        from_acnts.erase(from);
    }
    else
    {
        from_acnts.modify(from, owner, [&](auto &a) {
            a.balance -= value;
        });
    }
}

void equitytoken::add_balance(account_name owner, asset value, account_name ram_payer)
{
    config_table c_t (_self, _self);
    auto c_itr = c_t.begin();
    eosio_assert(c_itr != c_t.end(), "Config has not been set.");

    accounts to_acnts(_self, owner);
    auto to = to_acnts.find(value.symbol.name());
    if (to == to_acnts.end())
    {
        to_acnts.emplace(ram_payer, [&](auto &a) {
            a.balance = value;
            a.scaled_earnings_balance   =   0;
            a.scaled_earnings_credited  =   0;
        });
    }
    else
    {
        to_acnts.modify(to, 0, [&](auto &a) {
            a.balance += value;
        });
    }
}

void equitytoken::addconfig (const uint16_t earnings_ratex100,
                            const uint8_t   equity_symbol_precision,
                            const string    equity_symbol,
                            const uint8_t   reward_symbol_precision, 
                            const string    reward_symbol,
                            const account_name  rewards_contract) {

    config_table c_t (_self, _self);
    auto c_itr = c_t.begin();

    if (c_itr == c_t.end()) {
        c_t.emplace (_self, [&](auto &c) {
            c.earnings_ratex100         = earnings_ratex100;
            c.rewards_symbol            = string_to_symbol (reward_symbol_precision, reward_symbol.c_str());
            c.equity_symbol             = string_to_symbol (equity_symbol_precision, equity_symbol.c_str());
            c.scaled_earnings_per_token = 0;
            c.rewards_contract          = rewards_contract;
        });
    }
    else {
        c_t.modify (c_itr, _self, [&](auto &c) {
            c.earnings_ratex100     = earnings_ratex100;
        });
    }
}


void equitytoken::update (const account_name account) {

    config_table c_t (_self, _self);
    auto c_itr = c_t.begin();
    eosio_assert(c_itr != c_t.end(), "Config has not been set.");

    accounts acnts(_self, account);
    auto a_itr = acnts.find(c_itr->equity_symbol.name());
    if (a_itr == acnts.end()) {
        return;
    }

    uint128_t    owed   = c_itr->scaled_earnings_per_token - a_itr->scaled_earnings_credited;
    
    acnts.modify (a_itr, _self, [&](auto &a) {
        a.scaled_earnings_balance += (a.balance.amount)  * owed;
        a.scaled_earnings_credited  = c_itr->scaled_earnings_per_token; 
    });
}

void equitytoken::withrewards (const account_name account) {
    update (account);
    
    config_table c_t (_self, _self);
    auto c_itr = c_t.begin();
    eosio_assert(c_itr != c_t.end(), "Config has not been set.");
    
    accounts acnts(_self, account);
    auto a_itr = acnts.find(c_itr->equity_symbol.name());
    eosio_assert (a_itr != acnts.end(), "Account does not have a token balance.");
    uint128_t   amount = a_itr->scaled_earnings_balance / SCALER;
   
    acnts.modify (a_itr, _self, [&](auto &a) {
        a.scaled_earnings_balance %= SCALER;
    });
    
    payreward (_self, account, asset { static_cast<int64_t>(amount), c_itr->rewards_symbol}, "Reward from BCDE Holdings");
}


void equitytoken::apply(const account_name contract, const account_name act)
{

    if (contract == _self) {
        auto &thiscontract = *this;

        switch (act)
        {
            EOSIO_API(equitytoken, (create)(issue)(transfer)(addconfig)(update)(withrewards))
        };
    } else {

        if (act == N(transfer))
        {
            transferReceived(unpack_action_data<currency::transfer>(), contract);
            return;
        }
    }
    
}

void equitytoken::transferReceived(const currency::transfer &transfer, const account_name code)
{
    if (transfer.to != _self)
    {
        return;
    }

    print("Account Name     :   ", name{code}, "\n");
    print("From             :   ", name{transfer.from}, "\n");
    print("To               :   ", name{transfer.to}, "\n");
    print("Asset            :   ", transfer.quantity, "\n");
    print("Received Amount  :   ", transfer.quantity.amount, "\n");
    print("Received Symbol  :   ", transfer.quantity.symbol, "\n");
    print("Memo             :   ", transfer.memo, "\n");

    config_table c_t(_self, _self);
    auto c_itr = c_t.find(0);
    eosio_assert (c_itr != c_t.end(), "Configuration has not been set.");

    stats s_t (_self, c_itr->equity_symbol.name());
    auto s_itr = s_t.find (c_itr->equity_symbol.name());
    eosio_assert (s_itr != s_t.end(), "Equity token symbol is not found in stat table.");

    uint128_t   available = (c_itr->earnings_ratex100 * transfer.quantity.amount * SCALER / 10000 / 10000 ) + c_itr->scaled_remainder;

    c_t.modify (c_itr, _self, [&](auto &c) {
        c.scaled_earnings_per_token     += available / (s_itr->supply.amount / 10000);
        c.scaled_remainder              = available % (s_itr->supply.amount / 10000);
     });
}

extern "C"
{
    //using namespace bay;
    using namespace eosio;

    void apply(uint64_t receiver, uint64_t code, uint64_t action)
    {
        auto self = receiver;
        equitytoken contract(self);
        contract.apply(code, action);
        eosio_exit(0);
    }
}