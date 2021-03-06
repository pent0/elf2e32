// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// Copyright (c) 2017 Strizhniou Fiodar.
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors: Strizhniou Fiodar - fix build and runtime errors, refactoring.
//
// Description:
// Implementation of the Class ElfExports for the elf2e32 tool
// @internalComponent
// @released
//
//

#include <cstring>
#include "pl_elfexports.h"
#include "pl_elfimage.h"
#include "pl_symbol.h"

using std::set_difference;

/**
Constructor for class ElfExports
@internalComponent
@released
*/
ElfExports::ElfExports() : iDllName(nullptr), iSorted(false), iExportsFilteredP(false){
}

/**
Destructor for class ElfExports
@internalComponent
@released
*/
ElfExports::~ElfExports()
{
	if(iElfExports.size())
	{
		Exports::iterator aItr = iElfExports.begin();
		Exports::iterator last = iElfExports.end();
		Symbol *temp;

		while( aItr != last)
		{
			temp = *aItr;
			++aItr;
			delete temp;
			temp = nullptr;
		}
	}
	iElfExports.clear();

}

/**
This function validates exported symbols. The typeinfo name strings
are not valid export symbols and are discarded.
@param aExecutable - Instance of class ElfImage
@param aSym	- Symbol
@return True if symbol is valid, otherwise false
@internalComponent
@released
*/
bool ElfExports::IsValidExport(ElfImage * aElfImage, Symbol * aSym)
{
	char * aSymName = aElfImage->GetSymbolName(aSym->iSymbolIndex);
	int result = strncmp(aSymName, "_ZTS", strlen("_ZTS"));
	return ( result != 0);
}

/**
This function adds export symbols into exports list.
@param aDll - Dll name
@param aExecutable - Instance of class ElfImage
@param aSym - Dll symbol
@return Dll Symbol if its valid, otherwise nullptr
@internalComponent
@released
*/
Symbol* ElfExports::Add(char *aDll, ElfImage * aExecutable, Symbol *aSym)
{
	if (IsValidExport(aExecutable, aSym))
	{
		if( !iDllName )
			iDllName = aDll;

		iElfExports.push_back(aSym);
		iSorted = false;
		return aSym;
	}
	return nullptr;
}

/**
Function to add elf exports
@param aDll - Dll name
@param aSym - Dll symbol
@internalComponent
@released
*/
void ElfExports::Add(char *aDll, Symbol *aSym)
{
	if( !iDllName )
		iDllName = aDll;
	iElfExports.push_back(aSym);
	iSorted = false;
}

/**
Function to sort elf exports. Sorting may be done based on the symbol
name or on the ordinal number depending on the usecase.
@internalComponent
@released
*/
void ElfExports::Sort()
{
	if (!iSorted) {
		if(iExportsFilteredP) {
			std::sort(iFilteredExports.begin(), iFilteredExports.end(), PtrELFExportNameCompare());
		}
		else {
			std::sort(iElfExports.begin(), iElfExports.end(), PtrELFExportNameCompare());
		}
		iSorted = true;
	}
}

/**
Function to get exports list.
@param aSorted - sort before returning the exports.
@return export list
@internalComponent
@released
*/
ElfExports::Exports & ElfExports::GetExports(bool aSorted)
{
	if (aSorted) Sort();

	if(iExportsFilteredP)
		return iFilteredExports;
	else
		return iElfExports;
}

/**
Function to get exports in ordinal order
@return export list
@internalComponent
@released
*/
ElfExports::Exports & ElfExports::GetExportsInOrdinalOrder()
{
	if (iExportsFilteredP)
	{
		std::sort(iFilteredExports.begin(), iFilteredExports.end(), PtrELFExportOrdinalCompare());
		return iFilteredExports;
	}
	else
	{
		std::sort(iElfExports.begin(), iElfExports.end(), PtrELFExportOrdinalCompare());
		return iElfExports;
	}
}

/**
Function to process the filtered exports
@internalComponent
@released
*/
void ElfExports::FilterExports()
{
	std::sort(iElfExports.begin(), iElfExports.end(), PtrELFExportNameCompare());
	std::sort(iFilteredExports.begin(), iFilteredExports.end(), PtrELFExportNameCompare());

	Exports aNewList(iElfExports.size());
	Exports::iterator aNewListBegin = aNewList.begin();

	Exports::iterator aNewListEnd = set_difference(iElfExports.begin(), iElfExports.end(), \
		iFilteredExports.begin(), iFilteredExports.end(), aNewListBegin, PtrELFExportNameCompare());

	iFilteredExports.clear();
	while (aNewListBegin != aNewListEnd)
	{
		iFilteredExports.push_back(*aNewListBegin);
		++aNewListBegin;
	}
}

/**
Function to get number of exports
@return size of export list
@internalComponent
@released
*/
size_t ElfExports::GetNumExports()
{
	return iElfExports.size();
}

/**
Function to get Dll name
@return Dll name
@internalComponent
@released
*/
char* ElfExports::DllName()
{
	return iDllName;
}

/**
Overloaded operator to compare ELF export names.
@return True if lhs symbol name < rhs symbol name, otherwise false
@internalComponent
@released
*/
bool ElfExports::PtrELFExportNameCompare::operator()(const Symbol * lhs, const Symbol * rhs) const
{
	return strcmp( lhs->SymbolName(), rhs->SymbolName()) < 0;
}

/**
Overloaded operator to compare ordinal numbers of symbols.
@return True if lhs symbol name < rhs symbol name, otherwise false
@internalComponent
@released
*/
bool ElfExports::PtrELFExportOrdinalCompare::operator()(const Symbol * lhs, const Symbol * rhs) const
{
	return lhs->OrdNum() < rhs->OrdNum();
}

/**
Overloaded operator to compare and update symbol attributes that are
being compared. The comparision is done on the symbol names.
@return True if lhs symbol name < rhs symbol name, otherwise false
@internalComponent
@released
*/
bool ElfExports::PtrELFExportNameCompareUpdateAttributes::operator()(Symbol * lhs, Symbol * rhs) const
{
	int result = strcmp(lhs->SymbolName(), rhs->SymbolName());
	if (!result)
	{
		if (lhs->OrdNum() > 0)
			rhs->SetOrdinal( lhs->OrdNum());
		else if (rhs->OrdNum() > 0)
			lhs->SetOrdinal( rhs->OrdNum());

		if( lhs->Absent() )
			rhs->SetAbsent(true);
		else if ( rhs->Absent() )
			lhs->SetAbsent(true);

		if( lhs->SymbolSize() )
			rhs->SetSymbolSize(lhs->SymbolSize());
		else if( rhs->SymbolSize() )
			lhs->SetSymbolSize(rhs->SymbolSize());
	}
	return result < 0;
}

