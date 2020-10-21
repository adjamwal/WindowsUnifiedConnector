#pragma once

enum class PackageType
{
    EXE,
    MSI
};

//TODO Define these
struct PmComponent
{
    int i;
};

struct PmPackage
{
    PackageType Type;
    std::string Path;
    std::string Name;
    std::string CmdLine;
};

struct PmInstalledPackage
{

};

struct PmPackageConfigration
{

};