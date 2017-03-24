// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implementation of the Class ElfConsumer for the elf2e32 tool
// @internalComponent
// @released
//
//

#if !defined(_PL_ELFCONSUMER_H_)
#define _PL_ELFCONSUMER_H_

#include "pl_elfexecutable.h"
#include <list>
#include <stdio.h>

using std::list;

enum{ KMaxWindowsIOSize = 31 * 1024 * 1024 };

/**
This class is for reading the ELF file generated by the static linker and based on whether it is
ET_EXEC or ET_DYN, it processes the imports(if required) or exports respectively.
@internalComponent
@released
*/
class ElfConsumer : public ElfExecutable
{
public:
	ElfConsumer(ParameterManager* aParameterManager);
	virtual ~ElfConsumer();
	PLUINT32 ReadElfFile(char* aFile);
	int GetElfSymbolList(list<Symbol*>& aList);
	void GetImageDetails(/*E32ImageInterface aInterface*/);
	PLUINT32 ProcessElfFile();

private:
	char*		iMemBlock;
};




#endif // !defined(_PL_ELFCONSUMER_H_)
