// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// Copyright (c) 2017-2018 Strizhniou Fiodar.
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors: Strizhniou Fiodar - fix build and runtime errors.
//
// Description:
// Implementation of the Class ElfLocalRelocation for the elf2e32 tool
// @internalComponent
// @released
//
//

#include <portable.h>
#include "pl_elflocalrelocation.h"
#include "pl_elfimage.h"

/**
Constructor for class ElfLocalRelocation
@param aElfImage - Instance of class ElfImage
@param aAddr    - location where the relocation refers to.
@param aAddend  - addend for the relocation entry
@param aIndex   - symbol index
@param aRelType - Relocation type
@param aRel     - Elf relocation entry
@internalComponent
@released
*/
ElfLocalRelocation::ElfLocalRelocation(ElfImage *aElfImage, PLMemAddr32 aAddr,
		PLUINT32 aAddend, PLUINT32 aIndex, PLUCHAR aRelType,
		Elf32_Rel* aRel, bool aVeneerSymbol):
		ElfRelocation(aElfImage, aAddr, aAddend, aIndex, aRelType, aRel)
{
	iSegment = aElfImage->GetSegmentAtAddr( iAddr );
	iSegmentType = aElfImage->SegmentType( iAddr );
	iSymbol = &(aElfImage->iElfDynSym[iSymNdx]);
	iDelSym = false;
	iVeneerSymbol = aVeneerSymbol;
}

/**
Constructor for class ElfLocalRelocation
@param aElfImage - Instance of class ElfImage
@param aAddr    - location where the relocation refers to.
@param aAddend  - addend for the relocation entry
@param aIndex
@param aRelType - Relocation type
@param aRel     - Elf relocation entry
@param aSegmentType - Segment type
@param aSym			- Elf symbol
@param aDelSym		- indicate if the symbol is to be deleted by this relocation.
@internalComponent
@released
*/
ElfLocalRelocation::ElfLocalRelocation(ElfImage *aElfImage, PLMemAddr32 aAddr,
			PLUINT32 aAddend, PLUINT32 aIndex, PLUCHAR aRelType,
			Elf32_Rel* aRel, ESegmentType aSegmentType, Elf32_Sym* aSym,bool aDelSym, bool aVeneerSymbol):
		ElfRelocation(aElfImage, aAddr, aAddend, aIndex, aRelType, aRel)
{
	iSegmentType = aSegmentType;
	iSegment = aElfImage->Segment(aSegmentType);
	//iSymbol = &(aElfImage->iElfDynSym[iSymNdx]);
	iSymbol = aSym;
	iDelSym = aDelSym;
	iVeneerSymbol = aVeneerSymbol;
}


ElfLocalRelocation::~ElfLocalRelocation()
{
	if(iDelSym)
	{
		DELETE_PTR(iSymbol);
	}
}

/**
This function adjusts the fixup for the relocation entry.
@return - Relocation type
@internalComponent
@released
*/
PLUINT16 ElfLocalRelocation::Fixup()
{
	if(!ExportTableReloc() && !iVeneerSymbol)
	{
		Elf32_Word * aLoc = iElfImage->GetFixupLocation(this, iAddr);
		Elf32_Word aLocVal = * aLoc;

		if (iRelType == R_ARM_ABS32 || iRelType == R_ARM_GLOB_DAT )
		{

			Elf32_Word aFixedVal = aLocVal + iSymbol->st_value;
			*aLoc = aFixedVal;
		}
	}

	ESegmentType aType;
	if( iSymbol )
		aType = iElfImage->Segment(iSymbol);
	else
		aType = iSegmentType;

	if (aType == ESegmentRO)
		return KTextRelocType;
	else if (aType == ESegmentRW)
		return KDataRelocType;

	// maybe this should be an error
	return KInferredRelocType;
}

/**
Function for export table relocation
@return - True
@internalComponent
@released
*/
bool ElfLocalRelocation::ExportTableReloc()
{
	return iRel == nullptr;
}
