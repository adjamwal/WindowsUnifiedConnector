#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#define stringify( name ) # name

template<typename T>
struct EnumStrings
{
    static char const* data[];
};

template<typename T>
struct EnumRefHolder
{
    T& enumVal;
    EnumRefHolder( T& enumVal ) : enumVal( enumVal ) {}
};

template<typename T>
struct EnumConstRefHolder
{
    T const& enumVal;
    EnumConstRefHolder( T const& enumVal ) : enumVal( enumVal ) {}
};

template<typename T>
std::ostream& operator<<( std::ostream& str, EnumConstRefHolder<T> const& data )
{
    return str << EnumStrings<T>::data[ data.enumVal ];
}

template<typename T>
std::istream& operator>>( std::istream& str, EnumRefHolder<T> const& data )
{
    std::string value;
    str >> value;

    static auto begin = std::begin( EnumStrings<T>::data );
    static auto end = std::end( EnumStrings<T>::data );

    auto find = std::find( begin, end, value );
    if( find != end )
    {
        data.enumVal = static_cast< T >( std::distance( begin, find ) );
    }
    return str;
}

template<typename T>
std::string EnumToString( T const& e )
{ 
    std::stringstream ss;
    ss << EnumConstRefHolder<T>( e );
    return ss.str();
}

template<typename T>
EnumRefHolder<T> EnumFromString( T& e )
{
    return EnumRefHolder<T>( e );
}
