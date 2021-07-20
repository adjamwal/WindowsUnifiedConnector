// UCIDConsumerApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <memory>
#include "CodesignVerifier.h"
#include "UCIDApiDll.h"

int main()
{
    std::string id;
    std::string token;
    std::string temp;

    auto codeSignVerifier = std::make_unique<CodesignVerifier>( CodesignVerifier() );
    auto ucidApi = std::make_unique<UCIDApiDll>( UCIDApiDll( *codeSignVerifier ) );

    ucidApi->LoadApi();
    ucidApi->RefreshToken();

    int32_t retCode = 0;

    std::cout << "Started UCIDConsumerApp." << std::endl;

    retCode = ucidApi->GetId( id );
    std::cout << "Id: " << id << std::endl;

    retCode = ucidApi->GetToken( token );
    std::cout << "Token: " << token << std::endl;

    std::cout << "Press Enter to release dll and exist." << std::endl;
    std::cin >> temp;

    ucidApi->UnloadApi();
}
