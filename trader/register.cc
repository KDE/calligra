#include "register.h"
#include "trader_main.h"

Register::Register( Trader *_trader ) : ::TraderComponents( _trader ), ::SupportAttributes( _trader ),
	                                CosTrading::Register_skel()
{
}

char* Register::export( CORBA::Object_ptr reference, const char* type,
			const CosTrading::PropertySeq& properties )
{
  if ( CORBA::is_nil( reference ) )
  {
    CosTrading::Register::InvalidObjectRef exc;
    exc.ref = reference;
    mico_throw( exc );
  }
       
  Offer *offer = new Offer;
  offer->vReference = CORBA::Object::_duplicate( reference );
  offer->vType = CORBA::string_dup( type );
  offer->properties = properties;
  offer->isProxy = false;
  
  m_pTrader->export( offer );

  return CORBA::string_dup( offer->vOfferId.in() );
}

void Register::withdraw( const char* id )
{
  m_pTrader->remove( id );
}

CosTrading::Register::OfferInfo* Register::describe( const char* id )
{
  return m_pTrader->describe( id );
}

void Register::modify( const char* id, const CosTrading::PropertyNameSeq& del_list,
		       const CosTrading::PropertySeq& modify_list )
{
  m_pTrader->modify( id, del_list, modify_list );
}

void Register::withdraw_using_constraint( const char* type, const char* constr )
{
  m_pTrader->withdraw_using_constraint( type, constr );
}

CosTrading::Register_ptr Register::resolve( const CosTrading::TraderName& name )
{
  CosTrading::Register::UnknownTraderName exc;
  exc.name = name;
  mico_throw( exc );
}

