#pragma once

class IDiagTool
{
public:
    virtual ~IDiagTool() {}

    virtual void CreateDiagnosticPackage() = 0;
};
