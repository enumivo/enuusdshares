#include <enulib/currency.hpp>
#include <enulib/enu.hpp>
#include <vector>

#define ENU_SYMBOL S(4, ENU)  
#define USD_SYMBOL S(4, USD)  
#define ENUUSD_SYMBOL S(4, ENUUSD)  


using namespace enumivo;

class ex : public contract {
 public:
  ex(account_name self)
      : contract(self) {}

  void receivedenu(const currency::transfer& transfer);
  void receivedusd(const currency::transfer& transfer);
  void receivedshares(const currency::transfer& transfer);

  void apply(account_name contract, action_name act);

};
