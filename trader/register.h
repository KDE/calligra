#ifndef __register_h__
#define __register_h__

class Trader;
class Register;

#include "misc.h"

class Register : virtual public TraderComponents, virtual public SupportAttributes,
		 virtual public CosTrading::Register_skel
{
public:
  Register( Trader *_trader );
  
  char* export( CORBA::Object_ptr reference, const char* type,
		const CosTrading::PropertySeq& properties );
  void withdraw( const char* id );
  CosTrading::Register::OfferInfo* describe( const char* id );
  void modify( const char* id, const CosTrading::PropertyNameSeq& del_list,
	       const CosTrading::PropertySeq& modify_list );
  void withdraw_using_constraint( const char* type, const char* constr );
  CosTrading::Register_ptr resolve( const CosTrading::TraderName& name );
};

#endif
