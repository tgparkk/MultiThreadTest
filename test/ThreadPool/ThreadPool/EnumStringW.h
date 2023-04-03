
//  EnumStringW - A utility to provide stringizing support for C++ enums
//  Author: Francis Xavier Joseph Pulikotil
//
//  This code is free software: you can do whatever you want with it,
//	although I would appreciate if you gave credit where it's due.
//
//  This code is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#ifndef ENUMSTRINGW_HEADER
#define ENUMSTRINGW_HEADER

/* Usage example

    // WeekEnd enumeration
    enum WeekEnd
    {
        Sunday = 1,
        Saturday = 7
    };

    // String support for WeekEnd
    Begin_Enum_StringW( WeekEnd )
    {
        Enum_StringW( Sunday );
        Enum_StringW( Saturday );
    }
    End_Enum_StringW;

    // Convert from WeekEnd to string
    const std::string16 &str = EnumStringW<WeekEnd>::From( Saturday );
	// str should now be "Saturday"

    // Convert from string to WeekEnd
    WeekEnd w;
    EnumStringW<WeekEnd>::To( w, "Sunday" );
	// w should now be Sunday
*/

//#include "stl_extension.h"
#include <map>
#include <cassert>

// Helper macros

#define Begin_Enum_StringW(EnumerationName)                                      \
    template <> struct EnumStringW<EnumerationName> :                            \
        public EnumStringWBase< EnumStringW<EnumerationName>, EnumerationName >   \
    {                                                                           \
        static void RegisterEnumerators()
//      {

#define Enum_StringW(EnumeratorName)                                             \
			RegisterEnumerator( EnumeratorName, L#EnumeratorName );
//      }

#define End_Enum_StringW                                                         \
    }

// The EnumStringW base class
template <class DerivedType, class EnumType>
class EnumStringWBase
{
// Types
protected:
    typedef std::map<std::wstring, EnumType> AssocMap;

protected:
// Constructor / Destructor
    explicit EnumStringWBase();
    ~EnumStringWBase();

private:
// Copy Constructor / Assignment Operator
    EnumStringWBase(const EnumStringWBase &);
    const EnumStringWBase &operator =(const EnumStringWBase &);

// Functions
private:
    static AssocMap &GetMap();

protected:
    // Use this helper function to register each enumerator
    // and its string representation.
    static void RegisterEnumerator(const EnumType e, const std::wstring &eStr);

public:
    // Converts from an enumerator to a string.
    // Returns an empty string if the enumerator was not registered.
    static const std::wstring &From(const EnumType e);

    // Converts from a string to an enumerator.
    // Returns true if the conversion is successful; false otherwise.
    static const bool To(EnumType &e, const std::wstring &str);
};

// The EnumStringW class
// Note: Specialize this class for each enumeration, and implement
//       the RegisterEnumerators() function.
template <class EnumType>
struct EnumStringW : public EnumStringWBase< EnumStringW<EnumType>, EnumType >
{
    static void RegisterEnumerators();
};

// Function definitions

template <class D, class E>
typename EnumStringWBase<D,E>::AssocMap &EnumStringWBase<D,E>::GetMap()
{
    // A static map of associations from strings to enumerators
    static AssocMap assocMap;
    static bool     bFirstAccess = true;

    // If this is the first time we're accessing the map, then populate it.
    if( bFirstAccess )
    {
        bFirstAccess = false;
        D::RegisterEnumerators();
        assert( !assocMap.empty() );
    }

    return assocMap;
}

template <class D, class E>
void EnumStringWBase<D,E>::RegisterEnumerator(const E e, const std::wstring &eStr)
{
    const bool bRegistered = GetMap().insert( typename AssocMap::value_type( eStr, e ) ).second;
    assert( bRegistered );
    (void)sizeof( bRegistered ); // This is to avoid the pesky 'unused variable' warning in Release Builds.
}

template <class D, class E>
const std::wstring &EnumStringWBase<D,E>::From(const E e)
{
    for(;;) // Code block
    {
        // Search for the enumerator in our map
        typename AssocMap::const_iterator i;
        for(i = GetMap().begin(); i != GetMap().end(); ++i)
            if( (*i).second == e )
                break;

        // If we didn't find it, we can't do this conversion
        if( i == GetMap().end() )
            break;

        // Keep searching and see if we find another one with the same value
        typename AssocMap::const_iterator j( i );
        for(++j; j != GetMap().end(); ++j)
            if( (*j).second == e )
                break;

        // If we found another one with the same value, we can't do this conversion
        if( j != GetMap().end() )
            break;

        // We found exactly one string which matches the required enumerator
        return (*i).first;
    }

    // We couldn't do this conversion; return an empty string.
    static const std::wstring dummy;
    return dummy;
}

template <class D, class E>
const bool EnumStringWBase<D,E>::To(E &e, const std::wstring &str)
{
    // Search for the string in our map.
    const typename AssocMap::const_iterator itr( GetMap().find( str ) );

    // If we have it, then return the associated enumerator.
    if( itr != GetMap().end() )
    {
        e = (*itr).second;
        return true;
    }

    // We don't have it; the conversion failed.
    return false;
}

#endif
