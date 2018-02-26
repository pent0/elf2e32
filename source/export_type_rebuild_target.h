// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// Copyright (c) 2017 Strizhniou Fiodar
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
// Declaration of Class ExportTypeRebuildTarget of the elf2e32 tool
// @internalComponent
// @released
//
//

#ifndef EXPORT_TYPE_REBUILD_TARGET_H
#define EXPORT_TYPE_REBUILD_TARGET_H

#include "dll_type_target.h"

class DefFile;

/**
class ExportTypeRebuildTarget is dervied from DLLTarget and handles export type  target rebuilding

@internalComponent
@released
*/
class ExportTypeRebuildTarget : public DLLTarget
{
public:
	explicit ExportTypeRebuildTarget(ParameterManager* aParameterManager);
	virtual ~ExportTypeRebuildTarget();
	void ProcessExports();
protected:
	DefFile *iDefFile;
};

#endif // EXPORT_TYPE_REBUILD_TARGET_H

